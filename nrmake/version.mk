# Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


GIT := /usr/bin/git
VERSION := $(shell $(GIT_VERSION))
VERSION_FILE := src/version.h

$(VERSION_FILE): .git/HEAD .git/index
	@printf "#pragma once\nchar const* VERSION = \"$(VERSION)\";" > $@

# make sure the file is a dependency of all targets, so it is always
# built and there aren't any include errors
all: $(VERSION_FILE)
tidy: $(VERSION_FILE)
$(foreach module,$(call get-all-modules),                     \
  $(eval $(__modules.$(module).MODULE_DEPS): $(VERSION_FILE)) \
  $(eval $(module): $(VERSION_FILE))                          \
)
