# third party libs
# ----------------------------------------------------------------------

CATCH := third_party/catch2/2.12.1/single_include
use-catch =\
  $(eval LOCAL_CPPFLAGS += -isystem $(CATCH))

FMT := third_party/fmt/6.2.0
use-fmt =\
  $(eval LOCAL_CPPFLAGS += -DFMT_HEADER_ONLY -isystem$(FMT)/include)

ifeq ($(COMPILER),gcc)
  GOOGLE_BENCHMARK := third_party/google-benchmark-gcc/1.5.0
else
  GOOGLE_BENCHMARK := third_party/google-benchmark-clang/1.5.0
endif
use-google-benchmark =\
  $(eval LOCAL_CPPFLAGS += -isystem $(GOOGLE_BENCHMARK)/include)\
  $(eval LOCAL_LDFLAGS += -L$(GOOGLE_BENCHMARK)/lib -Wl,-rpath -Wl,$(GOOGLE_BENCHMARK)/lib)\
  $(eval LOCAL_LDLIBS += -lbenchmark)
