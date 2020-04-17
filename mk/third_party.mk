# third party libs
# ----------------------------------------------------------------------

CATCH := third_party/catch/2.11.3
use-catch =\
  $(eval LOCAL_CPPFLAGS += -isystem $(CATCH))

ifeq ($(COMPILER),gcc)
  GOOGLE_BENCHMARK := third_party/google-benchmark-gcc/1.5.0
else
  GOOGLE_BENCHMARK := third_party/google-benchmark-clang/1.5.0
endif
use-google-benchmark =\
  $(eval LOCAL_CPPFLAGS += -isystem $(GOOGLE_BENCHMARK)/include)\
  $(eval LOCAL_LDFLAGS += -L$(GOOGLE_BENCHMARK)/lib -Wl,-rpath -Wl,$(GOOGLE_BENCHMARK)/lib)\
  $(eval LOCAL_LDLIBS += -lbenchmark)
