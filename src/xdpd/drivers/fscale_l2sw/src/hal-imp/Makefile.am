MAINTAINERCLEANFILES = Makefile.in

SUBDIRS=openflow/openflow1x

noinst_LTLIBRARIES = libxdpd_driver_fscale_l2sw_hal_imp.la

libxdpd_driver_fscale_l2sw_hal_imp_la_SOURCES = driver.cc

libxdpd_driver_fscale_l2sw_hal_imp_la_LIBADD = openflow/openflow1x/libxdpd_driver_fscale_l2sw_hal_imp_of1x.la

libxdpd_driver_fscale_l2sw_hal_imp_la_LDFLAGS = -lvtss_api -lfscale_l2sw
