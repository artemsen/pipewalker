AC_PREREQ(2.64)

AC_INIT([PipeWalker],
        [0.9.5],
        [https://github.com/artemsen/pipewalker/issues],
        [],
        [https://github.com/artemsen/pipewalker])

AC_LANG([C++])
AM_INIT_AUTOMAKE([foreign -Wall])
AM_SILENT_RULES([yes])

AX_CHECK_ENABLE_DEBUG([no])
AM_CONDITIONAL([DEBUG], [test "x${ax_enable_debug}" = "xyes"])

AM_CONDITIONAL([WIN32], [test "${WIN32}" = "yes"])

# Compiler flags
AX_APPEND_COMPILE_FLAGS([-Wall -Wextra], [CXXFLAGS])

# Build dependencies
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_SED

# Runtime dependencies
AC_CHECK_LIB([png], [png_read_image], [], [AC_MSG_ERROR([png lib is required])])
AC_CHECK_LIB([SDL2], [SDL_Init], [], [AC_MSG_ERROR([SDL lib is required])])
AC_CHECK_LIB([GLESv2], [glBindTexture], [], [AC_MSG_ERROR([GLESv2 lib is required])])

# Enable/disable creating of menu entry with icon
#AC_ARG_ENABLE([menu],
#              [AS_HELP_STRING([--enable-menu],
#                              [enable integration into desktop menu (default=yes)])],
#              [case "${enableval}" in
#                 yes) menu=true ;;
#                 no)  menu=false ;;
#                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-menu]) ;;
#               esac],
#              [menu=true])
#AM_CONDITIONAL([MENU], [test x$menu = xtrue])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
#                 res/Makefile
#                 extra/Makefile])
AC_OUTPUT
