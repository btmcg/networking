#pragma once

#include <gnu/libc-version.h> // ::gnu_get_libc_version
#include <print>
#include <string>


#define PACKED __attribute__((packed))

#if (defined(__GNUC__) && !defined(__clang__))
#    define COMPILER_GCC
#else
#    define COMPILER_CLANG
#endif


inline std::string
get_compiler_version()
{
#if (defined(COMPILER_GCC))
    return std::format("gcc-{}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#elif (defined(COMPILER_CLANG))
    return std::format("clang-{}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#else
    return std::format("unknown_compiler");
#endif
}


inline std::string
get_version_info_multiline()
{
    return std::format("compiler_version={}\n"
                       "glibc_version={}",
            get_compiler_version(), ::gnu_get_libc_version());
}
