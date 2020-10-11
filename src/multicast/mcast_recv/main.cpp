#include "arg_parse.hpp"
#include "mcast_recv.hpp"
#include <cstdio> // std::fprintf
#include <exception>


int
main(int argc, char* argv[])
{
    try {
        cli_args const args = arg_parse(argc, argv);
        if (args.groups.empty()) {
            std::fprintf(stderr, "error: must provide at least one multicast group\n");
            return EXIT_FAILURE;
        }

        mcast_recv app(args.interface_name, args.groups);
        return app.run();
    } catch (std::exception const& e) {
        std::fprintf(stderr, "error: exception: %s\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        std::fprintf(stderr, "error: exception: ???\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
