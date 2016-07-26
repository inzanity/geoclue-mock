libexec_PROGRAMS = geoclue-mock

geoclue_mock_CFLAGS = 	\
	-I$(top_srcdir)		\
	-I$(top_builddir)	\
	$(GLIB_CFLAGS)	\
	$(GEOCLUE_CFLAGS)

geoclue_mock_LDADD =		\
	$(GLIB_LIBS)		\
	$(GEOCLUE_LIBS)	

geoclue_mock_SOURCES =	\
	geoclue-mock.c

providersdir = $(datadir)/geoclue-providers
providers_DATA = geoclue-mock.provider

servicedir = $(DBUS_SERVICES_DIR)
service_in_files = org.freedesktop.Geoclue.Providers.Mock.service.in
service_DATA = $(service_in_files:.service.in=.service)

$(service_DATA): $(service_in_files) Makefile
	$(AM_V_GEN) sed -e "s|\@libexecdir\@|$(libexecdir)|" $< > $@

EXTRA_DIST = 			\
	$(service_in_files)	\
	$(providers_DATA)

DISTCLEANFILES = \
	$(service_DATA)

gsettings_SCHEMAS = fi.inz.GeoclueMock.gschema.xml
@GSETTINGS_RULES@
