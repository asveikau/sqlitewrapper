ifndef LIBSQLITEWRAPPER_INC
LIBSQLITEWRAPPER_INC:=y

MAKEFILES_ROOT?=$(LIBSQLITEWRAPPER_ROOT)submodules/makefiles/
LIBCOMMON_ROOT?=$(LIBSQLITEWRAPPER_ROOT)submodules/common/
include $(LIBCOMMON_ROOT)Makefile.inc

LIBSQLITEWRAPPER_CXXFLAGS += -fno-rtti
LIBSQLITEWRAPPER_CXXFLAGS += -I$(LIBCOMMON_ROOT)include
LIBSQLITEWRAPPER_CXXFLAGS += -I$(LIBSQLITEWRAPPER_ROOT)include

ifdef WINDOWS
SQLITE_STATIC=y
endif

ifdef SQLITE_STATIC
SQLITE_DESCR:=sqlite
LIB_DESCRS+=SQLITE_DESCR
LIBSQLITEWRAPPER_SRC += \
   $(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.c
CFLAGS += \
   -I$(LIBSQLITEWRAPPER_ROOT)third_party/sqlite

ifdef WINDOWS
$(LIBSQLITEWRAPPER_ROOT)make_sqlite3_c.cmd:
	(vcvarsall=`sh -c "/usr/bin/find 'C:\Program Files (x86)' -name vcvarsall.bat || true" | sort | tail -1 | tr / '\\' `; \
	echo 'call "'$$vcvarsall'" amd64_x86'; \
	echo 'set CC=cl'; \
	echo 'set CFLAGS='; \
	echo 'set MAKEFLAGS='; \
	echo 'cd third_party/sqlite'; \
	echo 'nmake -f Makefile.msc sqlite3.c') > "$@"

$(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.c: $(LIBSQLITEWRAPPER_ROOT)make_sqlite3_c.cmd
	(cd $(LIBSQLITEWRAPPER_ROOT); cmd //c make_sqlite3_c.cmd)
else
$(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.c:
	(cd $(LIBSQLITEWRAPPER_ROOT)third_party/sqlite; ./configure; cd -)
	$(MAKE) -C $(LIBSQLITEWRAPPER_ROOT)third_party/sqlite sqlite3.c sqlite3.h sqlite3ext.h
endif

$(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.h: $(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.c
$(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3ext.h: $(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.c

SQLITE_STATIC_HEADER := $(LIBSQLITEWRAPPER_ROOT)third_party/sqlite/sqlite3.h
else
LDFLAGS += -lsqlite3
endif

GENERATED_HEADERS += sqlite3.h=SQLITE_STATIC_HEADER

LIBSQLITEWRAPPER_SRC += \
   $(LIBSQLITEWRAPPER_ROOT)src/db.cc \
   $(LIBSQLITEWRAPPER_ROOT)src/stmt.cc \
   $(LIBSQLITEWRAPPER_ROOT)src/error.cc \
   $(LIBSQLITEWRAPPER_ROOT)src/init.cc \

LIBSQLITEWRAPPER_OBJS := $(shell $(SRC2OBJ) $(LIBSQLITEWRAPPER_SRC))

LIBSQLITEWRAPPER:=$(LIBSQLITEWRAPPER_ROOT)$(LIBPREFIX)sqlitewrapper$(LIBSUFFIX)

$(LIBSQLITEWRAPPER): ${LIBSQLITEWRAPPER_OBJS}
	$(LIBWRAPPER) $(LIBSQLITEWRAPPER_OBJS)

-include $(LIBSQLITEWRAPPER_ROOT)depend.mk

endif
