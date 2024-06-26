dnl Process this file with autoconf to produce a configure script.

AC_CONFIG_MACRO_DIR([m4])

AC_INIT([glglobe],[0.1],[glglobe@pfeifer-syscon.de])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AX_CHECK_ENABLE_DEBUG([info], [DEBUG], [RELEASE], [IS-RELEASE])
AC_PROG_CXX
dnl we depend on c++20 just here
dnl (important only if you are compiling for a platform with timezone
dnl names not like e.g. Europe/Paris (e.g. windose)).
AX_CXX_COMPILE_STDCXX([20], [noext], [optional])

LT_INIT

AC_CANONICAL_HOST
AC_MSG_NOTICE([host_os $host_os])

PKG_CHECK_MODULES(GTHREAD,gthread-2.0)

PKG_CHECK_MODULES([GEODATA], [geodata >= 0.1.3])

dnl dont repeat dependencies from geodata
dnl PKG_CHECK_MODULES([GENERICGLM], [genericglm >= 0.3.1])
dnl PKG_CHECK_MODULES([GENERICIMG], [genericimg >= 0.3.1])
dnl PKG_CHECK_MODULES(GLIBMM,glibmm-2.4)
dnl PKG_CHECK_MODULES(GTKMM, [gtkmm-3.0 ])
dnl PKG_CHECK_MODULES(LIBSOUP3, [libsoup-3.0])
dnl PKG_CHECK_MODULES(JSONGLIB1, [json-glib-1.0])

AC_ARG_WITH([gles], AS_HELP_STRING([--with-gles], [use glesv3 (e.g. Raspi) otherweise fullgl, requires same use as with GenericGlm]))

if test "x$with_gles" = "xyes" ; then
    PKG_CHECK_MODULES(EPOXY,[glesv2], [AC_DEFINE([USE_GLES], [1], [Using glesv3 (e.g. Raspi)])])

else
    PKG_CHECK_MODULES(EPOXY,[epoxy])
fi

AX_CHECK_SED()

if test "$host_os" = "mingw32" ; then
    AC_SUBST(EXTRA_LDFLAGS, "-mwindows")
fi

AC_CONFIG_FILES([
Makefile
src/Makefile
res/Makefile
])
AC_OUTPUT
