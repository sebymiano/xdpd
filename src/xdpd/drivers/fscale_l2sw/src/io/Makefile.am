MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = packet_classifiers

noinst_LTLIBRARIES = libxdpd_driver_fscale_l2sw_src_io.la

libxdpd_driver_fscale_l2sw_src_io_la_SOURCES = \
					bufferpool.cc\
					datapacket_storage.cc\
					datapacketx86.cc\
					iface_utils.c

#Define classifier
CLASSIFIER_LIB=packet_classifiers/c_types_pktclassifier/libxdpd_driver_fscale_l2sw_io_classifiers_c_types.la


libxdpd_driver_fscale_l2sw_src_io_la_LIBADD = $(CLASSIFIER_LIB)
	
libxdpd_driver_fscale_l2sw_src_io_la_LDFLAGS = -lvtss_api -lfscale_l2sw

