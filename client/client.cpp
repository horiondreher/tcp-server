#include <iostream>
#include <boost/asio.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost::asio;
using ip::tcp;

int main(int argc, char** argv) {
    io_context io_context;
    tcp::socket socket(io_context);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server address> <port>" << std::endl;
        return 1;
    } 

    std::string host = argv[1];
    std::string port = argv[2];

    try {
        tcp::resolver resolver(io_context);
        connect(socket, resolver.resolve(host, port));

        std::string user_input;
        std::string line;

        std::cout << "Enter your message (press Enter twice to send):\n";

        while (std::getline(std::cin, line)) {
            user_input += line + "\n"; // Add a newline to simulate Enter keypress

            if (line.empty()) {
                // If an empty line is entered, break the loop
                break;
            }
        }

        // Send the input to the server
        write(socket, buffer(user_input));

        user_input.erase(user_input.size() - 2);
        std::cout << "Message sent to the server:\n" << user_input << std::endl;

        socket.close();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
