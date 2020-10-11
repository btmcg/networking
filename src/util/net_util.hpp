#pragma once

#include <linux/if_link.h> // rtnl_link_stats
#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

namespace net {
    struct interface
    {
        std::string name;
        std::uint32_t flags = 0;
        std::vector<std::int32_t> families;
        std::vector<std::tuple<std::string, std::string>> addresses;
        rtnl_link_stats stats{};
    };

    /// \throws std::exception On unexpected error
    std::vector<interface> get_interfaces();

    /// \returns Empty string if unknown family
    char const* family_to_string(int family);

    /// Parses string of form "ip:port"
    /// \returns Tuple of {empty string, 0} if invalid format
    std::tuple<std::string, std::uint16_t> parse_ip_port(std::string const&);

    /// Resolve interface name to ip
    /// \returns Empty string if invalid interface
    /// \throws std::exception On unexpected error
    std::string resolve_interface(std::string_view);

} // namespace net
