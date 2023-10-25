#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <iomanip>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/regex.hpp>

using namespace boost::asio;
using ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
private:
    tcp::socket socket_;
    deadline_timer timer_;

    streambuf buffer_;
    std::string message_;
    std::string file_name_;
    std::string client_endpoint_;
    size_t max_file_length_ = 32;

    enum { max_length = 1024 };
    char data_[max_length];

public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    tcp_connection(io_context& io_context) : socket_(io_context), timer_(io_context) { }
    ~tcp_connection() {
        std::cout << "Destroyed tcp_connection session\n";
    }

    static pointer create(io_context& io_context) {
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket& socket() {
        return socket_;
    }

    void start() {
        std::cout << "Created tcp_connection session\n";

        // Maybe change to UTC time
        std::time_t now = std::time(0);
        std::tm* now_tm = std::localtime(&now);
        std::stringstream ss;
        ss << std::put_time(now_tm, "%Y%m%d%H%M%S");
        std::string timestamp = ss.str();

        client_endpoint_ = socket_.remote_endpoint().address().to_string() +
            ":" + std::to_string(socket_.remote_endpoint().port());

        file_name_ = "CONNECTION_" + client_endpoint_ + "_" + timestamp + ".txt";

        async_read_until(
            socket_,
            buffer_,
            boost::regex("\n\n"),
            boost::bind(&tcp_connection::handle_read,
                        shared_from_this(),
                        placeholders::error,
                        placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred) {
        if (!err) {
            std::istream input_stream(&buffer_);
            std::ofstream output_file(file_name_, std::ios::app);

            if(!output_file.is_open()) {
                std::cerr << "Error opening file" << std::endl;
                return;
            }

            std::cout << "Bytes received from " << client_endpoint_ << ": " << bytes_transferred << std::endl;
            size_t file_length = 0;

            while(input_stream) {
                input_stream.read(data_, max_length);
                std::streamsize read = input_stream.gcount();

                if(file_length + read > max_file_length_) {
                    read = max_file_length_ - file_length;
                }

                output_file.write(data_, read);
                file_length += read;

                if(file_length >= max_file_length_) {
                    break;
                }
            }

            std::cout << "Message received from client, closing file" << std::endl;
            output_file.close();
        } else {
            std::cerr << "error: " << err.message() << std::endl;
        }
    }

    void handle_write(const boost::system::error_code& err, size_t bytes_transferred) {
        if (!err) {
            std::cout << "Message sent to client" << std::endl;
        }
    }
};

class tcp_server {
private:
    io_context& io_context_;
    tcp::acceptor acceptor_;

    void start_accept() {
        tcp_connection::pointer new_connection = boost::make_shared<tcp_connection>(io_context_);

        acceptor_.async_accept(
            new_connection->socket(),
            boost::bind(&tcp_server::handle_accept,
                        this,
                        new_connection,
                        placeholders::error));
    }

public:
    tcp_server(io_context& io_context): io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), 8080)) {
        start_accept();
    }

    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& err) {
        if (!err) {
            new_connection->start();
        }

        start_accept();
    }
};

/**
 * acceptor create a object to listen on a port for incoming connection
 * socket creates using the acceptor object
*/
int main() {
    std::cout << "Server started" << std::endl;

    try {
        io_context io_context;
        tcp_server server(io_context);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }

    return 0;
}