AC_INIT(xdpd, m4_esyscmd_s(cat VERSION), xdpd-devel@xdpd.org, xdpd, http://www.xdpd.org)

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

PKG_PROG_PKG_CONFIG

AUTOCONF_ENV="SHELL=/bin/sh"
LT_INIT
link_all_deplibs=yes
link_all_deplibs_CXX=yes
AC_ENABLE_STATIC
LIBTOOL="$LIBTOOL --preserve-dup-deps"
AC_SUBST(LIBTOOL)dnl


#Default flags
CFLAGS="-fgnu89-inline -Werror -Wall $CFLAGS"
CXXFLAGS="-Werror -Wall $CXXFLAGS"
LDFLAGS="-Wl,--as-needed -Wl,-export-dynamic"

AC_DEFINE(__STDC_FORMAT_MACROS)
AC_DEFINE(__STDC_CONSTANT_MACROS)

#Check libs used
AC_CHECK_LIB(pthread, pthread_kill,,AC_MSG_ERROR([pthread library not found])) 
AC_CHECK_LIB(rt, clock_gettime,,[AC_MSG_ERROR([rt library not found])]) 

#
#Detect srcddir and builddir
#

#Assume relative path first
XDPD_SRCDIR="$PWD/$srcdir/"
XDPD_BUILDDIR="$PWD/"

AC_MSG_CHECKING(and setting srcdir/ and builddir/)
if test ! -d "$XDPD_SRCDIR" ; then
	#Try absolute
	XDPD_SRCDIR="$srcdir/"
	if test ! -d "$XDPD_SRCDIR" ; then
		AC_ERROR("Unable to detect srcdir!  This is a bug in the build system, please report it. To workaround it, you can try manually specifying --srcdir to configure")
	fi
fi
AC_MSG_RESULT([done])

#Store autoconf variables
AC_CONFIGURE_ARGS="$ac_configure_args"
AC_SUBST(AC_CONFIGURE_ARGS)

#Perform GCC checkings
m4_include([config/gcc.m4])

#Debug
m4_include([config/debug.m4])

#Check for profiling mode
m4_include([config/profiling.m4])

#Experimental code
m4_include([config/experimental.m4])

#Pipeline optimizations
m4_include([config/pipeline_opts.m4])

#External libraries
m4_include([config/libs.m4])

#Checking OpenSSL
m4_include([config/openssl.m4])

#Boost
m4_include([config/boost.m4])

#Plugin main m4 macros
m4_include([config/plugins.m4])

#Check hardware support
m4_include([config/hw.m4])

#Check hardware support
m4_include([config/versioning.m4])

#
#Libraries
#

#ROFL
m4_include([config/rofl.m4])

#DPDK
m4_include([config/dpdk.m4])


#Output the files
AC_CONFIG_FILES([
	Makefile

	libs/Makefile

	src/Makefile
	src/xdpd/Makefile
	src/xdpd/drivers/Makefile
	src/xdpd/management/Makefile
	src/xdpd/management/plugins/Makefile
	src/xdpd/management/snapshots/Makefile
	src/xdpd/openflow/Makefile
	src/xdpd/openflow/openflow10/Makefile
	src/xdpd/openflow/openflow12/Makefile
	src/xdpd/openflow/openflow13/Makefile
	src/xdpd/openflow/pirl/Makefile

	test/Makefile
	test/xdpd/Makefile
	test/xdpd/unit/Makefile
	test/xdpd/unit/pirl/Makefile
])

# Doxygen (here to be the last Makefile) 
m4_include([config/doxygen.m4])

AC_OUTPUT
