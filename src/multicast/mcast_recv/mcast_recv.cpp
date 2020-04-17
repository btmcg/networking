#include "mcast_recv.hpp"
#include "util/net_util.hpp"
#include <arpa/inet.h>
#include <endian.h>
#include <getopt.h>
#include <net/if.h> // IFNAMSIZ
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h> // ::ioctl
#include <sys/socket.h> // ::recvmsg, ::setsockopt, ::socket
#include <sys/types.h>
#include <unistd.h> // ::close
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring> // ::basename, std::strerror, std::strncpy
#include <stdexcept> // std::runtime_error
#include <string>
#include <string_view>
#include <tuple>


McastRecv::McastRecv(Config const& cfg)
        : cfg_(cfg)
        , interface_ip_()
        , groups_()
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
            "listening on interface %s (%s)\n", cfg_.interface_name.c_str(), interface_ip_.c_str());
}

int
McastRecv::subscribe(std::string_view ip, std::uint16_t port)
{
    int const sock = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        std::fprintf(stderr, "error: socket: %s\n", std::strerror(errno));
        return -1;
    }

    // Allow re-use of port
    int const yes = 1;
    int rv = ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (rv == -1) {
        std::fprintf(stderr, "error: setsockopt(SO_REUSEADDR): %s\n", std::strerror(errno));
        return -1;
    }

    // Bind to filter incoming messages by port
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htobe16(port);
    addr.sin_addr.s_addr = ::inet_addr(std::string(ip).c_str());
    rv = ::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rv == -1) {
        std::fprintf(stderr, "error: bind: %s\n", std::strerror(errno));
        return -1;
    }

    // Subscribe
    ip_mreqn mreq = {};
    mreq.imr_multiaddr.s_addr = ::inet_addr(std::string(ip).c_str());
    mreq.imr_address.s_addr = ::inet_addr(interface_ip_.c_str());
    rv = ::setsockopt(
            sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&mreq), sizeof(mreq));
    if (rv == -1) {
        std::fprintf(stderr, "error: setsockopt(IP_ADD_MEMBERSHIP): %s\n", std::strerror(errno));
        return -1;
    }

    return sock;
}

int
McastRecv::run()
{
    std::vector<pollfd> fds;

    for (auto const& group : groups_) {
        int const sock = subscribe(group.ip, group.port);
        if (sock == -1) {
            std::fprintf(
                    stderr, "error: subscription failure: %s:%hu\n", group.ip.c_str(), group.port);
            return -1;
        }

        pollfd pfd;
        pfd.fd = sock;
        pfd.events = (POLLIN | POLLPRI | POLLERR | POLLHUP | POLLNVAL);
        pfd.revents = 0;
        fds.emplace_back(pfd);
    }

    while (true) {
        int const rv = ::ppoll(fds.data(), fds.size(), nullptr, nullptr);
        if (rv == -1) {
            std::fprintf(stderr, "error: ppoll: %s\n", std::strerror(errno));
            return 1;
        }
        if (rv == 0)
            continue;

        char buf[4096];
        for (pollfd const& fd : fds) {
            if (fd.revents == 0)
                continue;

            ::ssize_t const nbytes
                    = ::recvfrom(fd.fd, &buf, sizeof(buf), /*flags=*/0, nullptr, nullptr);
            if (nbytes == -1) {
                std::fprintf(stderr, "error: recvfrom: %s\n", std::strerror(errno));
                return 1;
            }
            std::printf("received %ld bytes\n", nbytes);
        }
    }

    return 0;
}
