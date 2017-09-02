#include <algorithm> // std::find_if
#include <cerrno> // cerrno
#include <cstdint>
#include <cstdio> // std::perror, std::fprintf, std::printf
#include <cstdlib>
#include <cstring> // std::strerror
#include <set>
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

    struct AddrStats {
        std::string address;
        std::string service;
        uint32_t tx_packets;
        uint32_t rx_packets;
        uint32_t tx_bytes;
        uint32_t rx_bytes;
    };

    struct Interface {
        std::string name;
        uint32_t flags;
        std::vector<int32_t> families;
        std::vector<AddrStats> addresses;
    };

    std::vector<Interface>
    get_interfaces() {
        std::unordered_map<std::string, Interface> result;

        ifaddrs* ifs = nullptr;
        int rv = ::getifaddrs(&ifs);
        if (rv == -1) {
            std::fprintf(stderr, "Error: getifaddrs: %s", std::strerror(errno));
            return {};
        }

        for (ifaddrs* i = ifs; i != nullptr; i = i->ifa_next) {
            if (i->ifa_addr == nullptr) continue;

            const std::string name = i->ifa_name;
            result[name].name = name;
            result[name].flags = i->ifa_flags;
            result[name].families.emplace_back(i->ifa_addr->sa_family);

            char host[NI_MAXHOST] = {};
            char service[NI_MAXSERV] = {};
            if (i->ifa_addr->sa_family == AF_INET || i->ifa_addr->sa_family == AF_INET6) {
                rv = getnameinfo(i->ifa_addr, sizeof(sockaddr_storage), host, NI_MAXHOST,
                                 service, NI_MAXSERV, NI_NUMERICHOST);
                if (rv != 0) {
                    std::fprintf(stderr, "Error: getnameinfo: %s\n", ::gai_strerror(rv));
                    ::freeifaddrs(ifs);
                    return {};
                }
            }
            if (host[0] == '\0') continue;
            AddrStats as = {};
            as.address = host;
            as.service = service;

            if (i->ifa_data != nullptr) {
                rtnl_link_stats* stats = static_cast<rtnl_link_stats*>(i->ifa_data);
                as.tx_packets = stats->tx_packets;
                as.rx_packets = stats->rx_packets;
                as.tx_bytes = stats->tx_bytes;
                as.rx_bytes = stats->rx_bytes;
            }
            result[name].addresses.emplace_back(as);
        }
        ::freeifaddrs(ifs);

        std::vector<Interface> vec;
        for (const auto& elem : result) {
            vec.emplace_back(elem.second);
        }
        return vec;
    }

} // namespace net


int main(int, char**) {
    std::vector<net::Interface> interfaces = net::get_interfaces();

    std::printf("Number of interfaces: %zu\n", interfaces.size());
    for (const auto& i : interfaces) {
        std::printf("%s\n    families: ", i.name.c_str());
        for(const auto& f : i.families) {
            std::printf("%s,", net::family_to_string(f));
        }
        std::printf("\n");
        for(const auto& a : i.addresses) {
            std::printf("    address: %s, service=%s\n", a.address.c_str(), a.service.c_str());
        }
    }
    return 0;
}
