# module-specific variables
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

modules-LOCALS := \
  LOCAL_CFLAGS \
  LOCAL_CPPFLAGS \
  LOCAL_CXXFLAGS \
  LOCAL_DEPS \
  LOCAL_LDFLAGS \
  LOCAL_LDLIBS \
  LOCAL_LIBRARIES \
  LOCAL_MODULE \
  LOCAL_OBJS \
  LOCAL_PATH \
  LOCAL_SOURCE_FILES \
  LOCAL_TARGET


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : my-dir
# returns  : the directory of the current Makefile
# usage    : $(call my-dir)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
my-dir = $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : empty
# returns  : an empty macro
# usage    : $(empty)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
empty :=


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : space
# returns  : a single space
# usage    : $(space)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
space  := $(empty) $(empty)
space2 := $(space)$(space)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : clear-vars
# returns  : nothing
# usage    : $(call clear-vars)
# rationale: undefines all LOCAL* variables
# note     : undefine was added in GNU Make v3.82
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
clear-vars =\
  $(foreach var,$(modules-LOCALS),\
    $(eval undefine $(var))\
  )


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : rwildcard
# arguments: 1: directory to start search in
#            2: pattern(s) to match
# returns  : all files matching pattern under directory
# usage    : $(call rwildcard,,*.cpp), $(call rwildcard,/tmp/,*.c *.cpp)
# rationale: recursively searchs each directory below $1 for filenames
#            matching pattern in $2
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
rwildcard =\
  $(patsubst $1/%,%,\
    $(foreach directory,$(wildcard $1*),\
      $(call rwildcard,$(directory)/,$2)\
      $(filter $(subst *,%,$2),$(directory))\
    )\
  )


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : convert-c-cpp-suffix-to
# arguments: 1: list of files
#            2: suffix to change all .c and .cpp suffices to
# returns  : files from $1 with suffices changed
# usage    : $(call convert-c-cpp-suffix-to,<file1> <file2> <file3>,o)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
convert-c-cpp-suffix-to =\
  $(patsubst %.c,%.$2,\
    $(patsubst %.cpp,%.$2,$1))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : get-all-deps
# returns  : every module's LOCAL_DEPS
# usage    : $(call get-all-deps)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
get-all-deps =\
  $(foreach module,$(__all_modules),$(__modules.$(module).LOCAL_DEPS))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : get-all-modules
# returns  : every module's name
# usage    : $(call get-all-modules)
# note     : this exists simply to make the top-level Makefile more
#          : consistent in its usage of get-all-xxx functions
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
get-all-modules = $(__all_modules)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : get-all-objs
# returns  : every module's LOCAL_OBJS
# usage    : $(call get-all-objs)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
get-all-objs =\
  $(foreach module,$(__all_modules),$(__modules.$(module).LOCAL_OBJS))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : get-all-targets
# returns  : every module's LOCAL_TARGET
# usage    : $(call get-all-targets)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
get-all-targets =\
  $(foreach module,$(__all_modules),$(__modules.$(module).LOCAL_TARGET))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : load-modules
# returns  : nothing
# usage    : $(call load-modules)
# rationale: Initialization function for this build system. This
#            function simply includes all of the necessary Module.mk
#            files which provide the foundation for all of the pieces of
#            data needed to put together rules and recipes.
# note     : The default goal "all" needs to be declared here (before
#            any other goals) so that it is considered the main goal.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
load-modules =\
  $(eval all:)\
  $(eval include $(shell find . -name "Module.mk"))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : add-executable-module
# returns  : nothing
# usage    : $(call add-executable-module)
# rationale: used in Module.mk files to add an executable target to the
#            build system
# note     : this function requires LOCAL_PATH to be set by caller
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add-executable-module =\
  $(eval LOCAL_MODULE ?= $(notdir $(LOCAL_PATH)))\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_TARGET := $(LOCAL_PATH)/$(LOCAL_MODULE))\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_TYPE   := executable)\
  $(call _add-module,$(LOCAL_MODULE))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : add-shared-library-module
# returns  : nothing
# usage    : $(call add-shared-library-module)
# rationale: used in Module.mk files to add a shared library target to
#            the build system
# note     : this function requires LOCAL_PATH to be set by caller
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add-shared-library-module =\
  $(eval LOCAL_MODULE ?= $(notdir $(LOCAL_PATH)))\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_TARGET := $(LOCAL_PATH)/lib$(LOCAL_MODULE).so)\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_TYPE   := shared_library)\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_CPPFLAGS += -fPIC)\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_LDFLAGS  += -fPIC -shared)\
  $(call _add-module,$(LOCAL_MODULE))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : add-static-library-module
# returns  : nothing
# usage    : $(call add-static-library-module)
# rationale: used in Module.mk files to add a static library target to
#            the build system
# note     : this function requires LOCAL_PATH to be set by caller
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
add-static-library-module =\
  $(eval LOCAL_MODULE ?= $(notdir $(LOCAL_PATH)))\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_TARGET := $(LOCAL_PATH)/lib$(LOCAL_MODULE).a)\
  $(eval __modules.$(LOCAL_MODULE).LOCAL_TYPE   := static_library)\
  $(call _add-module,$(LOCAL_MODULE))


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : _add-module
# arguments: 1: single module name to be added
# returns  : nothing
# usage    : $(call _add-module,<module>)
# rationale: internal function used for common add-module code used by
#            all of the different module types.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
_add-module =\
  $(if $(LOCAL_SOURCE_FILES),\
    $(eval __local_src := $(addprefix $(LOCAL_PATH)/,$(LOCAL_SOURCE_FILES)))\
  ,\
    $(eval __local_src := $(wildcard $(LOCAL_PATH)/*.cpp))\
  )\
  $(eval __modules.$1.LOCAL_CFLAGS       += $(LOCAL_CFLAGS))\
  $(eval __modules.$1.LOCAL_CPPFLAGS     += $(LOCAL_CPPFLAGS))\
  $(eval __modules.$1.LOCAL_CXXFLAGS     += $(LOCAL_CXXFLAGS))\
  $(eval __modules.$1.LOCAL_DEPS         += $(call convert-c-cpp-suffix-to,$(__local_src),d))\
  $(eval __modules.$1.LOCAL_LDFLAGS      += $(LOCAL_LDFLAGS))\
  $(eval __modules.$1.LOCAL_LDLIBS       += $(addprefix -l,$(LOCAL_LIBRARIES)) $(LOCAL_LDLIBS))\
  $(eval __modules.$1.LOCAL_OBJS         += $(call convert-c-cpp-suffix-to,$(__local_src),o))\
  $(eval __modules.$1.LOCAL_PATH         += $(LOCAL_PATH))\
  $(eval __modules.$1.LOCAL_LIBRARIES    += $(LOCAL_LIBRARIES))\
  $(eval __modules.$1.LOCAL_SOURCE_FILES += $(__local_src))\
  $(eval __all_modules += $1)\
  $(call clear-vars)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-module-rules
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-module-rules,<module_name>)
# rationale: generates rules for module
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
build-module-rules =\
  $(eval __modules.$1.LOCAL_LDFLAGS += -L$(LIB_DIR))\
  $(eval $1: $(__modules.$1.LOCAL_TARGET))\
  $(eval $1: $(__modules.$1.LOCAL_PATH)/Module.mk)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-local-target-rules
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-local-target-rules,<module_name>)
# rationale: generates rules for module's LOCAL_TARGET
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
build-local-target-rules =\
  $(eval $(__modules.$1.LOCAL_TARGET): $(__modules.$1.LOCAL_OBJS))\
  $(eval $(__modules.$1.LOCAL_TARGET): $(__modules.$1.LOCAL_PATH)/Module.mk)\
  $(eval $(__modules.$1.LOCAL_TARGET):| $(BIN_DIR) $(LIB_DIR))\
  $(foreach other_module,$(__modules.$1.LOCAL_LIBRARIES),\
    $(eval $(__modules.$1.LOCAL_TARGET): $(__modules.$(other_module).LOCAL_TARGET))\
    $(eval $(__modules.$1.LOCAL_TARGET): $(__modules.$(other_module).LOCAL_PATH)/Module.mk)\
  )

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-internal-dependencies
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-internal-dependencies,<module_name>)
# rationale: internal dependencies are based on the values given in
#            LOCAL_LIBRARIES, so for each module provided, append all
#            flags from that module to this module
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
build-internal-dependencies =\
  $(foreach other_module,$(__modules.$1.LOCAL_LIBRARIES),\
    $(eval __modules.$1.LOCAL_CPPFLAGS += $(__modules.$(other_module).LOCAL_CPPFLAGS))\
    $(eval __modules.$1.LOCAL_CFLAGS += $(__modules.$(other_module).LOCAL_CFLAGS))\
    $(eval __modules.$1.LOCAL_CXXFLAGS += $(__modules.$(other_module).LOCAL_CXXFLAGS))\
    $(eval __modules.$1.LOCAL_LDFLAGS += $(__modules.$(other_module).LOCAL_LDFLAGS))\
    $(eval __modules.$1.LOCAL_LDLIBS += $(__modules.$(other_module).LOCAL_LDLIBS))\
  )

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-object-rules
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-object-rules,<module_name>)
# rationale: generates rules for each object file (*.o) in module's
#            LOCAL_OBJS
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
build-object-rules =\
  $(foreach source,$(__modules.$1.LOCAL_SOURCE_FILES),\
    $(eval $(call convert-c-cpp-suffix-to,$(source),o): $(source) Makefile mk/env.mk mk/functions.mk mk/pattern_rules.mk mk/third_party.mk)\
    $(eval $(call convert-c-cpp-suffix-to,$(source),o): $(__modules.$1.LOCAL_PATH)/Module.mk)\
    $(eval $(call convert-c-cpp-suffix-to,$(source),o): __local_cflags := $$(__modules.$1.LOCAL_CFLAGS))\
    $(eval $(call convert-c-cpp-suffix-to,$(source),o): __local_cppflags := $$(__modules.$1.LOCAL_CPPFLAGS))\
    $(eval $(call convert-c-cpp-suffix-to,$(source),o): __local_cxxflags := $$(__modules.$1.LOCAL_CXXFLAGS))\
  )


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-rules
# arguments: 1: list of module names
# returns  : nothing
# usage    : $(call build-rules,<module_1> <module_2> <module_3> ...)
# rationale: Generates and invokes all of the rules needed for all
#            modules. Once all rules and dependencies have been
#            determined, all that is left to do is kick off the build.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
build-rules =\
  $(foreach module,$1,\
    $(call build-module-rules,$(module))\
    $(call build-local-target-rules,$(module))\
    $(call build-internal-dependencies,$(module))\
    $(call build-object-rules,$(module))\
    $(eval -include $(__modules.$(module).LOCAL_DEPS))\
    \
    $(if $(filter executable,$(__modules.$(module).LOCAL_TYPE)),\
      $(call build-executable,$(module))\
    )\
    $(if $(filter shared_library,$(__modules.$(module).LOCAL_TYPE)),\
      $(call build-shared-library,$(module))\
    )\
    $(if $(filter static_library,$(__modules.$(module).LOCAL_TYPE)),\
      $(call build-static-library,$(module))\
    )\
  )


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-executable
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-executable,<module_name>)
# rationale: generates recipe for module's LOCAL_TARGET
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
define build-executable
$(__modules.$1.LOCAL_TARGET): __build_cmd := $$(call cmd-build,$1)
$(__modules.$1.LOCAL_TARGET):
	$$(__build_cmd)
	$(CP) $(__modules.$1.LOCAL_TARGET) $(BIN_DIR)/$(notdir $(__modules.$1.LOCAL_TARGET))

endef


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-shared-library
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-shared-library,<module_name>)
# rationale: generates recipe for module's LOCAL_TARGET
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
define build-shared-library
$(__modules.$1.LOCAL_TARGET): __build_cmd := $$(call cmd-build,$1)
$(__modules.$1.LOCAL_TARGET):
	$$(__build_cmd)
	$(CP) $(__modules.$1.LOCAL_TARGET) $(LIB_DIR)/$(notdir $(__modules.$1.LOCAL_TARGET))

endef


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : build-static-library
# arguments: 1: module name
# returns  : nothing
# usage    : $(call build-static-library,<module_name>)
# rationale: generates recipe for module's LOCAL_TARGET
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
define build-static-library
$(__modules.$1.LOCAL_TARGET): __build_cmd := $$(call cmd-build-static-lib,$1)
$(__modules.$1.LOCAL_TARGET):
	$$(__build_cmd)
	$(CP) $(__modules.$1.LOCAL_TARGET) $(LIB_DIR)/$(notdir $(__modules.$1.LOCAL_TARGET))

endef

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : cmd-build
# arguments: 1: module name
# returns  : full, executable, compilation/link line for executables and
#            shared libraries
# usage    : $(call cmd-build,<module_name>)
# note     : If there is a file in LOCAL_SOURCE_FILES with an extension
#          : of ".cpp", then $(CXX) will be used to compile/link,
#          : otherwise, $(CC) will be used.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cmd-build =\
  $(strip \
    $(if $(filter %.cpp,$(__modules.$1.LOCAL_SOURCE_FILES)),\
      $(call cmd-build-cpp,$1)\
    ,\
      $(call cmd-build-c,$1)\
    )\
  )


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : cmd-build-c
# arguments: 1: module name
# returns  : full, executable, compilation/link line for executables and
#            shared libraries written in C.
# usage    : $(call cmd-build-c,<module_name>)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cmd-build-c =\
  $(CC)\
  $(__modules.$1.LOCAL_OBJS)\
  $(CPPFLAGS)\
  $(__modules.$1.LOCAL_CPPFLAGS)\
  $(CFLAGS)\
  $(__modules.$1.LOCAL_CFLAGS)\
  $(LDFLAGS)\
  $(__modules.$1.LOCAL_LDFLAGS)\
  $(TARGET_ARCH)\
  $(LDLIBS)\
  $(__modules.$1.LOCAL_LDLIBS)\
  -o $(__modules.$1.LOCAL_TARGET)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : cmd-build-cpp
# arguments: 1: module name
# returns  : full, executable, compilation/link line for executables and
#            shared libraries written in C++
# usage    : $(call cmd-build-cpp,<module_name>)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cmd-build-cpp =\
  $(CXX)\
  $(__modules.$1.LOCAL_OBJS)\
  $(CPPFLAGS)\
  $(__modules.$1.LOCAL_CPPFLAGS)\
  $(CXXFLAGS)\
  $(__modules.$1.LOCAL_CXXFLAGS)\
  $(LDFLAGS)\
  $(__modules.$1.LOCAL_LDFLAGS)\
  $(TARGET_ARCH)\
  $(LDLIBS)\
  $(__modules.$1.LOCAL_LDLIBS)\
  -o $(__modules.$1.LOCAL_TARGET)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : cmd-build-static-lib
# arguments: 1: module name
# returns  : full command to produce a static lib
# usage    : $(call cmd-build-static-lib,<module_name>)
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cmd-build-static-lib =\
  $(AR)\
  $(__modules.$1.LOCAL_TARGET)\
  $(__modules.$1.LOCAL_OBJS)


# debugging functions
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# function : list-modules
# returns  : nothing
# usage    : $(call list-modules)
# rationale: Useful for debugging. Prints all fields of all modules.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
list-modules =\
  $(info modules [$(__all_modules)])\
  $(info targets [$(call get-all-targets)])\
  $(info deps    [$(call get-all-deps)])\
  $(info objs    [$(call get-all-objs)])\
  $(info )\
  $(foreach module,$(__all_modules),\
    $(info $(module))\
    $(info $(space2)LOCAL_CFLAGS           [$(__modules.$(module).LOCAL_CFLAGS)])\
    $(info $(space2)LOCAL_CPPFLAGS         [$(__modules.$(module).LOCAL_CPPFLAGS)])\
    $(info $(space2)LOCAL_CXXFLAGS         [$(__modules.$(module).LOCAL_CXXFLAGS)])\
    $(info $(space2)LOCAL_DEPS             [$(__modules.$(module).LOCAL_DEPS)])\
    $(info $(space2)LOCAL_LDFLAGS          [$(__modules.$(module).LOCAL_LDFLAGS)])\
    $(info $(space2)LOCAL_LDLIBS           [$(__modules.$(module).LOCAL_LDLIBS)])\
    $(info $(space2)LOCAL_OBJS             [$(__modules.$(module).LOCAL_OBJS)])\
    $(info $(space2)LOCAL_PATH             [$(__modules.$(module).LOCAL_PATH)])\
    $(info $(space2)LOCAL_LIBRARIES        [$(__modules.$(module).LOCAL_LIBRARIES)])\
    $(info $(space2)LOCAL_SOURCE_FILES     [$(__modules.$(module).LOCAL_SOURCE_FILES)])\
    $(info $(space2)LOCAL_TARGET           [$(__modules.$(module).LOCAL_TARGET)])\
    $(info $(space2)LOCAL_TYPE             [$(__modules.$(module).LOCAL_TYPE)])\
  )
