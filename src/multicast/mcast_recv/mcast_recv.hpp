#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


struct config
{
    std::string interface_name;
    std::vector<std::string> groups;
};

struct multicast_group
{
    int sock = -1;
    std::string ip = "";
    std::uint16_t port = 0;
};

class mcast_recv final
{
public:
    explicit mcast_recv(config const&);
    int run();

private:
    int subscribe(std::string_view ip, std::uint16_t port);

private:
    config const& cfg_;
    std::string interface_ip_;
    std::vector<multicast_group> groups_;
};
