#pragma once

#include <vector>


/*  \class  TcpEchoServer
 *  \brief  FIXME
 */
class tcp_echo_server
{
public:
    tcp_echo_server();
    ~tcp_echo_server();

    // No copies/moves
    tcp_echo_server(tcp_echo_server const&) = delete;
    tcp_echo_server(tcp_echo_server&&) = delete;
    tcp_echo_server& operator=(tcp_echo_server const&) = delete;
    tcp_echo_server&& operator=(tcp_echo_server&&) = delete;

    /// Start the server and begin listening on socket.
    /// \return \c false on error
    bool run();

private:
    /// Called on new connection
    /// \return \c false on error
    bool on_incoming_connection(int fd);

    /// Called on incoming data
    /// \return \c false on error
    bool on_incoming_data(int fd);

private:
    enum
    {
        ListenBacklog = 10,             ///< max num of pending connections
        EpollMaxEvents = 20,            ///< max num of pending epoll events
        EpollTimeoutMsecs = 10,         ///< num of milliseconds to block on epoll_wait
        IncomingBufferSizeBytes = 1024, ///< size of recv buffer
        ListenPort = 42483,             ///< default listening port
    };

private:
    int port_{ListenPort};     ///< port to listen on
    int sockfd_{-1};           ///< listening socket
    int epollfd_{-1};          ///< epoll file descriptor
    std::vector<int> clients_; ///< list of client connected sockets

}; // class TcpEchoServer
