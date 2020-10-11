#pragma once

#include "version.h"
#include "util/compiler.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <getopt.h>
#include <cstdio>  // std::fprintf, std::FILE
#include <cstdlib> // std::exit
#include <string>
#include <vector>


struct cli_args
{
    std::string interface_name;
    std::vector<std::string> groups;
};


inline cli_args
arg_parse(int argc, char** argv)
{
    auto usage = [](std::FILE* outerr, std::filesystem::path const& app) {
        fmt::print(outerr,
                "usage: {} [-hv] [-i <interface>] <group> [[<group>] ...]\n"
                "positional arguments:\n"
                "  group                    multicast group in the for 'ip:port'\n"
                "optional arguments:\n"
                "  -h, --help               this output\n"
                "  -i, --interface=<name>   network interface name (e.g., eno1, lo)\n"
                "  -v, --version            version\n",
                app.c_str());
        std::exit(outerr == stdout ? EXIT_SUCCESS : EXIT_FAILURE);
    };

    auto const app = std::filesystem::path(argv[0]).filename();
    if (argc == 1)
        usage(stderr, app);

    cli_args args;
    while (true) {
        static constexpr option long_options[] = {
                {"help", no_argument, nullptr, 'h'},
                {"interface", no_argument, nullptr, 'i'},
                {"version", no_argument, nullptr, 'v'},
                {nullptr, 0, nullptr, 0},
        };

        int const c = ::getopt_long(
                argc, argv, "hi:v", static_cast<option const*>(long_options), nullptr);
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage(stdout, app);
                break;

            case 'i':
                args.interface_name = optarg;
                break;

            case 'v':
                fmt::print(stdout, "app_version={}\n{}\n", ::VERSION, get_version_info_multiline());
                std::exit(EXIT_SUCCESS);
                break;

            case '?':
            default:
                usage(stderr, app);
                break;
        }
    } // while


    if (optind == argc) {
        fmt::print(stderr, "missing required argument(s)\n\n");
        usage(stderr, app);
    }

    while (optind < argc) {
        args.groups.emplace_back(argv[optind]);
        ++optind;
    }

    return args;
}
