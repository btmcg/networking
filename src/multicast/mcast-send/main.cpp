#include "arg_parse.hpp"
#include "mcast_send.hpp"
#include <cstdio> // std::fprintf


int
main(int argc, char* argv[])
{
    try {
        cli_args const args = arg_parse(argc, argv);
        if (args.groups.empty()) {
            std::fprintf(stderr, "error: must provide at least one multicast group\n");
            return EXIT_FAILURE;
        }

        mcast_send app(args.interface_name, args.groups, args.text);
        return app.run();
    } catch (std::exception const& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        std::fprintf(stderr, "error: exception: ???\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
