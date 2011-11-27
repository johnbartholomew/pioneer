include $(top_srcdir)/Makefile.common

AM_CFLAGS += $(WARN_CFLAGS)
AM_CPPFLAGS += $(WARN_CPPFLAGS)
AM_CXXFLAGS += $(WARN_CXXFLAGS)

INCLUDES = -I$(srcdir)/..

noinst_LIBRARIES = libgalaxy.a
noinst_HEADERS = \
	SystemPath.h \
	Galaxy.h \
	Polit.h \
	Sector.h \
	StarSystem.h \
	CustomSystem.h

libgalaxy_a_SOURCES = \
	Galaxy.cpp \
	Polit.cpp \
	Sector.cpp \
	StarSystem.cpp \
	CustomSystem.cpp

INCLUDES += -isystem @top_srcdir@/contrib
if !HAVE_LUA
INCLUDES += -isystem @top_srcdir@/contrib/lua
endif
