#include "net_util.h"
#include <algorithm> // for std::transform
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio> // for std::fprintf, std::printf
#include <cstdlib>
#include <cstring> // for std::strerror
#include <set>
#include <unordered_map>

#include <arpa/inet.h>
#include <ifaddrs.h> // for ::freeifaddrs, ::getifaddrs
#include <netdb.h> // for ::getnameinfo, NI_MAXHOST, NI_MAXSERV
#include <sys/socket.h>
#include <sys/socket.h> // for ::getnameinfo
#include <sys/types.h> // for ::freeifaddrs, ::getifaddrs
#include <unistd.h>

namespace net {

    const char*
    family_to_string(int family) {
        switch (family) {
            case AF_PACKET: return "AF_PACKET";
            case AF_INET: return "AF_INET";
            case AF_INET6: return "AF_INET6";
            default: return "";
        }
    }

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


    std::tuple<std::string, std::uint16_t>
    parse_ip_port(const std::string& ip_port) {
        static auto error = std::make_tuple("", 0);

        const std::string::size_type colon = ip_port.find_first_of(':');
        if (colon == std::string::npos)
            return error;

        const std::string ip = ip_port.substr(0, colon);
        const std::string port = ip_port.substr(colon + 1);

        if (ip.empty() || port.empty())
            return error;

        return std::make_tuple(ip, static_cast<std::uint16_t>(std::stoi(port)));
    }

} // namespace net
