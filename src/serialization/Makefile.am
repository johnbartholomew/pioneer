include $(top_srcdir)/Makefile.common

AM_CFLAGS += $(WARN_CFLAGS)
AM_CPPFLAGS += $(WARN_CPPFLAGS)
AM_CXXFLAGS += $(STD_CXXFLAGS) $(WARN_CXXFLAGS)

AM_CPPFLAGS += -I$(srcdir)/.. -isystem $(top_srcdir)/contrib
AM_CPPFLAGS += -isystem @top_srcdir@/contrib/lua

bin_PROGRAMS = test_cbor_writer

noinst_LIBRARIES = libserialization.a
noinst_HEADERS = \
	GZipFormat.h \
	JsonUtils.h \
	LuaBinSerializer.h \
	PioneerBinSerializer.h \
	cbor.hpp

libserialization_a_SOURCES = \
	GZipFormat.cpp \
	JsonUtils.cpp \
	LuaBinSerializer.cpp \
	PioneerBinSerializer.cpp \
	cbor.cpp

test_cbor_writer_SOURCES = \
	cbor.cpp \
	test_cbor_writer.cpp
