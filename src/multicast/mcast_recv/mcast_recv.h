#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


struct Config {
    std::string interface_name;
    std::vector<std::string> groups;
};

struct MulticastGroup {
    int sock = -1;
    std::string ip = "";
    std::uint16_t port = 0;
};

class McastRecv final {
public:
    explicit McastRecv(const Config&);
    int run();

private:
    int subscribe(std::string_view ip, std::uint16_t port);

private:
    const Config& cfg_;
    std::string interface_ip_;
    std::vector<MulticastGroup> groups_;
};
