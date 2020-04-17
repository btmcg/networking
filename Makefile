include mk/env.mk
include mk/functions.mk
include mk/pattern_rules.mk
include mk/third_party.mk
include mk/version.mk


# FIXME:
# o Add global variable APP_OUT. If defined, place the bin and lib
#   directories in that directory.
# o Ensure that we have permission to write to APP_OUT, BIN_DIR, LIB_DIR
#   before actually doing any work.
# o Add documentation for all build targets.
# o Add 'doc' default target.
# o Support some sort of code gen, specifically where some other program
#   generates .cpp and .h files.
# o Document build system hierarchy:
#   app -> module -> target -> object -> source
# o Document build system variable naming so there aren't any collisions
#   with user variables.
# o Create module_example.mk, documenting all of the LOCAL_* variables
#   that can be defined. (Perhaps also provide a module_example_minimal.mk
#   which simply defines the bare essentials (LOCAL_PATH, add_module())?
# o Add test examples:
#   - a subdirectory (under src/) that contains several modules
#   - an executable several levels deep (e.g., src/a/b/c/d/e/Module.mk)
# o Add support for make v3.81 and below
# o Add ability to clean just one module (e.g., make module clean)
# o Split compile/link function into two distinct functions
# o Support DESTDIR:
#     (https://www.gnu.org/prep/standards/html_node/DESTDIR.html#DESTDIR)
# o Add better support for clang


# initialization
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# load modules (any subdirectory that contains a "Module.mk" file)
$(call load-modules)


# rules and dependencies
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# generate all necessary rules
$(eval $(call build-rules,$(call get-all-modules)))


# recipes
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# necessary targets and phony targets
.PHONY: all clean distclean list-modules tags $(call get-all-modules)

all: $(call get-all-modules)

clean:
	$(if $(wildcard $(call get-all-targets) $(call get-all-objs) $(call get-all-deps)),\
		$(RM) $(strip $(call get-all-targets)) $(call get-all-objs) $(call get-all-deps))
	$(if $(wildcard $(VERSION_FILE)),$(RM) $(VERSION_FILE))

distclean: clean
	$(if $(wildcard $(LIB_DIR)),\
		$(RM) $(LIB_DIR)/* && $(RMDIR) $(LIB_DIR))
	$(if $(wildcard $(BIN_DIR)),\
		$(RM) $(BIN_DIR)/* && $(RMDIR) $(BIN_DIR))

tags:
	ctags --recurse src

test: test-runner
	./bin/$^

list-modules:
	$(list-modules)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

$(LIB_DIR):
	$(MKDIR) $(LIB_DIR)
