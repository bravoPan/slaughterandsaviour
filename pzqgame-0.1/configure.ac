#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([PZQGame], [0.1], [longfeiqiu2012@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([DEPENDENCIES], [pangocairo pango cairo fontconfig freetype2 glew sdl2 SDL2_image])

AC_CANONICAL_HOST
case "${host_os}" in
     cygwin*|mingw*)
	DEPENDENCIES_LIBS+=" -lopengl32"
	;;
     darwin*)
	DEPENDENCIES_LIBS+=" -framework OpenGL"
	;;
     *)
        DEPENDENCIES_LIBS+=" -lGL"
	;;
esac

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
#AC_C_INLINE

# Checks for library functions.
#AC_CHECK_FUNCS([memset])
AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
