# This file was generated by "make depend".
#

$(LIBSQLITEWRAPPER_ROOT)src/db.o: $(LIBSQLITEWRAPPER_ROOT)src/db.cc $(LIBSQLITEWRAPPER_ROOT)../common/include/common/error.h $(LIBSQLITEWRAPPER_ROOT)include/sqlitewrapper.h
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LIBSQLITEWRAPPER_CXXFLAGS) $(LIBSQLITEWRAPPER_CFLAGS) -c -o $@ $<
$(LIBSQLITEWRAPPER_ROOT)src/error.o: $(LIBSQLITEWRAPPER_ROOT)src/error.cc $(LIBSQLITEWRAPPER_ROOT)../common/include/common/error.h $(LIBSQLITEWRAPPER_ROOT)include/sqlitewrapper.h
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LIBSQLITEWRAPPER_CXXFLAGS) $(LIBSQLITEWRAPPER_CFLAGS) -c -o $@ $<
$(LIBSQLITEWRAPPER_ROOT)src/stmt.o: $(LIBSQLITEWRAPPER_ROOT)src/stmt.cc $(LIBSQLITEWRAPPER_ROOT)../common/include/common/error.h $(LIBSQLITEWRAPPER_ROOT)include/sqlitewrapper.h
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(LIBSQLITEWRAPPER_CXXFLAGS) $(LIBSQLITEWRAPPER_CFLAGS) -c -o $@ $<
