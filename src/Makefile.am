## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICGLM_CFLAGS) \
	$(GENERICIMG_CFLAGS) \
	$(GEODATA_CFLAGS) \
	$(GTHREAD_CFLAGS) \
	$(EPOXY_CFLAGS)

AM_CFLAGS= \
	-Wall

## if you really  want to be pedantic use -Wconversion,
## but i did not included casts when feasible
AM_CXXFLAGS =\
	-Wall \
	-Wextra \
	-Wno-unused-parameter \
	-pedantic

bin_PROGRAMS = glglobe

glglobe_SOURCES = \
	resources.c \
	Config.cpp \
	Config.hpp \
	GlGlobeApp.cpp \
	GlGlobeApp.hpp \
	GlGlobeWindow.cpp \
	GlGlobeWindow.hpp \
	GlSphereView.cpp \
	GlSphereView.hpp \
	MoonContext.cpp \
	MoonContext.hpp \
	SphereGlArea.cpp \
	SphereGlArea.hpp \
	SphereContext.cpp \
	SphereContext.hpp \
	SunSet.cpp \
	SunSet.hpp \
	ConfigDialog.cpp \
	ConfigDialog.hpp \
	BoundsDisplay.cpp \
	BoundsDisplay.hpp \
	GeoJsonGeometryHandler.cpp \
	GeoJsonGeometryHandler.hpp \
	WeatherDialog.cpp \
	WeatherDialog.hpp \
	Timer.cpp \
	Timer.hpp \
	PlotDialog.cpp \
	PlotDialog.hpp \
	Moon.cpp \
	Moon.hpp \
	TimezoneInfo.cpp \
	TimezoneInfo.hpp

glglobe_LDFLAGS = @EXTRA_LDFLAGS@

# the generics are not be needed for linux, but on win&msys2 this will get link errors otherwise
glglobe_LDADD = \
	$(GENERICGLM_LIBS) \
	$(GENERICIMG_LIBS) \
	$(GEODATA_LIBS) \
	$(GTHREAD_LIBS) \
	$(EPOXY_LIBS)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(pkgdatadir)

EXTRA_DIST = m4/ChangeLog
