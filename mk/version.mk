GIT := /usr/bin/git
VERSION := $(shell $(GIT) describe --abbrev=4 --always --dirty --tags)
VERSION_FILE := src/version.hpp

all: $(VERSION_FILE)

$(VERSION_FILE): .git/HEAD .git/index
	@echo "#pragma once" > $@
	@echo "constexpr char const* VERSION = \"$(VERSION)\";" >> $@
