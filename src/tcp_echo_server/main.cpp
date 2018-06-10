#include "tcp_echo_server.hpp"
#include <exception>
#include <iostream>

int main(int, char**) {
    try {
        TcpEchoServer server;
        const bool status = server.run();

        if (!status) {
            std::cout << "{main} Server shutdown with an error" << std::endl;
            return 1;
        }
    }
    catch ( const std::exception& e ) {
        std::cerr << "{main} Caught exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
