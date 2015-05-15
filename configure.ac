#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(seneyed, 0.01, ashes-iontach@hotmail.com)
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT(Makefile src/Makefile)
AM_INIT_AUTOMAKE
# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADERS([hidapi/hidapi.h])
AC_CHECK_HEADERS([libusb.h])
AC_CHECK_HEADERS([pthread.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])
AC_SEARCH_LIBS([hid_init], [hidapi-libusb], [], [
  AC_MSG_ERROR([unable to find the hid_init() function])
])
AC_SEARCH_LIBS([libusb_init], [usb-1.0], [], [
  AC_MSG_ERROR([unable to find the libusb_init() function])
])
AC_SEARCH_LIBS([pthread_mutex_lock], [pthread], [], [
  AC_MSG_ERROR([unable to find the pthread_mutex_lock() function])
])
AC_OUTPUT
