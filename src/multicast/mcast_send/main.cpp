#include "mcast_send.hpp"
#include <getopt.h>
#include <cstdio>
#include <cstdlib> // std::exit
#include <cstring> // ::basename
#include <string>

namespace { // anonymous

    void
    usage(FILE* f, char const* app)
    {
        std::fprintf(
                f, "usage: %s [-h] [-i <interface>] [-t <text>] <group> [[<group>] ...]\n", app);
        std::fprintf(f,
                "positional arguments:\n"
                "   group               multicast group in the form 'ip:port'\n"
                "optional arguments:\n"
                "   -h, --help          show this help message and exit\n"
                "   -i, --interface     network interface name (e.g. eno1, lo)\n"
                "   -t, --text          text to send\n");
        std::exit(f == stderr ? 1 : 0);
    }

    config
    arg_parse(int argc, char* argv[])
    {
        config cfg;
        char const* app = ::basename(argv[0]);
        if (argc == 1) {
            std::printf("%s: missing required argument(s)\n\n", app);
            usage(stderr, ::basename(app));
        }

        while (true) {
            static option long_options[] = {
                    {"help", no_argument, nullptr, 'h'},
                    {"interface", required_argument, nullptr, 'i'},
                    {"text", required_argument, nullptr, 't'},
                    {nullptr, 0, nullptr, 0},
            };
            int const c = ::getopt_long(argc, argv, "hi:t:", long_options, nullptr);
            if (c == -1)
                break;

            switch (c) {
                case 'h':
                    usage(stdout, ::basename(argv[0]));
                    break;

                case 'i':
                    cfg.interface_name = optarg;
                    break;

                case 't':
                    cfg.text = optarg;
                    break;

                default:
                    usage(stderr, ::basename(argv[0]));
                    break;
            }
        }

        while (optind < argc) {
            cfg.groups.emplace_back(argv[optind]);
            ++optind;
        }

        return cfg;
    }
} // namespace

int
main(int argc, char* argv[])
{
    config const cfg = arg_parse(argc, argv);
    if (cfg.groups.empty()) {
        std::fprintf(stderr, "error: must provide at least one multicast group\n");
        return 1;
    }

    try {
        mcast_send app(cfg);
        return app.run();
    } catch (std::exception const& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }

    return 0;
}
