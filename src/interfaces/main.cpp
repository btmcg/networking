#include "util/net_util.hpp"
#include <cstdio> // std::fprintf
#include <print>
#include <vector>


int
main()
{
    try {
        std::vector<net::interface> const interfaces = net::get_interfaces();

        std::println("Number of interfaces: {}", interfaces.size());
        for (auto const& i : interfaces) {
            std::println("{}\n    flags: {:#010x}", i.name, i.flags);
            std::print("    families: ");
            for (auto const& f : i.families) {
                std::print("{},", net::family_to_string(f));
            }
            std::println();
            for (auto const& a : i.addresses) {
                std::string const& address = std::get<0>(a);
                std::string const& service = std::get<1>(a);
                if (!address.empty()) {
                    std::println("    address: {}, service={}", address, service);
                }
            }
            std::println("    rx_packets={}, rx_bytes={}, rx_errors={}, rx_dropped={}",
                    i.stats.rx_packets, i.stats.rx_bytes, i.stats.rx_errors, i.stats.rx_dropped);
            std::println("    tx_packets={}, tx_bytes={}, tx_errors={}, tx_dropped={}",
                    i.stats.tx_packets, i.stats.tx_bytes, i.stats.tx_errors, i.stats.tx_dropped);
            std::println("    multicast={}", i.stats.multicast);
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
