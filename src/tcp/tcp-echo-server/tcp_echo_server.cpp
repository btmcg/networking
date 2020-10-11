#include "tcp_echo_server.hpp"
#include <fcntl.h>
#include <fmt/format.h>
#include <netdb.h> // ::getaddrinfo
#include <sys/epoll.h>
#include <sys/socket.h> // socket calls
#include <sys/types.h>  // addrinfo
#include <unistd.h>     // ::close
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>   // std::memset, std::strerror
#include <stdexcept> // std::runtime_error
#include <string>


tcp_echo_server::tcp_echo_server()
        : clients_()
{
    addrinfo hints{};

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // tcp
    hints.ai_flags = AI_PASSIVE;     // wildcard ip
    hints.ai_protocol = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    // Get local address.
    addrinfo* result = nullptr;
    int status = ::getaddrinfo(nullptr, std::to_string(port_).c_str(), &hints, &result);
    if (status != 0) {
        throw std::runtime_error(std::string("getaddrinfo: ") + std::strerror(errno));
    }

    // Get socket
    sockfd_ = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sockfd_ == -1) {
        throw std::runtime_error(std::string("socket: ") + std::strerror(errno));
    }

    // Bind
    status = ::bind(sockfd_, result->ai_addr, result->ai_addrlen);
    if (status == -1) {
        throw std::runtime_error(std::string("bind: ") + std::strerror(errno));
    }

    ::freeaddrinfo(result);

    // Allow for socket reuse
    int const yes = 1;
    status = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (status == -1) {
        throw std::runtime_error(std::string("setsockopt (SO_REUSEADDR): ") + std::strerror(errno));
    }

    status = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));
    if (status == -1) {
        throw std::runtime_error(std::string("setsockopt (SO_REUSEPORT): ") + std::strerror(errno));
    }

    // Set socket as non-blocking
    status = ::fcntl(sockfd_, F_SETFL, O_NONBLOCK);
    if (status == -1) {
        throw std::runtime_error(std::string("fcntl (O_NONBLOCK): ") + std::strerror(errno));
    }

    // Get epoll fd
    epollfd_ = ::epoll_create1(0);
    if (epollfd_ == -1) {
        throw std::runtime_error(std::string("epoll_create1: ") + std::strerror(errno));
    }
}


tcp_echo_server::~tcp_echo_server()
{
    ::close(sockfd_);
    ::close(epollfd_);

    for (auto sock : clients_) {
        ::close(sock);
    }
}


bool
tcp_echo_server::run()
{
    // Start listening
    int result = ::listen(sockfd_, ListenBacklog);
    if (result == -1) {
        fmt::print(stderr, "error: listen: {}\n", std::strerror(errno));
        return false;
    }
    fmt::print("listening on port {}\n", port_);

    // Add our listening socket to epoll.
    epoll_event event{};
    event.data.fd = sockfd_;
    event.events = (EPOLLIN | EPOLLET);
    result = ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, sockfd_, &event);
    if (result == -1) {
        fmt::print(stderr, "error: epoll_ctl: {}\n", std::strerror(errno));
        return false;
    }

    epoll_event events[EpollMaxEvents];
    for (;;) {
        int const num_events = ::epoll_wait(
                epollfd_, static_cast<epoll_event*>(events), EpollMaxEvents, EpollTimeoutMsecs);
        if (num_events == -1) {
            fmt::print(stderr, "error: epoll_wait: {}\n", std::strerror(errno));
            return false;
        }


        for (int i = 0; i < num_events; ++i) {
            // Check for flag that we aren't listening for. Not sure if this is necessary.
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) { // NOLINT
                fmt::print(stderr, "error: unexpected event on fd {}\n", i);

                auto itr = std::find(clients_.begin(), clients_.end(), events[i].data.fd); // NOLINT
                assert(itr != clients_.end());

                clients_.erase(itr);
                ::close(events[i].data.fd); // NOLINT
                continue;
            }

            if (events[i].data.fd == sockfd_) {                                // NOLINT
                bool const status = on_incoming_connection(events[i].data.fd); // NOLINT
                if (!status)
                    return false;
            } else {
                bool const status = on_incoming_data(events[i].data.fd); // NOLINT
                if (!status)
                    return false;
            }
        } // for each event
    }

    return true;
}


bool
tcp_echo_server::on_incoming_connection(int)
{
    // Accept the connection
    sockaddr_storage their_addr{};
    socklen_t addr_size = sizeof(their_addr);
    int const accepted_sock
            = ::accept(sockfd_, reinterpret_cast<sockaddr*>(&their_addr), &addr_size);
    if (accepted_sock == -1) {
        if (errno == EAGAIN)
            return true; // not an error

        throw std::runtime_error(std::string("accept: ") + std::strerror(errno));
    }

    fmt::print(stderr, "error: on_incoming_connection: Client connected on socket fd: {}\n",
            accepted_sock);

    // Successfully connected. Store the new fd.
    clients_.emplace_back(accepted_sock);

    // Add the new fd to epoll.
    epoll_event event{};
    event.events = (EPOLLIN | EPOLLET);
    event.data.fd = accepted_sock;
    int const result = ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, accepted_sock, &event);
    if (result == -1) {
        fmt::print(stderr, "error: epoll_ctl (EPOLL_CTL_ADD): {}\n", std::strerror(errno));
        return false;
    }

    return true;
}


bool
tcp_echo_server::on_incoming_data(int fd)
{
    char buf[IncomingBufferSizeBytes];

    ::ssize_t const bytes_recvd = ::recv(fd, &buf, sizeof(buf), 0);
    if (bytes_recvd == -1) {
        fmt::print(stderr, "error: recv: {}\n", std::strerror(errno));
        return false;
    }

    // Client disconnected
    if (bytes_recvd == 0) {
        fmt::print(stderr, "error: on_incoming_data: client on fd {} disconnected\n", fd);

        auto itr = std::find(clients_.begin(), clients_.end(), fd);
        assert(itr != clients_.end());
        clients_.erase(itr);

        epoll_event event{};
        event.events = (EPOLLIN | EPOLLET);
        event.data.fd = fd;
        int const result = ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event);
        if (result == -1) {
            fmt::print(stderr, "error: epoll_ctl (EPOLL_CTL_DEL): {}\n", std::strerror(errno));
            return false;
        }

        assert(::close(fd) != -1);
        return true;
    }

    // Echo
    ::ssize_t const bytes_sent = ::send(fd, &buf, static_cast<std::size_t>(bytes_recvd), 0);
    if (bytes_sent == -1) {
        fmt::print(stderr, "error: send: {}\n", std::strerror(errno));
        return false;
    }

    buf[bytes_recvd - 1] = '\0'; // NOLINT
    fmt::print("on_incoming_data fd={}, buf={}\n", fd, buf);
    return true;
}
