bin_PROGRAMS = mfdmgr

mfdmgr_SOURCES = mfdmgr.cpp

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include
AM_LDFLAGS = $(BOOST_SYSTEM_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS) $(top_builddir)/src/libmfd.la $(gsoap_LIBS)
