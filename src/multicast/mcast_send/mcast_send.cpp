#include "mcast_send.hpp"
#include "util/net_util.hpp"
#include <arpa/inet.h>
#include <endian.h>
#include <getopt.h>
#include <net/if.h> // IFNAMSIZ
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h> // ::recvmsg, ::setsockopt, ::socket
#include <sys/types.h>
#include <unistd.h> // ::close
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring> // ::basename, std::strerror
#include <stdexcept>
#include <string>
#include <tuple>


McastSend::McastSend(Config const& cfg)
        : cfg_(cfg)
        , groups_()
        , interface_ip_()
{
    // Need a socket to get interface ip from name
    int const tmp_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (tmp_sock == -1)
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));

    // ioctl (what else?) will translate to ip
    ifreq req = {};
    req.ifr_addr.sa_family = AF_INET;
    std::strncpy(req.ifr_name, cfg_.interface_name.c_str(), IFNAMSIZ - 1);
    int const rv = ::ioctl(tmp_sock, SIOCGIFADDR, &req);
    if (rv == -1)
        throw std::runtime_error(std::string("ioctl(SIOCGIFADDR): ") + std::strerror(errno));

    // Socket no longer needed
    ::close(tmp_sock);

    // Convert/validate all requested groups
    groups_.reserve(cfg_.groups.size());
    for (auto const& g : cfg_.groups) {
        auto [ip, port] = net::parse_ip_port(g);
        if (ip.empty() || port == 0)
            throw std::runtime_error("invalid group: " + g);
        groups_.emplace_back(MulticastGroup{-1, ip, port});
    }

    interface_ip_ = ::inet_ntoa(reinterpret_cast<sockaddr_in*>(&req.ifr_addr)->sin_addr);
    std::printf(
            "sending on interface %s (%s)\n", cfg_.interface_name.c_str(), interface_ip_.c_str());
}

int
McastSend::run()
{
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

    for (auto const& group : groups_) {
        // Set target address
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htobe16(group.port);
        addr.sin_addr.s_addr = ::inet_addr(group.ip.c_str());

        ::ssize_t const nbytes = ::sendto(group.sock, cfg_.text.c_str(), cfg_.text.size(),
                /*flag=*/0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if (nbytes == -1) {
            std::fprintf(stderr, "error: sendto: %s\n", std::strerror(errno));
            return 1;
        }
    }

    return 0;
}
