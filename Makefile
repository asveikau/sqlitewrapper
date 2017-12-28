all: all-phony

CFLAGS += -O2 -Wall
include Makefile.inc
CXXFLAGS += $(CFLAGS)

all-phony: $(LIBSQLITEWRAPPER)

clean:
	rm -f $(LIBCOMMON) $(LIBCOMMON_OBJS)
	rm -f $(LIBSQLITEWRAPPER) $(LIBSQLITEWRAPPER_OBJS)

export
depend:
	env PROJECT=LIBSQLITEWRAPPER $(DEPEND) src/*.cc > depend.mk

