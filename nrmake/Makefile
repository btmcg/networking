include nrmake/env.mk
include nrmake/functions.mk
include nrmake/pattern_rules.mk
include nrmake/third_party.mk


# initialization
# ----------------------------------------------------------------------

# load modules (any subdirectory that contains a "Module.mk" file)
$(load-modules)


# binary versioning
# ----------------------------------------------------------------------

# to disable any versioning information, comment out this line
include nrmake/version.mk


# rules and dependencies
# ----------------------------------------------------------------------

# generate all necessary rules
$(eval $(call build-rules,$(get-all-modules)))


# recipes
# ----------------------------------------------------------------------

# necessary targets and phony targets
.PHONY: all benchmark clean dist distclean format gen genclean help list-modules tags test tidy
.PHONY: $(get-all-modules)

## all  build all modules in tree
all: $(get-all-modules)

## dist  create tarball for distribution
dist: all | $(INC_DIR)
	$(make-dist)

## gen  build/create all generated files
gen: $(get-all-generated)

## clean  remove all targets and object code
clean:
	$(if $(wildcard $(get-all-targets) $(get-all-objs)),  \
		$(RM) $(strip $(get-all-targets)) $(get-all-objs) \
	)

## genclean  remove all generated files
genclean:
	$(if $(wildcard $(get-all-generated)),  \
		$(RM) $(strip $(get-all-generated)) \
	)

## distclean  remove all build artifacts, completely cleaning tree
distclean: genclean clean
	$(if $(wildcard $(get-all-deps)),  \
		$(RM) $(strip $(get-all-deps)) \
	)
	$(if $(wildcard $(VERSION_FILE)), \
		$(RM) $(VERSION_FILE)         \
	)
	$(if $(wildcard $(BIN_DIR)),                  \
		$(RM) $(BIN_DIR)/* && $(RMDIR) $(BIN_DIR) \
	)
	$(if $(wildcard $(INC_DIR)), \
		$(RM_RF) $(INC_DIR)      \
	)
	$(if $(wildcard $(LIB_DIR)),                  \
		$(RM) $(LIB_DIR)/* && $(RMDIR) $(LIB_DIR) \
	)

## format  run clang-format on all c and cpp files in tree
format:
	@[ ! -d src       ] || find -O3 src       -type f -regex ".*\.[ch]\(pp\)?$$" -exec $(FORMAT) $(FORMATFLAGS) {} \;
	@[ ! -d test      ] || find -O3 test      -type f -regex ".*\.[ch]\(pp\)?$$" -exec $(FORMAT) $(FORMATFLAGS) {} \;
	@[ ! -d benchmark ] || find -O3 benchmark -type f -regex ".*\.[ch]\(pp\)?$$" -exec $(FORMAT) $(FORMATFLAGS) {} \;

## help  show this message and exit
help: $(firstword $(MAKEFILE_LIST))
	$(info nrmake $(shell (cd nrmake && $(GIT_VERSION))))
	@printf "target arguments:\n"
	@sed --quiet --regexp-extended 's/^## ([-_a-zA-Z]+) +(.*)/    \1|\2/p' $< | sort | column --table --separator='|'
	@printf "module arguments:\n"
	@printf "    %s\n" $(get-all-modules) | sort

## tags  generate ctags
tags:
	ctags --recurse src

## benchmark  build benchmark-runner and execute it
benchmark: benchmark-runner
	./bin/$^

## test  build test-runner and execute it
test: test-runner
	./bin/$^

## tidy  run clang-tidy on all c and cpp files in tree
tidy:
	$(run-clang-tidy)

## tidy-fix  run clang-tidy on all c and cpp files in tree (and apply fixes)
tidy-fix:
	$(eval TIDYFLAGS += -fix-errors)
	$(run-clang-tidy)

## list-modules  list all known modules in dependency tree
list-modules:
	$(list-modules)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(INC_DIR):
	$(MKDIR) $(INC_DIR)

$(LIB_DIR):
	$(MKDIR) $(LIB_DIR)
