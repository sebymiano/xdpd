MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = hal-imp pipeline-imp io util vtss_l2sw

noinst_LTLIBRARIES = libxdpd_driver_fscale_l2sw_src.la

libxdpd_driver_fscale_l2sw_src_la_SOURCES = bg_taskmanager.cc config.cc
libxdpd_driver_fscale_l2sw_src_la_LIBADD = \
		hal-imp/libxdpd_driver_fscale_l2sw_src_hal_imp.la\
		pipeline-imp/libxdpd_driver_fscale_l2sw_src_pipeline_imp.la\
		io/libxdpd_driver_fscale_l2sw_src_io.la\
		vtss_l2sw/libxdpd_driver_fscale_l2sw_src_vtss_l2sw.la\
		util/libxdpd_driver_fscale_l2sw_src_util.la
		
libxdpd_driver_fscale_l2sw_src_la_LDFLAGS = -lvtss_api -lfscale_l2sw
	
