#include <fmt/format.h>
#include <sys/socket.h> // ::accept, ::bind, ::listen, ::socket
#include <sys/un.h>     // sockaddr_un
#include <unistd.h>     // ::read
#include <cerrno>       // errno
#include <cstdio>       // std::fprintf, std::printf
#include <cstring>      // std::strerror, std::strncpy


constexpr char const* SocketPath = "\0socket";
constexpr std::size_t BufferSize = 1024;


int
main(int, char**)
{
    try {
        int const fd = ::socket(AF_UNIX, SOCK_STREAM, /*protocol=*/0);
        if (fd == -1) {
            fmt::print(stderr, "error: socket: {}\n", std::strerror(errno));
            return 1;
        }

        sockaddr_un addr = {};
        addr.sun_family = AF_UNIX;
        std::strncpy(static_cast<char*>(addr.sun_path), SocketPath, sizeof(addr.sun_path) - 1);

        int rv = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if (rv == -1) {
            fmt::print(stderr, "error: bind: {}\n", std::strerror(errno));
            return 1;
        }

        rv = ::listen(fd, /*backlog=*/0); // NOLINT
        if (rv == -1) {
            fmt::print(stderr, "error: listen: {}\n", std::strerror(errno));
            return 1;
        }

        while (true) {
            sockaddr sa{};
            socklen_t len = 0;
            int sockfd = ::accept(fd, &sa, &len);
            if (sockfd == -1) {
                fmt::print(stderr, "error: accept: {}\n", std::strerror(errno));
                continue;
            }

            char buf[BufferSize];
            ::ssize_t nbytes = 0;
            do {
                nbytes = ::read(sockfd, static_cast<void*>(buf), sizeof(buf));
                fmt::print("read {} bytes: [{:.{}}]\n", nbytes, buf, nbytes);
            } while (nbytes > 0);

            if (nbytes == -1) {
                fmt::print(stderr, "error: read: {}\n", std::strerror(errno));
                return EXIT_FAILURE;
            }
            if (nbytes == 0) {
                fmt::print(stderr, "EOF\n");
                ::close(sockfd);
            }
        }
    } catch (std::exception const& e) {
        std::fprintf(stderr, "error: exception: %s\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        std::fprintf(stderr, "error: exception: ???\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
