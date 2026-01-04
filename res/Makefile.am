PKGCONFIG = pkgconf
GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

GLGLOBE_RESOURCE_XML=$(srcdir)/glglobeapp.gresource.xml
GLGLOBE_RESOURCES=$(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir) --generate-dependencies $(GLGLOBE_RESOURCE_XML))
resources_c=$(top_srcdir)/src/resources.c

all:$(resources_c)

$(resources_c): $(GLGLOBE_RESOURCE_XML) $(GLGLOBE_RESOURCES)
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(dir $<) --generate-source $<

# The desktop files
desktopdir = $(datadir)/applications
desktop_DATA=glglobe.desktop

# application icon
icon_basename=glglobe
pixmapdir=$(datadir)/icons/hicolor/64x64/apps
pixmap_DATA=$(icon_basename).png
appicondir=$(datadir)/icons/hicolor/scalable/apps
appicon_DATA=$(icon_basename).svg

dist_pkgdata_DATA = 2k_earth_daymap.jpg 2k_earth_nightmap.jpg 2k_earth_normal_map.tif 2k_earth_specular_map.tif 2k_moon.jpg

EXTRA_DIST = $(GLGLOBE_RESOURCES) $(GLGLOBE_RESOURCE_XML) $(desktop_DATA) $(appicon_DATA) $(pixmap_DATA) $(dist_pkgdata_DATA)

clean-local:
	-rm -rf $(resources_c)

2k_moon.jpg:
	curl https://www.solarsystemscope.com/textures/download/$@ -o $@

2k_earth_specular_map.tif:
	curl https://www.solarsystemscope.com/textures/download/$@ -o $@

2k_earth_normal_map.tif:
	curl https://www.solarsystemscope.com/textures/download/$@ -o $@

2k_earth_nightmap.jpg:
	curl https://www.solarsystemscope.com/textures/download/$@ -o $@

2k_earth_daymap.jpg:
	curl https://www.solarsystemscope.com/textures/download/$@ -o $@
