#include <cerrno> // for errno
#include <cstdio> // for std::fprintf, std::printf
#include <cstring> // for std::strerror, std::strncpy
#include <sys/socket.h> // for ::accept, ::bind, ::listen, ::socket
#include <sys/un.h> // for sockaddr_un
#include <unistd.h> // for ::read

const char* socket_path = "\0socket";

int main(int, char**)
{
    const int fd = ::socket(AF_UNIX, SOCK_STREAM, /*protocol=*/0);
    if (fd == -1) {
        std::fprintf(stderr, "[error] socket: %s", std::strerror(errno));
        return 1;
    }

    sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    int rv = ::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (rv == -1) {
        std::fprintf(stderr, "[error] bind: %s", std::strerror(errno));
        return 1;
    }

    rv = ::listen(fd, /*backlog=*/0);
    if (rv == -1) {
        std::fprintf(stderr, "[error] listen: %s", std::strerror(errno));
        return 1;
    }

    while (true) {
        sockaddr sa;
        socklen_t len = 0;
        int sockfd = ::accept(fd, &sa, &len);
        if (sockfd == -1) {
            std::fprintf(stderr, "[error] accept: %s", std::strerror(errno));
            continue;
        }

        char buf[1024];
        ssize_t nbytes = 0;
        do {
            nbytes = ::read(sockfd, buf, sizeof(buf));
            std::printf("read %zd bytes: [%.*s]\n", nbytes, static_cast<int>(nbytes), buf);
        } while (nbytes > 0);

        if (nbytes == -1) {
            std::fprintf(stderr, "[error] read: %s", std::strerror(errno));
            return 1;
        }
        if (nbytes == 0) {
            std::fprintf(stderr, "EOF\n");
            ::close(sockfd);
        }
    }

    return 0;
}
