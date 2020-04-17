CC  := gcc
CXX := g++

# warnings
# ----------------------------------------------------------------------
# c/c++ warning flags
WARN :=

# c-specific warning flags
CC_WARN  :=

# c++-specific warning flags
CXX_WARN := \
  -Winline \
  -Wsuggest-override \
  -Wuseless-cast

CPPFLAGS += $(WARN)
CFLAGS   += $(CC_WARN)
CXXFLAGS += $(CXX_WARN)
