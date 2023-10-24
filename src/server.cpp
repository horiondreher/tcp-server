#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

std::string read_message(tcp::socket& socket) {
    streambuf buf;
    read_until(socket, buf, "\n");
    std::string data = buffer_cast<const char*>(buf.data());

    return data;
}

void send_message(tcp::socket& socket, const std::string& message) {
    const std::string msg = message + "\n";
    write(socket, buffer(message));
}

/**
 * acceptor create a object to listen on a port for incoming connection
 * socket creates using the acceptor object
*/
int main()
{
    io_service io_service;
    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 8080));
    tcp::socket socket(io_service);

    acceptor.accept(socket);

    std::string message = read_message(socket);
    std::cout << message << std::endl;

    send_message(socket, "Hello from server!");

    return 0;
}