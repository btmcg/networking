#include "tcp_echo_server.hpp"
#include <exception>
#include <iostream>

int
main(int, char**)
{
    try {
        tcp_echo_server server;
        bool const status = server.run();

        if (!status) {
            std::cout << "{main} Server shutdown with an error" << std::endl;
            return 1;
        }
    } catch (std::exception const& e) {
        std::cerr << "{main} Caught exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
