## Process this file with automake to produce Makefile.in

SUBDIRS = res src po

dist_doc_DATA = \
	README.md \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS

ACLOCAL_AMFLAGS = --install -I m4

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)
