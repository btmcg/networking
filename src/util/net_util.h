#pragma once

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include <linux/if_link.h> // for rtnl_link_stats

namespace net {

    struct Interface {
        std::string name;
        std::uint32_t flags = 0;
        std::vector<std::int32_t> families;
        std::vector<std::tuple<std::string, std::string>> addresses;
        rtnl_link_stats stats;
    };

    std::vector<Interface> get_interfaces();

    const char* family_to_string(int family);

} // namespace net
