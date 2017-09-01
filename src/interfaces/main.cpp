#include <cerrno> // cerrno
#include <cstdint>
#include <cstdio> // std::perror, std::fprintf, std::printf
#include <cstdlib>
#include <cstring> // std::strerror
#include <string> // std::string
#include <arpa/inet.h>
#include <ifaddrs.h> // ::freeifaddrs, ::getifaddrs
#include <linux/if_link.h> // rtnl_link_stats
#include <netdb.h> // ::getnameinfo, NI_MAXHOST, NI_MAXSERV
#include <sys/socket.h> // ::getnameinfo
#include <sys/types.h> // ::freeifaddrs, ::getifaddrs
#include <unistd.h>

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
    int flags;
    int family;
    std::string address;
    std::string service;
    uint32_t tx_packets;
    uint32_t rx_packets;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
};


int main(int, char**) {
    ifaddrs* ifaddr;

    int rv = ::getifaddrs(&ifaddr);
    if (rv == -1) {
        std::fprintf(stderr, "Error: getifaddrs: %s", std::strerror(errno));
        return 1;
    }

    for (ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;

        int family = ifa->ifa_addr->sa_family;
        char host[NI_MAXHOST] = {};
        char service[NI_MAXSERV] = {};
        unsigned tx_packets = 0, rx_packets = 0;
        unsigned tx_bytes = 0, rx_bytes = 0;

        if (family == AF_INET || family == AF_INET6) {
            rv = getnameinfo(ifa->ifa_addr, sizeof(sockaddr_storage), host, NI_MAXHOST,
                    service, NI_MAXSERV, NI_NUMERICHOST);
            if (rv != 0) {
                std::fprintf(stderr, "Error: getnameinfo: %s\n", ::gai_strerror(rv));
                return 1;
            }
        } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
            rtnl_link_stats* stats = static_cast<rtnl_link_stats*>(ifa->ifa_data);
            tx_packets = stats->tx_packets;
            rx_packets = stats->rx_packets;
            tx_bytes = stats->tx_bytes;
            rx_bytes = stats->rx_bytes;
        }


        std::printf("name=%s,flags=0x%x,family=%s,address=%s,service=%s,tx_packets=%u,rx_packets=%u,tx_bytes=%u,rx_bytes=%u\n",
                    ifa->ifa_name, ifa->ifa_flags, ::family_to_string(family), host, service,
                    tx_packets, rx_packets, tx_bytes, rx_bytes);
    } // for

    ::freeifaddrs(ifaddr);
    std::exit(EXIT_SUCCESS);
}
