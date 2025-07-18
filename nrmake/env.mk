# Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


# GNU Make settings
# ----------------------------------------------------------------------
# eliminate all built-in implicit rules and rule-specific variables
MAKEFLAGS += --no-builtin-rules --no-builtin-variables

# delete the target if a recipe fails with a non-zero status
.DELETE_ON_ERROR:

# rely on standardized POSIX shell
SHELL := /bin/sh

# define all utilities used
include nrmake/default_utilities.mk


# distribution
# ----------------------------------------------------------------------
# root-directory of project; considered the project name
ROOT_DIR := $(realpath $(dir $(dir $(realpath $(firstword $(MAKEFILE_LIST))))../))
PROJECT  := $(notdir $(ROOT_DIR))

BIN_DIR := bin
INC_DIR := include
LIB_DIR := lib
PACKAGE_NAME = $(PROJECT)-$(VERSION)
PACKAGE_EXT := .tar.gz


# settings due to cmd-line-specified targets
# ----------------------------------------------------------------------

# clang-tidy won't work with another compiler's flags/options, so set
# COMPILER to clang if user provided 'tidy' as the target
ifeq (tidy,$(findstring tidy,$(MAKECMDGOALS)))
  COMPILER := clang
endif


# command variables
# ----------------------------------------------------------------------
CP          := cp --force
GIT_VERSION := $(GIT) describe --abbrev=4 --always --dirty --tags
MKDIR       := mkdir --parents
MV          := mv --force
RM          := rm --force
RMDIR       := rmdir
RM_RF       := rm --force --recursive


# compiler and linker flags
# ----------------------------------------------------------------------
TARGET_ARCH := -march=native

# c/c++ warning flags
WARN :=                        \
  -pedantic-errors             \
  -Wall                        \
  -Wcast-align                 \
  -Wcast-qual                  \
  -Wdisabled-optimization      \
  -Wempty-body                 \
  -Werror                      \
  -Wextra                      \
  -Wfloat-equal                \
  -Wformat=2                   \
  -Wmissing-include-dirs       \
  -Wno-deprecated-declarations \
  -Wswitch-default             \
  -Wswitch-enum                \
  -Wundef                      \
  -Wuninitialized

# c-specific warning flags
CC_WARN :=

# c++-specific warning flags
CXX_WARN :=                       \
  -fno-operator-names             \
  -Wctor-dtor-privacy             \
  -Wnon-virtual-dtor              \
  -Woverloaded-virtual            \
  -Wzero-as-null-pointer-constant

# sanitizer options
ifdef ASAN
  # address sanitizer
  # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
  # https://clang.llvm.org/docs/AddressSanitizer.html
  # https://github.com/google/sanitizers/wiki/AddressSanitizer

  # ASAN_OPTIONS=check_initialization_order=1 bin/test-runner
  CPPFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif

ifdef MSAN
  # memory sanitizer (only supported by clang)
  # https://clang.llvm.org/docs/MemorySanitizer.html

  COMPILER := clang
  CPPFLAGS += -fsanitize=memory -fPIE -pie -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -Wno-unused-command-line-argument
endif

ifdef UBSAN
  # undefined behavior sanitizer
  # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
  # https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html

  # UBSAN_OPTIONS=print_stacktrace=1 bin/test-runner
  CPPFLAGS += -fsanitize=undefined -fno-omit-frame-pointer
endif

# optimization flags
ifdef DEBUG
  OPTFLAGS := -O0 -fno-inline
  WARN += -Wno-error
else
  OPTFLAGS += -O3 -DNDEBUG
endif


# compiler flags
CPPFLAGS += -ggdb3 -fstrict-aliasing $(WARN) $(OPTFLAGS) -iquote src
CXXFLAGS += -std=c++26 $(CXX_WARN)
CFLAGS   += -std=c11 $(CC_WARN)

# linker flags
LDFLAGS += -Wl,-rpath='$$ORIGIN/../lib',--enable-new-dtags
LDLIBS  += -ldl -lrt -pthread


# compiler-specific additions/changes
# ----------------------------------------------------------------------
# If the user chooses their own compiler (CC/CXX) via the command line
# or environment variables, then they're responsible for fine-tuning any
# extra flags they may want. Otherwise, use the compiler configuration
# provided in the env_$(COMPILER).mk files.
ifeq (,$(findstring environment,$(origin CC)$(origin CXX)))

  # either 'gcc' or 'clang' (default to gcc)
  COMPILER ?= gcc
  ifeq (,$(findstring $(COMPILER),gcc clang))
    $(error "Invalid value COMPILER=$(COMPILER), must be either 'gcc' or 'clang'")
  endif

  include nrmake/env_$(COMPILER).mk

else
  # The user has provided their own CC or CXX, notify them.
  ifeq (environment, $(origin CC))
    $(info CC=$(CC) (provided by user))
  else
    CC=cc
  endif
  ifeq (environment, $(origin CXX))
    $(info CXX=$(CXX) (provided by user))
  else
    CXX=c++
  endif
endif
