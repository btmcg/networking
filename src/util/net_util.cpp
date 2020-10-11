#include "net_util.hpp"
#include <arpa/inet.h>
#include <ifaddrs.h> // ::freeifaddrs, ::getifaddrs
#include <netdb.h>   // ::getnameinfo, NI_MAXHOST, NI_MAXSERV
#include <sys/socket.h>
#include <sys/socket.h> // ::getnameinfo
#include <sys/types.h>  // ::freeifaddrs, ::getifaddrs
#include <unistd.h>
#include <algorithm> // std::transform
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio> // std::fprintf, std::printf
#include <cstdlib>
#include <cstring> // std::strerror, std::strlen
#include <set>
#include <stdexcept>
#include <unordered_map>

namespace net {
    char const*
    family_to_string(int family)
    {
        switch (family) {
            case AF_PACKET:
                return "AF_PACKET";
            case AF_INET:
                return "AF_INET";
            case AF_INET6:
                return "AF_INET6";
            default:
                return "";
        }
    }

    std::vector<interface>
    get_interfaces()
    {
        std::unordered_map<std::string, interface> map;

        ifaddrs* ifs = nullptr;
        int rv = ::getifaddrs(&ifs);
        if (rv == -1)
            throw std::runtime_error(std::string("error: getifaddrs: ") + std::strerror(errno));

        for (ifaddrs* i = ifs; i != nullptr; i = i->ifa_next) {
            if (i->ifa_addr == nullptr)
                continue;

            std::string const name = i->ifa_name;
            map[name].name = name;
            assert(map[name].flags == 0);
            map[name].flags = i->ifa_flags;
            map[name].families.emplace_back(i->ifa_addr->sa_family);

            // If the address is ipv4/6, then we can get the host and
            // service names (in ip address form).
            char host[NI_MAXHOST] = {};
            char service[NI_MAXSERV] = {};
            if (i->ifa_addr->sa_family == AF_INET || i->ifa_addr->sa_family == AF_INET6) {
                rv = ::getnameinfo(i->ifa_addr, sizeof(sockaddr_storage), static_cast<char*>(host),
                        sizeof(host), static_cast<char*>(service), sizeof(service), NI_NUMERICHOST);
                if (rv != 0) {
                    ::freeifaddrs(ifs);
                    throw std::runtime_error(
                            std::string("error: getnameinfo: ") + ::gai_strerror(rv));
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
        std::vector<interface> vec;
        vec.reserve(map.size());
        std::transform(map.cbegin(), map.cend(), std::back_inserter(vec),
                [](auto& kv) { return kv.second; });
        return vec;
    }


    std::tuple<std::string, std::uint16_t>
    parse_ip_port(std::string const& ip_port)
    {
        static auto error = std::make_tuple("", 0);

        std::string::size_type const colon = ip_port.find_first_of(':');
        if (colon == std::string::npos)
            return error;

        std::string const ip = ip_port.substr(0, colon);
        std::string const port = ip_port.substr(colon + 1);

        if (ip.empty() || port.empty())
            return error;

        return std::make_tuple(ip, static_cast<std::uint16_t>(std::stoi(port)));
    }

    std::string
    resolve_interface(std::string_view name)
    {
        static std::string error;

        if (name.empty())
            return error;

        ifaddrs* ifs = nullptr;
        int rv = ::getifaddrs(&ifs);
        if (rv == -1)
            throw std::runtime_error(std::string("error: getifaddrs: ") + std::strerror(errno));

        std::string address;
        for (ifaddrs* i = ifs; i != nullptr; i = i->ifa_next) {
            if (i->ifa_addr == nullptr)
                continue;

            if (name != i->ifa_name)
                continue;

            // If the address is ipv4/6, then we can get the host and
            // service names (in ip address form).
            char host[NI_MAXHOST] = {};
            char service[NI_MAXSERV] = {};
            if (i->ifa_addr->sa_family == AF_INET || i->ifa_addr->sa_family == AF_INET6) {
                rv = ::getnameinfo(i->ifa_addr, sizeof(sockaddr_storage), static_cast<char*>(host),
                        sizeof(host), static_cast<char*>(service), sizeof(service), NI_NUMERICHOST);
                if (rv != 0) {
                    ::freeifaddrs(ifs);
                    throw std::runtime_error(
                            std::string("error: getnameinfo: ") + ::gai_strerror(rv));
                }
            }

            if (std::strlen(static_cast<char const*>(host)) == 0)
                continue;

            address.assign(static_cast<char const*>(host));
            break;
        }

        ::freeifaddrs(ifs);
        return address;
    }

} // namespace net
