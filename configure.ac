#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pperf], [0.0])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/pserver.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([netdb.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_LIB([pthread], [pthread_create], [], [], [])

AC_OUTPUT(Makefile src/Makefile)
