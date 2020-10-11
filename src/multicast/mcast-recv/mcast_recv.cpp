#include "mcast_recv.hpp"
#include "util/net_util.hpp"
#include <arpa/inet.h>
#include <endian.h>
#include <fmt/format.h>
#include <getopt.h>
#include <net/if.h> // IFNAMSIZ
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>  // ::ioctl
#include <sys/socket.h> // ::recvmsg, ::setsockopt, ::socket
#include <sys/types.h>
#include <unistd.h> // ::close
#include <cerrno>
#include <cstdint>
#include <cstring>   // ::basename, std::strerror, std::strncpy
#include <stdexcept> // std::runtime_error
#include <string>
#include <string_view>
#include <tuple>


mcast_recv::mcast_recv(std::string const& interface, std::vector<std::string> const& groups)
        : interface_ip_()
        , groups_()
{
    // Need a socket to get interface ip from name
    int const tmp_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (tmp_sock == -1)
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));

    // ioctl will translate to ip
    ifreq req = {};
    req.ifr_addr.sa_family = AF_INET;                            // NOLINT
    std::strncpy(req.ifr_name, interface.c_str(), IFNAMSIZ - 1); // NOLINT
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
    fmt::print("listening on interface {} ({})\n", interface, interface_ip_);
}

int
mcast_recv::subscribe(std::string_view ip, std::uint16_t port)
{
    int const sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        fmt::print(stderr, "error: socket: {}\n", std::strerror(errno));
        return -1;
    }

    // Allow re-use of port
    int const yes = 1;
    int rv = ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (rv == -1) {
        fmt::print(stderr, "error: setsockopt(SO_REUSEADDR): {}\n", std::strerror(errno));
        return -1;
    }

    // Bind to filter incoming messages by port
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(port);
    addr.sin_addr.s_addr = ::inet_addr(std::string(ip).c_str());
    rv = ::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rv == -1) {
        fmt::print(stderr, "error: bind: {}\n", std::strerror(errno));
        return -1;
    }

    // Subscribe
    ip_mreqn mreq = {};
    mreq.imr_multiaddr.s_addr = ::inet_addr(std::string(ip).c_str());
    mreq.imr_address.s_addr = ::inet_addr(interface_ip_.c_str());
    rv = ::setsockopt(
            sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&mreq), sizeof(mreq));
    if (rv == -1) {
        fmt::print(stderr, "error: setsockopt(IP_ADD_MEMBERSHIP): {}\n", std::strerror(errno));
        return -1;
    }

    return sock;
}

int
mcast_recv::run()
{
    std::vector<pollfd> fds;

    for (auto const& group : groups_) {
        int const sock = subscribe(group.ip, group.port);
        if (sock == -1) {
            fmt::print(stderr, "error: subscription failure: {}:{}\n", group.ip, group.port);
            return -1;
        }

        pollfd pfd{};
        pfd.fd = sock;
        pfd.events = (POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL);
        pfd.revents = 0;
        fds.emplace_back(pfd);
    }

    while (true) {
        int const rv = ::ppoll(fds.data(), fds.size(), nullptr, nullptr);
        if (rv == -1) {
            fmt::print(stderr, "error: ppoll: {}\n", std::strerror(errno));
            return 1;
        }
        if (rv == 0)
            continue;

        char buf[DefaultBufferSize];
        for (pollfd const& fd : fds) {
            if (fd.revents == 0)
                continue;

            ::ssize_t const nbytes
                    = ::recvfrom(fd.fd, &buf, sizeof(buf), /*flags=*/0, nullptr, nullptr);
            if (nbytes == -1) {
                fmt::print(stderr, "error: recvfrom: {}\n", std::strerror(errno));
                return 1;
            }
            fmt::print("received {} bytes\n", nbytes);
        }
    }

    return 0;
}
