AC_INIT(xdpd - Freescale L2Switch T1040 driver, m4_esyscmd_s(cat VERSION), rofl@bisdn.de,  xdpd_fscale_l2sw_fm)

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_GNU_SOURCE

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LD
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AUTOCONF_ENV="SHELL=/bin/sh"
LT_INIT
link_all_deplibs=yes
link_all_deplibs_CXX=yes
AC_ENABLE_STATIC
LIBTOOL="$LIBTOOL --preserve-dup-deps"
AC_SUBST(LIBTOOL)dnl

# default flags
CFLAGS="-fgnu89-inline  -Werror -Wall $CFLAGS"
CXXFLAGS="-Werror -Wall $CXXFLAGS"
LDFLAGS=" -Wl,-export-dynamic"
AC_DEFINE(__STDC_FORMAT_MACROS)

#Driver enviorment. Must go first
m4_include([../../../../config/driver.m4])

#ROFL (Must always be included *before* no_pipeline_inline.m4)
m4_include([../../../../config/rofl.m4])

#Debug
m4_include([../../../../config/debug.m4])

# Check for profiling mode
m4_include([../../../../config/profiling.m4])

# pkt classifier 
m4_include([config/pktclassifier.m4])

# Ensure rofl-pipeline platform functions are NOT inlined 
m4_include([../../../../config/no_pipeline_inline.m4])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/hal-imp/Makefile
	src/hal-imp/openflow/openflow1x/Makefile
	src/pipeline-imp/Makefile
	src/io/Makefile
	src/io/packet_classifiers/Makefile
	src/io/packet_classifiers/c_types_pktclassifier/Makefile
	src/vtss_l2sw/Makefile
	src/util/Makefile
])

AC_OUTPUT
