lib_LTLIBRARIES = libmfd.la

libmfd_la_SOURCES = main.cpp
libmfd_la_SOURCES += device-ricoh-aficio.cpp
libmfd_la_SOURCES += exceptions.cpp

EXTRA_libmfd_la_SOURCES = main.hpp
EXTRA_libmfd_la_SOURCES += device-ricoh-aficio.hpp
EXTRA_libmfd_la_SOURCES += ricoh-udirectory.wsdl

# File copied from gSOAP source as we need to change build flags
libmfd_la_SOURCES += stdsoap2.cpp
# Sources generated by gSOAP but distributed with the code to avoid a dependency
libmfd_la_SOURCES += soapC.cpp
libmfd_la_SOURCES += soapuDirectoryProxy.cpp
EXTRA_libmfd_la_SOURCES += ricoh-udirectory.h
EXTRA_libmfd_la_SOURCES += typemap.dat
EXTRA_libmfd_la_SOURCES += uDirectory.nsmap
EXTRA_libmfd_la_SOURCES += soapH.h
EXTRA_libmfd_la_SOURCES += soapStub.h
EXTRA_libmfd_la_SOURCES += soapuDirectoryProxy.h

AM_CPPFLAGS = $(BOOST_CPPFLAGS) -I $(top_srcdir)/include
# gSOAP build flag to prevent SOAP action from being included in the HTTP
# Content-Type header, needed for Ricoh machines.
AM_CPPFLAGS += -DWITHOUT_CONTENT_TYPE_ACTION

libmfd_la_LDFLAGS = $(AM_LDFLAGS) -release @VERSION@ -version-info 0
libmfd_la_LIBADD = $(BOOST_SYSTEM_LIBS) $(BOOST_FILESYSTEM_LIBS)
