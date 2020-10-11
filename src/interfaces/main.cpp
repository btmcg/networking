#include "util/net_util.hpp"
#include <fmt/format.h>
#include <cstdio> // std::fprintf
#include <vector>


int
main()
{
    try {
        std::vector<net::interface> const interfaces = net::get_interfaces();

        fmt::print(FMT_STRING("Number of interfaces: {}\n"), interfaces.size());
        for (auto const& i : interfaces) {
            fmt::print(FMT_STRING("{}\n    flags: {:#010x}\n"), i.name, i.flags);
            fmt::print(FMT_STRING("    families: "));
            for (auto const& f : i.families) {
                fmt::print(FMT_STRING("{},"), net::family_to_string(f));
            }
            fmt::print(FMT_STRING("\n"));
            for (auto const& a : i.addresses) {
                std::string const& address = std::get<0>(a);
                std::string const& service = std::get<1>(a);
                if (!address.empty())
                    fmt::print(FMT_STRING("    address: {}, service={}\n"), address, service);
            }
            fmt::print(FMT_STRING("    rx_packets={}, rx_bytes={}, rx_errors={}, rx_dropped={}\n"),
                    i.stats.rx_packets, i.stats.rx_bytes, i.stats.rx_errors, i.stats.rx_dropped);
            fmt::print(FMT_STRING("    tx_packets={}, tx_bytes={}, tx_errors={}, tx_dropped={}\n"),
                    i.stats.tx_packets, i.stats.tx_bytes, i.stats.tx_errors, i.stats.tx_dropped);
            fmt::print(FMT_STRING("    multicast={}\n"), i.stats.multicast);
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
