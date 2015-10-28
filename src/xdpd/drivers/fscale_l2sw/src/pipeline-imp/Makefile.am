MAINTAINERCLEANFILES = Makefile.in

noinst_LTLIBRARIES = libxdpd_driver_fscale_l2sw_src_pipeline_imp.la

libxdpd_driver_fscale_l2sw_src_pipeline_imp_la_SOURCES = \
					memory.c\
					packet.c\
					platform_hooks_of1x.cc\
					atomic_operations.c\
					pthread_lock.c\
					timing.c

libxdpd_driver_fscale_l2sw_src_pipeline_imp_la_LIBADD = 

libxdpd_driver_fscale_l2sw_src_pipeline_imp_la_LDFLAGS = -lvtss_api -lfscale_l2sw
