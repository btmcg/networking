# third party libs
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BOOST := /path/to/boost/
use-boost =\
  $(eval LOCAL_CPPFLAGS += -isystem $(BOOST)/include)\
  $(eval LOCAL_LDFLAGS += -L$(BOOST)/lib)\
  $(eval LOCAL_LDLIBS += -lboost_program_options -lboost_system)

LIBCONFIG := /path/to/libconfig
use-libconfig =\
  $(eval LOCAL_CPPFLAGS += -isystem $(LIBCONFIG)/include)\
  $(eval LOCAL_LDFLAGS += -L$(LIBCONFIG)/lib -Wl,-rpath -Wl,$(LIBCONFIG)/lib)\
  $(eval LOCAL_LDLIBS += -lconfig++)
