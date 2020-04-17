#pragma once

#include <cstdint>
#include <string>
#include <vector>


struct Config
{
    std::string interface_name;
    std::vector<std::string> groups;
    std::string text;
};

struct MulticastGroup
{
    int sock = -1;
    std::string ip = "";
    std::uint16_t port = 0;
};

class McastSend final
{
public:
    explicit McastSend(Config const&);
    int run();

private:
    Config const& cfg_;
    std::vector<MulticastGroup> groups_;
    std::string interface_ip_;
};
