#include "mcast_send.h"
#include "util/net_util.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib> // for ::exit
#include <cstring> // for ::basename, std::strerror
#include <stdexcept>
#include <string>
#include <tuple>

#include <arpa/inet.h>
#include <endian.h>
#include <getopt.h>
#include <net/if.h> // for IFNAMSIZ
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h> // for ::recvmsg, ::setsockopt, ::socket
#include <sys/types.h>
#include <unistd.h> // for ::close


McastSend::McastSend(const Config& cfg)
        : cfg_(cfg)
        , groups_()
        , interface_ip_() {
    // Need a socket to get interface ip from name
    const int tmp_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (tmp_sock == -1)
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));

    // ioctl (what else?) will translate to ip
    ifreq req = {};
    req.ifr_addr.sa_family = AF_INET;
    std::strncpy(req.ifr_name, cfg_.interface_name.c_str(), IFNAMSIZ - 1);
    const int rv = ::ioctl(tmp_sock, SIOCGIFADDR, &req);
    if (rv == -1)
        throw std::runtime_error(std::string("ioctl(SIOCGIFADDR): ") + std::strerror(errno));

    // Socket no longer needed
    ::close(tmp_sock);

    // Convert/validate all requested groups
    groups_.reserve(cfg_.groups.size());
    for (const auto& g : cfg_.groups) {
        std::string ip; std::uint16_t port;
        std::tie(ip, port) = net::parse_ip_port(g);
        if (ip.empty() || port == 0)
            throw std::runtime_error("invalid group: " + g);
        groups_.emplace_back(MulticastGroup{-1, ip, port});
    }

    interface_ip_ = ::inet_ntoa(reinterpret_cast<sockaddr_in*>(&req.ifr_addr)->sin_addr);
    std::printf("sending on interface %s (%s)\n", cfg_.interface_name.c_str(),
        interface_ip_.c_str());
}

int
McastSend::run() {
    // Prep sockets and assign sending interface
    for (auto& group : groups_) {
        group.sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (group.sock == -1) {
            std::fprintf(stderr, "error: socket: %s\n", std::strerror(errno));
            return 1;
        }

        // Set sending interface
        in_addr iface = {};
        iface.s_addr = ::inet_addr(interface_ip_.c_str());
        int rv = ::setsockopt(group.sock, IPPROTO_IP, IP_MULTICAST_IF, &iface, sizeof(iface));
        if (rv == -1) {
            std::fprintf(stderr, "error: setsockopt(IP_MULTICAST_IF): %s\n", std::strerror(errno));
            return 1;
        }
    }

    for (const auto& group : groups_) {
        // Set target address
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htobe16(group.port);
        addr.sin_addr.s_addr = ::inet_addr(group.ip.c_str());

        const ssize_t nbytes = ::sendto(group.sock, cfg_.text.c_str(), cfg_.text.size(),
            /*flag=*/0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if (nbytes == -1) {
            std::fprintf(stderr, "error: sendto: %s\n", std::strerror(errno));
            return 1;
        }
    }

    return 0;
}
