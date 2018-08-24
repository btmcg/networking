#pragma once

#include <cstdint>
#include <string>
#include <string_view>
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

    /// \throws std::exception On unexpected error
    std::vector<Interface> get_interfaces();

    const char* family_to_string(int family);

    // Parses string of form "ip:port"
    std::tuple<std::string, std::uint16_t> parse_ip_port(const std::string&);

    /// Resolve interface name to ip
    /// \returns Empty string if invalid interface
    /// \throws std::exception On unexpected error
    std::string resolve_interface(std::string_view);

} // namespace net
