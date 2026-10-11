AC_INIT([HbTK], [0.0.1.0], [https://github.com/Blame582/Hackedbox])

AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([src/Application.cpp])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
AC_PROG_RANLIB

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([HBTK], [xft])

AC_CONFIG_FILES([Makefile hbtk.pc])
AC_OUTPUT