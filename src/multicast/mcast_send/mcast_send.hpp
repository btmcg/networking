#pragma once

#include <cstdint>
#include <string>
#include <vector>


struct config
{
    std::string interface_name;
    std::vector<std::string> groups;
    std::string text;
};

struct multicast_group
{
    int sock = -1;
    std::string ip = "";
    std::uint16_t port = 0;
};

class mcast_send final
{
public:
    explicit mcast_send(config const&);
    int run();

private:
    config const& cfg_;
    std::vector<multicast_group> groups_;
    std::string interface_ip_;
};
