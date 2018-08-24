# third party libs
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

BOOST := /path/to/boost
use-boost =\
  $(eval LOCAL_CPPFLAGS += -isystem $(BOOST)/include)\
  $(eval LOCAL_LDFLAGS += -L$(BOOST)/lib)\
  $(eval LOCAL_LDLIBS += -lboost_program_options -lboost_system)

CATCH := /path/to/catch
use-catch =\
  $(eval LOCAL_CPPFLAGS += -isystem $(CATCH))

ifeq ($(COMPILER),gcc)
  GOOGLE_BENCHMARK := /path/to/google-benchmark-gcc
else
  GOOGLE_BENCHMARK := /path/to/google-benchmark-clang
endif
use-google-benchmark =\
  $(eval LOCAL_CPPFLAGS += -isystem $(GOOGLE_BENCHMARK)/include)\
  $(eval LOCAL_LDFLAGS += -L$(GOOGLE_BENCHMARK)/lib -Wl,-rpath -Wl,$(GOOGLE_BENCHMARK)/lib)\
  $(eval LOCAL_LDLIBS += -lbenchmark)
