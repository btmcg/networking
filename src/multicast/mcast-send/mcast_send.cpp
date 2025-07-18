#include "mcast_send.hpp"
#include "util/net_util.hpp"
#include <arpa/inet.h>
#include <endian.h>
#include <net/if.h> // IFNAMSIZ
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h> // ::recvmsg, ::setsockopt, ::socket
#include <sys/types.h>
#include <unistd.h> // ::close
#include <cerrno>
#include <cstring> // std::strerror
#include <print>
#include <stdexcept>
#include <tuple>


mcast_send::mcast_send(
        std::string const& interface_name, std::vector<std::string> const& groups, std::string text)
        : groups_()
        , interface_ip_()
        , text_(std::move(text))
{
    // Need a socket to get interface ip from name
    int const tmp_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (tmp_sock == -1)
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));

    // ioctl (what else?) will translate to ip
    ifreq req = {};
    req.ifr_addr.sa_family = AF_INET;                                 // NOLINT
    std::strncpy(req.ifr_name, interface_name.c_str(), IFNAMSIZ - 1); // NOLINT
    int const rv = ::ioctl(tmp_sock, SIOCGIFADDR, &req);
    if (rv == -1)
        throw std::runtime_error(std::string("ioctl(SIOCGIFADDR): ") + std::strerror(errno));

    // Socket no longer needed
    ::close(tmp_sock);

    // Convert/validate all requested groups
    groups_.reserve(groups.size());
    for (auto const& g : groups) {
        auto [ip, port] = net::parse_ip_port(g);
        if (ip.empty() || port == 0)
            throw std::runtime_error("invalid group: " + g);
        groups_.emplace_back(multicast_group{-1, ip, port});
    }

    interface_ip_ = ::inet_ntoa(reinterpret_cast<sockaddr_in*>(&req.ifr_addr)->sin_addr); // NOLINT
    std::println("sending on interface {} ({})", interface_name, interface_ip_);
}

int
mcast_send::run()
{
    // Prep sockets and assign sending interface
    for (auto& group : groups_) {
        group.sock = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (group.sock == -1) {
            std::println(stderr, "error: socket: {}", std::strerror(errno));
            return 1;
        }

        // Set sending interface
        in_addr iface = {};
        iface.s_addr = ::inet_addr(interface_ip_.c_str());
        int rv = ::setsockopt(group.sock, IPPROTO_IP, IP_MULTICAST_IF, &iface, sizeof(iface));
        if (rv == -1) {
            std::println(stderr, "error: setsockopt(IP_MULTICAST_IF): {}", std::strerror(errno));
            return 1;
        }
    }

    for (auto const& group : groups_) {
        // Set target address
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htobe16(group.port);
        addr.sin_addr.s_addr = ::inet_addr(group.ip.c_str());

        ::ssize_t const nbytes = ::sendto(group.sock, text_.c_str(), text_.size(),
                /*flag=*/0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)); // NOLINT
        if (nbytes == -1) {
            std::println(stderr, "error: sendto: {}", std::strerror(errno));
            return 1;
        }
    }

    return 0;
}
