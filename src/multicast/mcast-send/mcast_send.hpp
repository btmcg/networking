#pragma once

#include <cstdint>
#include <string>
#include <vector>


struct multicast_group
{
    int sock = -1;
    std::string ip = "";
    std::uint16_t port = 0;
};

class mcast_send final
{
public:
    mcast_send(std::string const& interface_name, std::vector<std::string> const& groups,
            std::string text);
    int run();

private:
    std::vector<multicast_group> groups_;
    std::string interface_ip_;
    std::string const text_;
};
