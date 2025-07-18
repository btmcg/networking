#include <sys/socket.h> // ::connect, ::socket
#include <sys/un.h>     // sockaddr_un
#include <unistd.h>     // ::read, ::write
#include <cerrno>       // errno
#include <cstdio>       // std::fprintf, std::printf
#include <cstdlib>      // EXIT_FAILURE, EXIT_SUCCESS
#include <cstring>      // std::strerror, std::strncpy
#include <print>


constexpr char const* SocketPath = "\0socket";
constexpr std::size_t BufferSize = 1024;


int
main(int, char**)
{
    try {
        int const fd = ::socket(AF_UNIX, SOCK_STREAM, /*protocol=*/0);
        if (fd == -1) {
            std::println(stderr, "error: socket: {}", std::strerror(errno));
            return 1;
        }

        sockaddr_un addr = {};
        addr.sun_family = AF_UNIX;
        std::strncpy(static_cast<char*>(addr.sun_path), SocketPath, sizeof(addr.sun_path) - 1);

        int rv = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if (rv == -1) {
            std::println(stderr, "error: connect: {}", std::strerror(errno));
            return 1;
        }

        char buf[BufferSize];
        ::ssize_t rbytes = 0;
        do {
            rbytes = ::read(STDIN_FILENO, static_cast<void*>(buf), sizeof(buf));
            ssize_t wbytes
                    = ::write(fd, static_cast<void const*>(buf), static_cast<std::size_t>(rbytes));
            if (wbytes == -1) {
                std::println(stderr, "error: write: {}", std::strerror(errno));
                return 1;
            }

            if (wbytes < rbytes) {
                std::println(stderr, "error: partial write");
            }
        } while (rbytes > 0);
    } catch (std::exception const& e) {
        std::fprintf(stderr, "error: exception: %s\n", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        std::fprintf(stderr, "error: exception: ???\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
