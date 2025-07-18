# Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


# clang-specific options
# --------------------------------------------------------------------

# command variables
# ----------------------------------------------------------------------
# Note: In order for -flto to work properly, llvm-ar must be used.
AR  := llvm-ar
CC  := clang
CXX := clang++

# optimization flags
ifndef DEBUG
  OPTFLAGS += -flto=thin

  ifdef PGO_GEN
    OPTFLAGS += -fprofile-instr-generate
  endif
  ifdef PGO_USE
    # Note: Before being able to use the profile data, you must convert
    # the raw data into a useable format:
    #   llvm-profdata merge -output=default.profdata *.profraw
    OPTFLAGS += -fprofile-instr-use -Wno-error=profile-instr-unprofiled -Wno-error=profile-instr-out-of-date
  endif
endif

# warnings
# --------------------------------------------------------------------
# c/c++ warning flags
WARN +=

# c-specific warning flags
CC_WARN +=

# c++-specific warning flags
CXX_WARN +=
CXXFLAGS += -stdlib=libc++

# use the clang linker
LDFLAGS += -rtlib=compiler-rt -fuse-ld=lld
LDLIBS += -lgcc_s
