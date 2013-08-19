#########################
#
#  p o i n t v i w e r
#
#########################

## Initalise Autoconf...

AC_INIT([Autoconf GL Macros],[1.0])
AC_PREREQ([2.65])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CXX

## Initialise Automake...
## NOTE: use config.h option to improve passing of -D flags
#AM_CONFIG_HEADER(config.h)
# Options direct to automake 
AM_INIT_AUTOMAKE([foreign])

## Check for SDL and OpenGL (our only dependencies)...
## NOTE: our acompanying m4 folder has definitions for these checks...

AM_PATH_SDL(1.2.0,,AC_MSG_ERROR([*** SDL >= 1.2.0 not installed - please install first ***]))
AX_CHECK_GL

## Finally, assemble all definitions and generate output Makefile...

CXXFLAGS="$CXXFLAGS $SDL_CFLAGS $GL_CFLAGS"
LIBS="$LIBS $SDL_LIBS $GL_LIBS"

AC_OUTPUT
