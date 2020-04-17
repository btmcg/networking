#include "mcast_recv.h"
#include <getopt.h>
#include <cstdio>
#include <cstdlib> // for ::exit
#include <cstring> // for ::basename
#include <string>


namespace
{ // anonymous
    void
    usage(FILE* f, const char* app)
    {
        std::fprintf(f, "usage: %s [-h] [-i <interface>] <group> [[<group>] ...]\n", app);
        std::fprintf(f,
                "positional arguments:\n"
                "   group               multicast group in the form 'ip:port'\n"
                "optional arguments:\n"
                "   -h, --help          show this help message and exit\n"
                "   -i, --interface     network interface name (e.g. eno1, lo)\n");
        ::exit(f == stderr ? 1 : 0);
    }

    Config
    arg_parse(int argc, char* argv[])
    {
        Config cfg;
        const char* app = ::basename(argv[0]);
        if (argc == 1) {
            std::printf("%s: missing required argument(s)\n", app);
            usage(stderr, ::basename(app));
        }

        while (true) {
            static option long_options[] = {
                    {"help", no_argument, nullptr, 'h'},
                    {"interface", required_argument, nullptr, 'i'},
                    {nullptr, 0, nullptr, 0},
            };
            const int c = ::getopt_long(argc, argv, "hi:", long_options, nullptr);
            if (c == -1)
                break;

            switch (c) {
                case 'h':
                    usage(stdout, ::basename(argv[0]));
                    break;

                case 'i':
                    cfg.interface_name = optarg;
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
    const Config cfg = arg_parse(argc, argv);
    if (cfg.groups.empty()) {
        std::fprintf(stderr, "error: must provide at least on multicast group\n");
        return 1;
    }

    try {
        McastRecv app(cfg);
        return app.run();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }

    return 0;
}
