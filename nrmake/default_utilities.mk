# Default utilities and flags
# https://www.gnu.org/prep/standards/html_node/Utilities-in-Makefiles.html#Utilities-in-Makefiles
# ----------------------------------------------------------------------

# CC and CXX should be set in the env_gcc.mk and env_clang.mk files.

AR ?= ar
ARFLAGS ?= rcsv

DOXYGEN ?= doxygen
DOXYGENFLAGS ?=

FORMAT ?= clang-format
FORMATFLAGS ?= -i --verbose

GIT ?= git
GITFLAGS ?=

INSTALL ?= install
INSTALLFLAGS ?=

LD ?= ld
LDFLAGS ?=

TAR ?= tar
TARFLAGS ?= --create --exclude-vcs --exclude=test-runner --exclude=benchmark-runner

TIDY ?= clang-tidy
TIDYFLAGS ?=
