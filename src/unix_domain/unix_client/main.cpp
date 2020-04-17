#include <sys/socket.h> // ::connect, ::socket
#include <sys/un.h> // sockaddr_un
#include <unistd.h> // ::read, ::write
#include <cerrno> // errno
#include <cstdio> // std::fprintf, std::printf
#include <cstring> // std::strerror, std::strncpy

char const* socket_path = "\0socket";

int
main(int, char**)
{
    int const fd = ::socket(AF_UNIX, SOCK_STREAM, /*protocol=*/0);
    if (fd == -1) {
        std::fprintf(stderr, "[error] socket: %s", std::strerror(errno));
        return 1;
    }

    sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    int rv = ::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rv == -1) {
        std::fprintf(stderr, "[error] connect: %s", std::strerror(errno));
        return 1;
    }

    char buf[1024];
    ::ssize_t rbytes = 0;
    do {
        rbytes = ::read(STDIN_FILENO, buf, sizeof(buf));
        ssize_t wbytes = ::write(fd, buf, static_cast<std::size_t>(rbytes));
        if (wbytes == -1) {
            std::fprintf(stderr, "[error] write: %s", std::strerror(errno));
            return 1;
        }

        if (wbytes < rbytes) {
            std::fprintf(stderr, "partial write");
        }
    } while (rbytes > 0);

    return 0;
}
