MAINTAINERCLEANFILES = \
	$(top_srcdir)/aclocal.m4 \
	$(top_srcdir)/configure \
	$(top_srcdir)/Makefile.in

SUBDIRS = src #test 

noinst_LTLIBRARIES = libxdpd_driver_fscale_l2sw.la

libxdpd_driver_fscale_l2sw_la_SOURCES = 

libxdpd_driver_fscale_l2sw_la_LIBADD = \
		src/libxdpd_driver_fscale_l2sw_src.la

libxdpd_driver_fscale_l2sw_la_LDFLAGS = -L/home/seby/Desktop/l2switch/lib -lvtss_api

maintainer-clean-local:
	rm -rf $(top_srcdir)/m4
	rm -rf $(top_srcdir)/build-aux
