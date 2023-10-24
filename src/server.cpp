#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
private:
    tcp::socket socket_;
    deadline_timer timer_;
    std::string message_;

    enum { max_length = 1024 };
    char data_[max_length];

public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    tcp_connection(io_context& io_context) : socket_(io_context), timer_(io_context) {}
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
        // async_read(
        //     socket_,
        //     buffer(data_, max_length),
        //     boost::bind(&tcp_connection::handle_read,
        //                 shared_from_this(),
        //                 placeholders::error,
        //                 placeholders::bytes_transferred));

        // socket_.async_read_some(
        //     boost::asio::buffer(data_, max_length),
        //     boost::bind(&tcp_connection::handle_read,
        //                 shared_from_this(),
        //                 boost::asio::placeholders::error,
        //                 boost::asio::placeholders::bytes_transferred));
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred) {
        if (!err) {
            std::cout << "Client says: " << data_ << std::endl;
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