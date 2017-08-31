# GNU Make built-in targets
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# delete the target if a recipe fails with a non-zero status
.DELETE_ON_ERROR:

# standardize on good ol' Bourne shell
SHELL := /bin/sh


# output directories
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BIN_DIR := bin
LIB_DIR := lib


# command variables
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CC      := gcc
CP      := cp -f
CXX     := g++
DOXYGEN := doxygen
MKDIR   := mkdir -p
MV      := mv -f
RM      := rm -f
RMDIR   := rmdir


# compiler and linker flags
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# gcc warning flags
CPPWFLAGS += \
  -Wall \
  -Wcast-align \
  -Wcast-qual \
  -Wconversion \
  -Wdisabled-optimization \
  -Wempty-body \
  -Werror \
  -Wextra \
  -Wfloat-equal \
  -Wformat=2 \
  -Wmissing-include-dirs \
  -Wshadow \
  -Wsign-conversion \
  -Wswitch-default \
  -Wswitch-enum \
  -Wundef \
  -Wuninitialized
CWFLAGS   +=
CXXWFLAGS += \
  -Wctor-dtor-privacy \
  -Wnon-virtual-dtor \
  -Woverloaded-virtual \
  -Wsuggest-final-methods \
  -Wsuggest-final-types \
  -Wsuggest-override \
  -Wuseless-cast \
  -Wzero-as-null-pointer-constant

# gcc optimization flags
ifdef DEBUG
  OPTFLAGS := -O0 -fno-inline
  CPPWFLAGS += -Wno-error
else
  OPTFLAGS := -O3 -flto -DNDEBUG
endif

# compiler flags
CPPFLAGS += -ggdb3 -fstrict-aliasing -pedantic-errors $(CPPWFLAGS) -MMD -fPIC $(OPTFLAGS) -iquotesrc
CXXFLAGS += -fno-operator-names $(CXXWFLAGS) -std=c++14
CFLAGS   += $(CWFLAGS) -std=c11

# linker flags
LDFLAGS += -rdynamic -Wl,-rpath=$(LIB_DIR),--enable-new-dtags
LDLIBS  += -lrt -lpthread
