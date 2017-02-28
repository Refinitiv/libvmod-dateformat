AM_CFLAGS = $(VARNISHAPI_CFLAGS) -Wall -Werror
AM_LDFLAGS = $(VARNISHAPI_LIBS) $(VMOD_LDFLAGS)

vmod_LTLIBRARIES = libvmod_dateformat.la

libvmod_dateformat_la_SOURCES = \
	vmod_dateformat.c

nodist_libvmod_dateformat_la_SOURCES = \
	vcc_dateformat_if.c \
	vcc_dateformat_if.h

dist_man_MANS = vmod_dateformat.3

@BUILD_VMOD_DATEFORMAT@

AM_TESTS_ENVIRONMENT = \
	PATH="$(VMOD_TEST_PATH)" \
	LD_LIBRARY_PATH="$(VARNISH_LIBRARY_PATH)"
TEST_EXTENSIONS = .vtc
VTC_LOG_COMPILER = varnishtest -v
AM_VTC_LOG_FLAGS = -Dvmod_dateformat="$(VMOD_DATEFORMAT)"

TESTS = @VMOD_TESTS@

EXTRA_DIST = \
	vmod_dateformat.vcc \
	$(VMOD_TESTS)
