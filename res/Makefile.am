PKGCONFIG = pkgconf
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

GLGLOBE_RESOURCE_XML=glglobeapp.gresource.xml
GLGLOBE_RESOURCES=$(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies $(GLGLOBE_RESOURCE_XML))

all:../src/resources.c

../src/resources.c: $(GLGLOBE_RESOURCE_XML) $(GLGLOBE_RESOURCES)
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(dir $<) --generate-source $<

# The desktop files
desktopdir = $(datadir)/applications
desktop_in_files=glglobe.desktop.in
desktop_DATA=$(desktop_in_files:.desktop.in=.desktop)

# application icon
appicondir = $(datadir)/icons/hicolor/64x64/apps
appicon_DATA = glglobe.png
appdir_icon=$(appicondir)/$(appicon_DATA)

dist_pkgdata_DATA = 2k_earth_daymap.jpg 2k_earth_nightmap.jpg 2k_earth_normal_map.tif 2k_earth_specular_map.tif 2k_moon.jpg

EXTRA_DIST = $(GLGLOBE_RESOURCES) $(GLGLOBE_RESOURCE_XML) $(desktop_in_files) $(appicon_DATA) $(dist_pkgdata_DATA)

clean-local:
	-rm -rf $(desktop_DATA)

%.desktop: %.desktop.in
	@regex_cmd@ -e "s,\@APP_ICON\@,$(appdir_icon),g" $? > $@
