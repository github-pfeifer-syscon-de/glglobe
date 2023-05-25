## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)."\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(GENERICGLM_CFLAGS) \
	$(GEODATA_CFLAGS) \
	$(GTKMM_CFLAGS) \
	$(GLIBMM_CFLAGS) \
	$(GTHREAD_CFLAGS) \
	$(LIBSOUP3_CFLAGS) \
	$(JSONGLIB1_CFLAGS) \
	$(EPOXY_CFLAGS)

AM_CFLAGS= \
	-std=c17 -Wall

AM_CXXFLAGS =\
	-Wall -pedantic

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
	TimezoneInfo.cpp \
	TimezoneInfo.hpp

glglobe_LDFLAGS = @EXTRA_LDFLAGS@


glglobe_LDADD = $(GENERICGLM_LIBS) \
	$(GEODATA_LIBS) \
	$(GLIBMM_LIBS) \
	$(GTKMM_LIBS) \
	$(GTHREAD_LIBS) \
	$(LIBSOUP3_LIBS) \
	$(JSONGLIB1_LIBS) \
	$(EPOXY_LIBS)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(pkgdatadir)

