#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.64)
AC_INIT([PipeWalker], [0.9.4], [artemsen@gmail.com], [], [http://pipewalker.sourceforge.net])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src/cell.cpp])

# Check for some target-specific stuff
test -z "${WIN32}" && WIN32="no"
case "${build}" in
	*-mingw32*)
		WIN32="yes"
	;;
esac

AM_CONDITIONAL([WIN32], [test "${WIN32}" = "yes"])

# Checks for programs.
AC_PROG_CXX

# Check for debug option
AC_ARG_ENABLE(debug,
	[AS_HELP_STRING([--enable-debug], [enable debug data generation (default=no)])],
	[CXXFLAGS="$CXXFLAGS -g -Wall -Werror -O0"],
	[CXXFLAGS="$CXXFLAGS -DNDEBUG=1 -O3"]
)

# Checks for typedefs, structures, and compiler characteristics
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for header files
AC_HEADER_STDC
AC_HEADER_ASSERT
AC_HEADER_DIRENT
AC_CHECK_HEADER([SDL/SDL.h], [], [AC_MSG_ERROR(SDL/SDL.h header file not found)])
AC_CHECK_HEADER([GL/gl.h], [], [AC_MSG_ERROR(GL/gl.h header file not found)])
AS_IF([test "${WIN32}" = "yes"],
	[AC_CHECK_HEADER([GdiPlus.h], [], [AC_MSG_ERROR(GdiPlus.h header file not found)])],
	[AC_CHECK_HEADER([png.h], [], [AC_MSG_ERROR(png.h header file not found)])]
)
AC_CHECK_HEADERS(string map vector iterator fstream complex algorithm)

# Checks for libraries
AC_CHECK_LIB([SDL], [main], [], [AC_MSG_ERROR(SDL library not found)])
AC_CHECK_LIB([GL], [main],
	[OPENGL_LIBS="-lGL"],
	[AC_CHECK_LIB(opengl32, main,
		[OPENGL_LIBS="-lopengl32"],
		[AC_MSG_ERROR(OpenGL library not found)]
	])
)
AS_IF([test "${WIN32}" = "yes"],
	[AC_CHECK_LIB([gdiplus], [main], [], [AC_MSG_ERROR(GDI+ library not found)])],
	[
	AC_CHECK_LIB([png], [main], [], [AC_MSG_ERROR(PNG library not found)])
	]
)

# Set compiler/linker flags
AS_IF([test "${WIN32}" = "yes"],
	[CXXFLAGS="$CXXFLAGS -DPW_GAMEDATADIR='\"data\\\\\"'"],
	[CXXFLAGS="$CXXFLAGS -DPW_GAMEDATADIR='\"$datadir/$PACKAGE/\"'"]
)
LIBS="$LIBS $OPENGL_LIBS"
AS_IF([test "${WIN32}" = "yes"],
	[LDFLAGS="$LDFLAGS -mwindows -static-libgcc -static-libstdc++"]
)

AC_CONFIG_FILES(
		Makefile
		src/Makefile
		extra/Makefile
		data/Makefile
)
AC_OUTPUT
