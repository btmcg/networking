#pragma once

#include <vector>


/*  \class  TcpEchoServer
 *  \brief  FIXME
 */
class TcpEchoServer
{
public:
    TcpEchoServer();
    ~TcpEchoServer();

    // No copies
    TcpEchoServer(TcpEchoServer const&) = delete;
    TcpEchoServer& operator=(TcpEchoServer const&) = delete;

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
        ListenBacklog = 10, ///< max num of pending connections
        EpollMaxEvents = 20, ///< max num of pending epoll events
        EpollTimeoutMsecs = 10, ///< num of milliseconds to block on epoll_wait
    };

private:
    int port_; ///< port to listen on
    int sockfd_; ///< listening socket
    int epollfd_; ///< epoll file descriptor
    std::vector<int> clients_; ///< list of client connected sockets

}; // class TcpEchoServer
