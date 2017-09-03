#include <algorithm> // std::find_if
#include <cassert> // assert
#include <cerrno> // errno
#include <cinttypes> // PRI
#include <cstdint>
#include <cstdio> // std::perror, std::fprintf, std::printf
#include <cstdlib>
#include <cstring> // std::strerror
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>
#include <ifaddrs.h> // ::freeifaddrs, ::getifaddrs
#include <linux/if_link.h> // rtnl_link_stats
#include <netdb.h> // ::getnameinfo, NI_MAXHOST, NI_MAXSERV
#include <sys/socket.h> // ::getnameinfo
#include <sys/types.h> // ::freeifaddrs, ::getifaddrs
#include <unistd.h>

namespace net {

    const char*
    family_to_string(int family) {
        switch(family) {
            case AF_PACKET: return "AF_PACKET";
            case AF_INET: return "AF_INET";
            case AF_INET6: return "AF_INET6";
            default: return "?";
        }
    }

    struct Interface {
        std::string name;
        uint32_t flags;
        std::vector<int32_t> families;
        std::vector<std::tuple<std::string, std::string>> addresses;
        rtnl_link_stats stats;
    };

    std::vector<Interface>
    get_interfaces() {
        std::unordered_map<std::string, Interface> map;

        ifaddrs* ifs = nullptr;
        int rv = ::getifaddrs(&ifs);
        if (rv == -1) {
            std::fprintf(stderr, "Error: getifaddrs: %s", std::strerror(errno));
            return {};
        }

        for (ifaddrs* i = ifs; i != nullptr; i = i->ifa_next) {
            if (i->ifa_addr == nullptr) continue;

            const std::string name = i->ifa_name;
            map[name].name = name;
            assert(map[name].flags == 0);
            map[name].flags = i->ifa_flags;
            map[name].families.emplace_back(i->ifa_addr->sa_family);

            // If the address is ipv4/6, then we can get the host and
            // service names (in ip address form).
            char host[NI_MAXHOST] = {};
            char service[NI_MAXSERV] = {};
            if (i->ifa_addr->sa_family == AF_INET || i->ifa_addr->sa_family == AF_INET6) {
                rv = ::getnameinfo(i->ifa_addr, sizeof(sockaddr_storage), host, NI_MAXHOST,
                                   service, NI_MAXSERV, NI_NUMERICHOST);
                if (rv != 0) {
                    std::fprintf(stderr, "Error: getnameinfo: %s\n", ::gai_strerror(rv));
                    ::freeifaddrs(ifs);
                    return {};
                }
            }

            // Statistics (see linux/if_link.h)
            if (i->ifa_data != nullptr) {
                assert(map[name].stats.rx_packets == 0);
                map[name].stats = *static_cast<rtnl_link_stats*>(i->ifa_data);
            }

            map[name].addresses.emplace_back(std::make_tuple(host, service));
        }
        ::freeifaddrs(ifs);

        // Copy map into vector
        std::vector<Interface> vec;
        vec.reserve(map.size());
        std::transform(map.cbegin(), map.cend(), std::back_inserter(vec),
                       [](auto &kv){ return kv.second; });
        return vec;
    }

} // namespace net


int main(int, char**) {
    const std::vector<net::Interface> interfaces = net::get_interfaces();

    std::printf("Number of interfaces: %zu\n", interfaces.size());
    for (const auto& i : interfaces) {
        std::printf("%s\n    flags: 0x%x\n", i.name.c_str(), i.flags);
        std::printf("    families: ");
        for(const auto& f : i.families) {
            std::printf("%s,", net::family_to_string(f));
        }
        std::printf("\n");
        for(const auto& a : i.addresses) {
            const std::string& address = std::get<0>(a);
            const std::string& service = std::get<1>(a);
            if(!address.empty()) {
                std::printf("    address: %s, service=%s\n", address.c_str(), service.c_str());
            }
        }
        std::printf("    rx_packets=%" PRIu32 ", rx_bytes=%" PRIu32 ", rx_errors=%" PRIu32 ", rx_dropped=%" PRIu32 "\n",
                    i.stats.rx_packets, i.stats.rx_bytes, i.stats.rx_errors, i.stats.rx_dropped);
        std::printf("    tx_packets=%" PRIu32 ", tx_bytes=%" PRIu32 ", tx_errors=%" PRIu32 ", tx_dropped=%" PRIu32 "\n",
                    i.stats.tx_packets, i.stats.tx_bytes, i.stats.tx_errors, i.stats.tx_dropped);
        std::printf("    multicast=%" PRIu32 "\n", i.stats.multicast);
    }
    return 0;
}
