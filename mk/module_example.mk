# Example Module.mk

# The bare-minimum required for a Module.mk file is to define the
# current path (a Make limitation) and to define what kind of output
# should be created. The following two lines are all that is required to
# create a c++ executable.
#
# LOCAL_PATH := = $(call my-dir)
# $(call add-executable-module)


# Acceptable variables:

# LOCAL_PATH
# This always needs to be set in order for the build system to determine
# the current path and location of the module. In almost all cases, it
# should be set by calling the my-dir function, which will do "The Right
# Thing" and set the variable to the full canonical filepath of the
# current Module.mk.
# Example: $(call my-dir)
# Default: empty

# LOCAL_CFLAGS
# Flags to be passed to the C compiler. This should contain any CFLAGS
# necessary for building this particular module. Only necessary for C
# programs.
# Example: -std=c11
# Default: empty

# LOCAL_CPPFLAGS
# Flags to be passed to the C preprocessor and programs that use it.
# This should contains any CPPFLAGS necessary for building this
# particular module. Relevant for C and C++ programs.
# Example: -ggdb3 -MMD -O3
# Defualt: empty

# LOCAL_CXXFLAGS
# Flags to be passed to the C++ compiler. This should contain any
# CXXFLAGS necessary for building this particular module. Only necessary
# for C++ programs.
# Example: -std=c++17
# Default: empty

# LOCAL_DEPS
# This should contain any additional dependencies (internal to the
# module) required to successfully compile the c/c++ source code listed
# in LOCAL_SOURCE_FILES.
# Example: extra_file.h extra_file2.h
# Default: <file>.d dependency files created by using compiler features
# such as -MMD.

# LOCAL_LDFLAGS
# Flags to be passed to the linker. This should contain any LDFLAGS
# necessary for linking this particular module.
# Example: -L/path/to/lib1 -L/path/to/lib2
# Default: empty

# LOCAL_LDLIBS
# Library names and flags to be passed to the linker. This should
# contain any LDLIBS necessary for linking this particular module.
# Example: -pthread -lmylib1 -lmylib2
# Default: empty

# LOCAL_LIBRARIES
# This should contain the basename of libraries within the project that
# this module depends on. For example, if this module relies on the
# module defined in src/mylib/Module.mk, then this variable should
# contain "mylib".
# Example: core mylib1 mylib2
# Default: empty

# LOCAL_MODULE
# This should contain the name of this particular module, if it should
# be different than the basename of the path defined by LOCAL_PATH.
# Example: mymodule
# Default: basename of $(LOCAL_PATH)

# LOCAL_OBJS
# This should contain the names of the object files to be created as a
# result of the compilation process.
# Example: file1.o file2.o
# Default: LOCAL_SOURCE_FILES with extensions replaced by 'o'.

# LOCAL_SOURCE_FILES
# This should contain the name of all source files to be compiled.
# Example: $(call rwildcard,$(LOCAL_PATH),*.cpp) which will populate the
# variable with all cpp files in every subdirectory of the module.
# Default: *.cpp

# LOCAL_TARGET
# This should contain the name of resulting object created by this
# module, whether it be an executable or library.
# Example: my_executable
# Default: $(LOCAL_PATH)/$(LOCAL_MODULE) (for executable)
#          $(LOCAL_PATH)/lib$(LOCAL_MODULE).a (for static lib)
#          $(LOCAL_PATH)/lib$(LOCAL_MODULE).so (for shared lib)
# Default: basename of $(LOCAL_PATH)

# Every Module.mk must conclude with one of the following:
# $(call add-executable-module)
# $(call add-shared-library-module)
# $(call add-static-library-module)
