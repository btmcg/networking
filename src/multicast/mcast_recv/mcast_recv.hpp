#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


struct multicast_group
{
    int sock = -1;
    std::string ip = "";
    std::uint16_t port = 0;
};

class mcast_recv final
{
public:
    mcast_recv(std::string const& interface, std::vector<std::string> const& groups);
    int run();

private:
    int subscribe(std::string_view ip, std::uint16_t port);

private:
    static constexpr std::size_t DefaultBufferSize = 4096;
    std::string interface_ip_;
    std::vector<multicast_group> groups_;
};
