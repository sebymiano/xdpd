MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = hal-imp pipeline-imp io vtss_l2sw

noinst_LTLIBRARIES = libxdpd_driver_fscale_l2sw_src.la

libxdpd_driver_fscale_l2sw_src_la_SOURCES = bg_taskmanager.cc config.cc
libxdpd_driver_fscale_l2sw_src_la_LIBADD = \
		hal-imp/libxdpd_driver_fscale_l2sw_hal_imp.la\
		pipeline-imp/libxdpd_driver_fscale_l2sw_pipeline_imp.la\
		io/libxdpd_driver_fscale_l2sw_io.la\
		vtss_l2sw/libxdpd_driver_fscale_l2sw_vtss_l2sw.la
		
libxdpd_driver_fscale_l2sw_src_la_LDFLAGS = -lvtss_api -lfscale_l2sw #-Iadd_header_path_here
