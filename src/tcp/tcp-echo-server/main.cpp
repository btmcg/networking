#include "tcp_echo_server.hpp"
#include <cstdio> // std::fprintf
#include <cstdlib> // EXIT_FAILURE, EXIT_SUCCESS
#include <exception>


int
main(int, char**)
{
    try {
        tcp_echo_server server;
        if (!server.run()) {
            std::fprintf(stderr, "error: server shutdown with an error\n");
            return EXIT_FAILURE;
        }
    } catch (std::exception const& e) {
        std::fprintf(stderr, "error: exception: %s\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        std::fprintf(stderr, "error: exception: ???\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
