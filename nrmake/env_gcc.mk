# Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


# gcc-specific options
# ----------------------------------------------------------------------

# command variables
# ----------------------------------------------------------------------
# Note: In order for -flto to work properly, gcc-ar must be used.
AR  := gcc-ar
CC  := gcc
CXX := g++

# optimization flags
ifndef DEBUG
  OPTFLAGS += -flto=auto -ffat-lto-objects -fuse-linker-plugin

  ifdef PGO_GEN
    OPTFLAGS += -fprofile-generate
  endif
  ifdef PGO_USE
    OPTFLAGS += -fprofile-use -Wno-error=missing-profile
  endif
endif

# warnings
# ----------------------------------------------------------------------
# c/c++ warning flags
WARN +=

# c-specific warning flags
CC_WARN +=

# c++-specific warning flags
CXX_WARN +=               \
  -Wsuggest-override      \
  -Wuseless-cast

# use the gold linker
CPPFLAGS += -fuse-ld=gold
