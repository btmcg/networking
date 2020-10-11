#include "util/net_util.hpp"
#include <cinttypes> // PRI identifiers
#include <cstdio>    // std::printf
#include <vector>

int
main(int, char**)
{
    std::vector<net::interface> const interfaces = net::get_interfaces();

    std::printf("Number of interfaces: %zu\n", interfaces.size());
    for (auto const& i : interfaces) {
        std::printf("%s\n    flags: 0x%x\n", i.name.c_str(), i.flags);
        std::printf("    families: ");
        for (auto const& f : i.families) {
            std::printf("%s,", net::family_to_string(f));
        }
        std::printf("\n");
        for (auto const& a : i.addresses) {
            std::string const& address = std::get<0>(a);
            std::string const& service = std::get<1>(a);
            if (!address.empty())
                std::printf("    address: %s, service=%s\n", address.c_str(), service.c_str());
        }
        std::printf("    rx_packets=%" PRIu32 ", rx_bytes=%" PRIu32 ", rx_errors=%" PRIu32
                    ", rx_dropped=%" PRIu32 "\n",
                i.stats.rx_packets, i.stats.rx_bytes, i.stats.rx_errors, i.stats.rx_dropped);
        std::printf("    tx_packets=%" PRIu32 ", tx_bytes=%" PRIu32 ", tx_errors=%" PRIu32
                    ", tx_dropped=%" PRIu32 "\n",
                i.stats.tx_packets, i.stats.tx_bytes, i.stats.tx_errors, i.stats.tx_dropped);
        std::printf("    multicast=%" PRIu32 "\n", i.stats.multicast);
    }

    return 0;
}
