## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
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
	TimezoneInfo.cpp \
	TimezoneInfo.hpp

glglobe_LDFLAGS = @EXTRA_LDFLAGS@

glglobe_LDADD = \
	$(GEODATA_LIBS) \
	$(GTHREAD_LIBS) \
	$(EPOXY_LIBS)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(pkgdatadir)

EXTRA_DIST = m4/ChangeLog
