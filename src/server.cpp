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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost::asio;
using ip::tcp;

static const std::string config_file = "config.json";

/**
 * Singleton class to load config files and read without instanciating everytime
*/
class config {
public:
    int port = 0;
    int max_file_size = 32;
    std::string file_name_prefix = "";

    static config& get_instance() {
        static config instance;
        return instance;
    }

    bool load_config(const std::string& config_file) {
        try {
            boost::property_tree::read_json(config_file, config_);
            port = get_config_value<int>("server_port");
            max_file_size = get_config_value<int>("max_file_size");
            file_name_prefix = get_config_value<std::string>("file_name_prefix");

            return true;
        } catch (const boost::property_tree::ptree_error& e) {
            std::cerr << "Error reading config file: " << e.what() << std::endl;
            return false;
        }
    }

private:
    boost::property_tree::ptree config_;

    config() {}

    config(const config&) = delete;
    config& operator=(const config&) = delete;
    
    template <typename T>
    T get_config_value(const std::string& key) const {
        return config_.get<T>(key);
    }
};

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
private:
    tcp::socket socket_;
    deadline_timer timer_;

    streambuf buffer_;
    std::string file_name_;
    std::string client_endpoint_;

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

        std::string prefix = config::get_instance().file_name_prefix;
        file_name_ = prefix + "_" + client_endpoint_ + "_" + timestamp + ".txt";

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
            std::ofstream output_file("output/" + file_name_, std::ios::app);

            if(!output_file.is_open()) {
                std::cerr << "Error opening file" << std::endl;
                return;
            }

            std::cout << "Bytes received from " << client_endpoint_ << ": " << bytes_transferred << std::endl;
            size_t file_length = 0;
            size_t max_file_size = config::get_instance().max_file_size;

            while(input_stream) {
                input_stream.read(data_, max_length);
                std::streamsize read = input_stream.gcount();

                if(file_length + read > max_file_size) {
                    read = max_file_size - file_length;
                }

                output_file.write(data_, read);
                file_length += read;

                if(file_length >= max_file_size) {
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
    tcp_server(io_context& io_context, ip::port_type port) : 
        io_context_(io_context), 
        acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) 
    {
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
    std::cout << "Read config file" << std::endl;

    config& config = config::get_instance();
    if(!config.load_config(config_file)) {
        std::cerr << "Error loading config file, closing server" << std::endl;
        return 1;
    }

    std::cout << "Starting server on port: " << config.port << std::endl;

    try {
        io_context io_context;
        tcp_server server(io_context, config.port);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }

    return 0;
}