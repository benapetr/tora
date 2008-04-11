##   -*- mode: autoconf; tab-width: 2; -*-

dnl mrjohnson0: nicked this from a kdevelop-started package
dnl - it didn't work without KDE, so i fixed that.
dnl - then it didn't work on the mac, so i removed the libtool stuff.
dnl - then it still had some problems, so my hacks are all over the place.
dnl
dnl blame me. mrjohnson0@users.sourceforge.net :-)

dnl    This file is part of the KDE libraries/packages
dnl    Copyright (C) 1997 Janos Farkas (chexum@shadow.banki.hu)
dnl              (C) 1997,98,99 Stephan Kulow (coolo@kde.org)

dnl    This file is free software; you can redistribute it and/or
dnl    modify it under the terms of the GNU Library General Public
dnl    License as published by the Free Software Foundation; either
dnl    version 2 of the License, or (at your option) any later version.

dnl    This library is distributed in the hope that it will be useful,
dnl    but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl    Library General Public License for more details.

dnl    You should have received a copy of the GNU Library General Public License
dnl    along with this library; see the file COPYING.LIB.  If not, write to
dnl    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
dnl    Boston, MA 02111-1307, USA.

dnl ------------------------------------------------------------------------
dnl Forward compatibility macros (make autoconf 2.13 look like 2.50),
dnl thanks to Raja R Harinath.
dnl ------------------------------------------------------------------------
dnl
dnl ifdef([_AC_PATH_X_XMKMF],[],
dnl    [AC_DEFUN([_AC_PATH_X_XMKMF],[AC_PATH_X_XMKMF])])
dnl ifdef([AC_OUTPUT_SUBDIRS],[],
dnl    [AC_DEFUN([AC_OUTPUT_SUBDIRS],[subdirs=$1; _AC_OUTPUT_SUBDIRS])])


dnl mrj
dnl check for qscintilla, fail if no
AC_DEFUN([TORA_CHECK_QSCINTILLA],
[
  AC_MSG_CHECKING([for qscintilla])

  scin_cflags=
  scin_ldflags=

  qscintilla_user_inc=
  AC_ARG_WITH(qscintilla-includes,
  [  --with-qscintilla-includes=DIR
                          QScintilla header file location],
  [
     qscintilla_user_inc=$withval
  ], )

  qscintilla_user_lib=
  AC_ARG_WITH(qscintilla-libraries,
  [   --with-qscintilla-libraries
                          QScintilla library dir],
  [
    qscintilla_user_lib=$withval
  ], )

  scin_check_inc="
    $qscintilla_user_inc
    /usr/include/qscintilla"

  for dir in $scin_check_inc; do
    if test -d $dir; then
      scin_cflags="$scin_cflags -I$dir"
    fi
  done

  scin_check_lib="
    $qscintilla_user_lib"

  for dir in $scin_check_lib; do
    if test -d $dir; then
      scin_ldflags="$scin_ldflags -L$dir"
    fi
  done

  dnl test that the library works
  cflags_scin_save=$CXXFLAGS
  ldflags_scin_save=$LDFLAGS
  libs_scin_save=$LIBS

  CXXFLAGS="$CXXFLAGS $KDE_INCLUDES $QT_INCLUDES $scin_cflags"
  LDFLAGS="$LDFLAGS
    @KDE_LDFLAGS@ \
    @X_LDFLAGS@ \
    @QT_LDFLAGS@ \
    @LIB_KPARTS@ \
    @LIB_KDEPRINT@ \
    @LIB_KDECORE@ \
    @LIB_KDEUI@ \
    @LIB_KIO@ \
    @LIB_KFILE@ \
    @LIB_DCOP@ \
    @LIB_KHTML@ \
    $scin_ldflags"
  LIBS="-lqscintilla2"

  AC_LANG_PUSH(C++)

  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <Qsci/qscilexersql.h>
    ]],
    [[QsciLexerSQL sqlLexer(0);]])],
    scin_works=yes,
    scin_works=no)

  AC_LANG_POP([C++])

  if test $scin_works = no; then
    AC_MSG_ERROR([Couldn't compile a simple QScintilla application. See config.log or specify its location with --with-qscintilla-includes])
  fi

  CXXFLAGS=$cflags_scin_save
  LDFLAGS=$ldflags_scin_save
  LIBS=$libs_scin_save

  scin_dll=0
  case $host_os in
  cygwin* | mingw*)
      scin_dll=1
      ;;
  esac

  AC_DEFINE_UNQUOTED(QSCINTILLA_DLL, 1, [True if on Windows.])
  AC_SUBST(QSCINTILLA_CXXFLAGS, $scin_cflags)
  AC_SUBST(QSCINTILLA_LDFLAGS, "$scin_ldflags -lqscintilla2")
  AC_MSG_RESULT(yes)
])

dnl --------------------------------------------------
dnl find out if we're running qt/mac native.
dnl doesn't print "checking..." messages because it's included from AC_PATH_QT
dnl --------------------------------------------------
AC_DEFUN([TORA_CHECK_MAC_NATIVE],
[

  ac_cppflags_safe="$CPPFLAGS"
  CPPFLAGS="$CPPFLAGS -I$qt_incdir $all_includes $X_CFLAGS"

  dnl only run the preprocessor, this will always fail if compiling
  dnl without the proper qt/mac includes.
  AC_PREPROC_IFELSE([
#include <Qt/qapplication.h>

#ifndef Q_OS_MACX
# error no
#endif
  ],
  have_qt_mac_native=yes,
  have_qt_mac_native=no)

  if test $have_qt_mac_native = no; then
    echo "no mac native" >&AS_MESSAGE_LOG_FD()
  else
    echo "using mac native" >&AS_MESSAGE_LOG_FD()
    dnl don't bother testing, these should always work on the mac.
    dnl if these change in future OS releases, then we'll add some checks.
    LIBS="$LIBS -bind_at_load -framework Carbon -framework QuickTime -lz -framework OpenGL -framework AGL"
  fi

  CPPFLAGS="$ac_cppflags_safe"
])

dnl ------------------------------------------------------------------------
dnl Try to find the Qt headers and libraries.
dnl $(QT_LDFLAGS) will be -Lqtliblocation (if needed)
dnl and $(QT_INCLUDES) will be -Iqthdrlocation (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT],
[
  AC_REQUIRE([AC_PATH_XTRA])
  AC_REQUIRE([TORA_DEBUG])
  dnl AC_REQUIRE([AC_FIND_PNG])
  dnl AC_REQUIRE([AC_FIND_JPEG])

  dnl make qtdir precious
  AC_ARG_VAR([QTDIR], [Path to your QT folder. Not required but still useful.])

  AC_MSG_CHECKING([for Qt])
  
  ac_qt_includes=NO ac_qt_libraries=NO ac_qt_bindir=NO
  qt_libraries=""
  qt_includes=""
  AC_ARG_WITH(qt-dir,
      [  --with-qt-dir=DIR       where the root of Qt is installed ],
      [  ac_qt_includes="$withval"/include
         ac_qt_libraries="$withval"/lib
         ac_qt_bindir="$withval"/bin
      ])
  
  AC_ARG_WITH(qt-includes,
      [  --with-qt-includes=DIR  where the Qt includes are. ],
      [
         ac_qt_includes="$withval"
      ])
  
  AC_ARG_WITH(qt-include,
      [  --with-qt-include=DIR   same as above. ],
      [
         ac_qt_includes="$ac_qt_includes $withval"
      ])

  AC_ARG_WITH(qt-libraries,
      [  --with-qt-libraries=DIR where the Qt library is installed.],
      [  ac_qt_libraries="$withval" ])
  
  AC_CACHE_VAL(ac_cv_have_qt,
  [#try to guess Qt locations
  
  qt_incdirs=""
  for dir in $kde_qt_dirs; do
     qt_incdirs="$qt_incdirs $dir/include $dir"
  done
  qt_incdirs="$QTINC
              $qt_incdirs
              $QTDIR/include
              /usr/local/qt/include
              /usr/include/qt
              /usr/include/qt4
              /usr/include
              /usr/X11R6/include/X11/qt
              /usr/X11R6/include/qt
              /usr/X11R6/include/qt4
              /Developer/qt/include
              /sw/include/qt
              /usr/qt/4/include
              /usr/lib/qt4/include
              $x_includes"
  if test "$ac_qt_includes" != "NO"; then
     qt_incdirs="$ac_qt_includes $qt_incdirs"
  fi
  
  kde_qt_header=Qt/qglobal.h
  
  AC_FIND_FILE($kde_qt_header, $qt_incdirs, qt_incdir)
  ac_qt_includes="$qt_incdir"
  
  qt_libdirs="$ac_qt_libraries
              $QTLIB
              $qt_libdirs
              $QTDIR/lib
              /Developer/qt/lib
              /sw/lib
              /usr/X11R6/lib
              /usr/lib
              /usr/local/qt/lib
              /usr/lib/qt4/lib
              $x_libraries"

  ac_qt_libdir=NO
  LIB_QT_GLOB=QtCore
  ac_lib_type=

  for dir in $qt_libdirs; do
    dnl try debug v 4
    if test "x$tora_debug" = "xyes"; then
      try="ls -1 $dir/*${LIB_QT_GLOB}d4.*"
      echo "trying $try" >&AS_MESSAGE_LOG_FD()
      if test -n "`$try 2> /dev/null`"; then
        ac_qt_libdir="$dir"
        ac_lib_type=d4
        break
      fi

      dnl didn't find (eg QtCored4.dll), drop the 4
      try="ls -1 $dir/*${LIB_QT_GLOB}d.*"
       echo "trying $try" >&AS_MESSAGE_LOG_FD()
      if test -n "`$try 2> /dev/null`"; then
        ac_qt_libdir="$dir"
        ac_lib_type=d
        break
      fi
    dnl if debug
    else
      try="ls -1 $dir/*${LIB_QT_GLOB}4.*"
      echo "trying $try" >&AS_MESSAGE_LOG_FD()
      if test -n "`$try 2> /dev/null`"; then
        ac_qt_libdir="$dir"
        ac_lib_type=4
        break
      fi

      dnl didn't find (eg QtCore4.dll), drop the 4
      try="ls -1 $dir/*${LIB_QT_GLOB}.*"
      echo "trying $try" >&AS_MESSAGE_LOG_FD()
      if test -n "`$try 2> /dev/null`"; then
        ac_qt_libdir="$dir"
        ac_lib_type=
        break
      fi
    dnl if debug else
    fi    
  done

  dnl warn if nothing found
  if test "x$ac_qt_libdir" = "xNO"; then
    AC_MSG_WARN([no library folders found])
  fi

  dnl all qt libary names needed for tora:
  pv_qt_libs="QtCore QtGui QtGui QtSql QtNetwork"
  all_qt_libs=

  dnl loop through libs and append type
  for needed in $pv_qt_libs; do
    all_qt_libs="$all_qt_libs -l${needed}${ac_lib_type}"
  done
  
  dnl before we try to compile, check for native qt on the mac.
  dnl it requires additional flags.
  dnl this check must be run after the above tests to find the qt dir,
  dnl but before trying to compile because we have no other way to know
  dnl if we're compiling against qt/x11 or qt/mac. if we need to add
  dnl more platforms with custom libraries in this manner, i'll probably
  dnl separate the qt tests into testing linking/compiling and finding QTDIR.
  TORA_CHECK_MAC_NATIVE

  K_PATH_X

  ac_cxxflags_safe="$CXXFLAGS"
  ac_ldflags_safe="$LDFLAGS"
  ac_libs_safe="$LIBS"
  
  CXXFLAGS="$CXXFLAGS -I$qt_incdir $all_includes $X_CFLAGS"
  LDFLAGS="$LDFLAGS -L$ac_qt_libdir $all_libraries $USER_LDFLAGS $KDE_MT_LDFLAGS"
  LIBS="$LIBS $all_qt_libs $X_LIBS"

  AC_LANG([C++])

  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([
#include "confdefs.h"
#include <Qt/qglobal.h>
#include <Qt/qapplication.h>
#include <Qt/qevent.h>
#include <Qt/qstring.h>
], [
  QString::fromLatin1("Elvis is alive");])],
  qt_compiled=yes,
  qt_compiled=no)

  CXXFLAGS="$ac_cxxflags_safe"
  LDFLAGS="$ac_ldflags_safe"
  LIBS="$ac_libs_safe"
  
  if test "$qt_compiled" = no; then
    have_qt=no
    AC_MSG_ERROR([Qt not found.
      Please check your installation! For more details about this problem,
      look at the end of config.log.])
  else
    have_qt=yes
  fi
  ])
  
  eval "$ac_cv_have_qt"
  
  if test "x$have_qt" != xyes; then
    AC_MSG_RESULT([$have_qt]);
  else
    ac_cv_have_qt="have_qt=yes \
      ac_qt_includes=$ac_qt_includes ac_qt_libraries=$ac_qt_libraries"
    qt_libraries="$ac_qt_libraries"
    qt_includes="$ac_qt_includes"
  
    AC_SUBST(qt_libraries)
    AC_SUBST(qt_includes)
  
    AC_MSG_RESULT([
      libqt:     $all_qt_libs,
      libraries: $ac_qt_libdir,
      headers:   $ac_qt_includes])
  fi
  
  QT_DEFINES=""
  dnl mrj - TODO this is just a hack for now, should test each -I
  QT_INCLUDES="-I$qt_includes -I$qt_incdir/Qt -I$qt_incdir/QtCore -I$qt_incdir/QtGui -I$qt_incdir/QtSql -I$qt_incdir/QtNetwork"
  all_includes="$QT_INCLUDES $all_includes"
  
  QT_LDFLAGS="$all_qt_libs"
  if test "x$ac_qt_libdir" != "xNO"; then
    QT_LDFLAGS="$QT_LDFLAGS -L$ac_qt_libdir"
  fi
  
  AC_SUBST(QT_DEFINES)
  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_LDFLAGS)
  AC_PATH_QT_MOC_UIC
])

dnl ------------------------------------------------------------------------
dnl Find the header files and libraries for X-Windows. Extended the
dnl macro AC_PATH_X
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([K_PATH_X],
[
AC_REQUIRE([KDE_MISC_TESTS])dnl - may not have kde
AC_REQUIRE([KDE_CHECK_LIB64])

AC_ARG_ENABLE(
  embedded,
  [  --enable-embedded       link to Qt-embedded, don't use X],
  kde_use_qt_emb=$enableval,
  kde_use_qt_emb=no
)

AC_ARG_ENABLE(
  qtopia,
  [  --enable-qtopia         link to Qt-embedded, link to the Qtopia Environment],
  kde_use_qt_emb_palm=$enableval,
  kde_use_qt_emb_palm=no
)

case $host_os in
cygwin* | mingw*)
  # no x for windows
  ;;
*)
  if test "$kde_use_qt_emb" = "no"; then
    AC_MSG_CHECKING(for X)
    AC_LANG_PUSH(C)
    AC_CACHE_VAL(kde_cv_have_x,
    [# One or both of the vars are not set, and there is no cached value.
    if test "{$x_includes+set}" = set || test "$x_includes" = NONE; then
       kde_x_includes=NO
    else
       kde_x_includes=$x_includes
    fi
    if test "{$x_libraries+set}" = set || test "$x_libraries" = NONE; then
       kde_x_libraries=NO
    else
       kde_x_libraries=$x_libraries
    fi
    
    # below we use the standard autoconf calls
    ac_x_libraries=$kde_x_libraries
    ac_x_includes=$kde_x_includes
    
    KDE_PATH_X_DIRECT
    dnl AC_PATH_X_XMKMF picks /usr/lib as the path for the X libraries.
    dnl Unfortunately, if compiling with the N32 ABI, this is not the correct
    dnl location. The correct location is /usr/lib32 or an undefined value
    dnl (the linker is smart enough to pick the correct default library).
    dnl Things work just fine if you use just AC_PATH_X_DIRECT.
    dnl Solaris has a similar problem. AC_PATH_X_XMKMF forces x_includes to
    dnl /usr/openwin/include, which doesn't work. /usr/include does work, so
    dnl x_includes should be left alone.
    case "$host" in
    mips-sgi-irix6*)
      ;;
    *-*-solaris*)
      ;;
    *)
      _AC_PATH_X_XMKMF
      if test -z "$ac_x_includes"; then
        ac_x_includes="."
      fi
      if test -z "$ac_x_libraries"; then
        ac_x_libraries="/usr/lib${kdelibsuff}"
      fi
    esac
    #from now on we use our own again
    
    # when the user already gave --x-includes, we ignore
    # what the standard autoconf macros told us.
    if test "$kde_x_includes" = NO; then
      kde_x_includes=$ac_x_includes
    fi
    
    # for --x-libraries too
    if test "$kde_x_libraries" = NO; then
      kde_x_libraries=$ac_x_libraries
    fi
    
    if test "$kde_x_includes" = NO; then
      AC_MSG_ERROR([Can't find X includes. Please check your installation and add the correct paths!])
    fi
    
    if test "$kde_x_libraries" = NO; then
      AC_MSG_ERROR([Can't find X libraries. Please check your installation and add the correct paths!])
    fi
    
    # Record where we found X for the cache.
    kde_cv_have_x="have_x=yes \
             kde_x_includes=$kde_x_includes kde_x_libraries=$kde_x_libraries"
    ])dnl
    
    eval "$kde_cv_have_x"
    
    if test "$have_x" != yes; then
      AC_MSG_RESULT($have_x)
      no_x=yes
    else
      AC_MSG_RESULT([libraries $kde_x_libraries, headers $kde_x_includes])
    fi
    
    if test -z "$kde_x_includes" || test "x$kde_x_includes" = xNONE; then
      X_INCLUDES=""
      x_includes="."; dnl better than nothing :-
     else
      x_includes=$kde_x_includes
      X_INCLUDES="-I$x_includes"
    fi
    
    if test -z "$kde_x_libraries" || test "x$kde_x_libraries" = xNONE; then
      X_LDFLAGS=""
      x_libraries="/usr/lib"; dnl better than nothing :-
     else
      x_libraries=$kde_x_libraries
      X_LDFLAGS="-L$x_libraries"
    fi
    all_includes="$X_INCLUDES"
    all_libraries="$X_LDFLAGS"
    
    AC_SUBST(X_INCLUDES)
    AC_SUBST(X_LDFLAGS)
    AC_SUBST(x_libraries)
    AC_SUBST(x_includes)
    
    # Check for libraries that X11R6 Xt/Xaw programs need.
    ac_save_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS $X_LDFLAGS"
    # SM needs ICE to (dynamically) link under SunOS 4.x (so we have to
    # check for ICE first), but we must link in the order -lSM -lICE or
    # we get undefined symbols.  So assume we have SM if we have ICE.
    # These have to be linked with before -lX11, unlike the other
    # libraries we check for below, so use a different variable.
    #  --interran@uluru.Stanford.EDU, kb@cs.umb.edu.
    AC_CHECK_LIB(ICE, IceConnectionNumber,
      [LIBSM="-lSM -lICE"], , $X_EXTRA_LIBS)
    AC_SUBST(LIBSM)
    LDFLAGS="$ac_save_LDFLAGS"
    
    AC_SUBST(X_PRE_LIBS)
    
    LIB_X11='-lX11 $(LIBSOCKET)'
    AC_SUBST(LIB_X11)
    
    AC_MSG_CHECKING(for libXext)
    AC_CACHE_VAL(kde_cv_have_libXext,
    [
    kde_ldflags_safe="$LDFLAGS"
    kde_libs_safe="$LIBS"
    
    LDFLAGS="$LDFLAGS $X_LDFLAGS $USER_LDFLAGS"
    LIBS="-lXext -lX11 $LIBSOCKET"
    
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    #include <stdio.h>
    #ifdef STDC_HEADERS
    # include <stdlib.h>
    #endif
    ]], 
    [[printf("hello Xext\n");]])],
    kde_cv_have_libXext=yes,
    kde_cv_have_libXext=no
       )
    
    LDFLAGS=$kde_ldflags_safe
    LIBS=$kde_libs_safe
     ])
    
    AC_MSG_RESULT($kde_cv_have_libXext)
    
    if test "$kde_cv_have_libXext" = "no"; then
      AC_MSG_ERROR([We need a working libXext to proceed. Since configure
    can't find it itself, we stop here assuming that make wouldn't find
    them either.])
    fi
    
    AC_MSG_CHECKING(for Xinerama)
    
     AC_ARG_WITH(xinerama,
      [  --with-xinerama         enable support for Xinerama ],
      [
        no_xinerama=no
      ], [
        no_xinerama=yes
      ]
    )
    
    kde_save_LDFLAGS="$LDFLAGS"
    kde_save_CFLAGS="$CFLAGS"
    kde_save_LIBS="$LIBS"
    LDFLAGS="$LDFLAGS $X_LDFLAGS $USER_LDFLAGS"
    CFLAGS="$CFLAGS -I$x_includes"
    LIBS="-lXinerama -lXext"
    
    if test "x$no_xinerama" = "xno"; then
    
      AC_CACHE_VAL(ac_cv_have_xinerama,
      [
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[
            #include <X11/Xlib.h>
            #include <X11/extensions/Xinerama.h>]],
            [[XineramaIsActive(NULL);]])],
          [ac_cv_have_xinerama="yes"],
          [ac_cv_have_xinerama="no"])
      ])
    else
      ac_cv_have_xinerama=no;
    fi
    
    AC_MSG_RESULT($ac_cv_have_xinerama)
    
    LIBXINERAMA=""
    
    if test "$ac_cv_have_xinerama" = "yes"; then
      AC_DEFINE(HAVE_XINERAMA, 1, [Define if you want Xinerama support])
      LIBXINERAMA="-lXinerama"
    fi
    
    AC_SUBST(LIBXINERAMA)
    
    LDFLAGS="$kde_save_LDFLAGS"
    CFLAGS="$kde_save_CFLAGS"
    LIBS="$kde_save_LIBS"
    
    LIB_XEXT="-lXext"
    QTE_NORTTI=""
  else
    dnl We're using QT Embedded
    CXXFLAGS=-DQWS
    CXXFLAGS="$CXXFLAGS -fno-rtti"
    QTE_NORTTI="-fno-rtti -DQWS"
    X_PRE_LIBS=""
    LIB_X11=""
    LIB_XEXT=""
    LIBSM=""
    X_INCLUDES=""
    X_LDFLAGS=""
    x_includes=""
    x_libraries=""
    AC_SUBST(X_PRE_LIBS)
    AC_SUBST(LIB_X11)
    AC_SUBST(LIBSM)
    AC_SUBST(X_INCLUDES)
    AC_SUBST(X_LDFLAGS)
    AC_SUBST(x_includes)
    AC_SUBST(x_libraries)
  fi
  ;;
  # end host case
esac

AC_SUBST(QTE_NORTTI)
AC_SUBST(LIB_XEXT)

AC_LANG_POP([C])
])

# KDE_PATH_X_DIRECT
dnl Internal subroutine of AC_PATH_X.
dnl Set ac_x_includes and/or ac_x_libraries.
AC_DEFUN([KDE_PATH_X_DIRECT],
[
AC_REQUIRE([KDE_CHECK_LIB64])

if test "$ac_x_includes" = NO; then
  # Guess where to find include files, by looking for this one X11 .h file.
  test -z "$x_direct_test_include" && x_direct_test_include=X11/Intrinsic.h

  # First, try using that file with no special directory specified.
AC_PREPROC_IFELSE([#include <$x_direct_test_include>],
[# We can compile using X headers with no special include directory.
ac_x_includes=],
[# Look for the header file in a standard set of common directories.
# Check X11 before X11Rn because it is often a symlink to the current release.
  for ac_dir in               \
    /usr/X11/include          \
    /usr/X11R6/include        \
    /usr/X11R5/include        \
    /usr/X11R4/include        \
                              \
    /usr/include/X11          \
    /usr/include/X11R6        \
    /usr/include/X11R5        \
    /usr/include/X11R4        \
                              \
    /usr/local/X11/include    \
    /usr/local/X11R6/include  \
    /usr/local/X11R5/include  \
    /usr/local/X11R4/include  \
                              \
    /usr/local/include/X11    \
    /usr/local/include/X11R6  \
    /usr/local/include/X11R5  \
    /usr/local/include/X11R4  \
                              \
    /usr/X386/include         \
    /usr/x386/include         \
    /usr/XFree86/include/X11  \
                              \
    /usr/include              \
    /usr/local/include        \
    /usr/unsupported/include  \
    /usr/athena/include       \
    /usr/local/x11r5/include  \
    /usr/lpp/Xamples/include  \
                              \
    /usr/openwin/include      \
    /usr/openwin/share/include \
    ; \
  do
    if test -r "$ac_dir/$x_direct_test_include"; then
      ac_x_includes=$ac_dir
      break
    fi
  done])
fi # $ac_x_includes = NO

if test "$ac_x_libraries" = NO; then
  # Check for the libraries.

  test -z "$x_direct_test_library" && x_direct_test_library=Xt
  test -z "$x_direct_test_function" && x_direct_test_function=XtMalloc

  # See if we find them without any special options.
  # Don't add to $LIBS permanently.
  ac_save_LIBS="$LIBS"
  LIBS="-l$x_direct_test_library $LIBS"

AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[${x_direct_test_function}()]])],
# AC_TRY_LINK(, [${x_direct_test_function}()],
[LIBS="$ac_save_LIBS"
# We can link X programs with no special library path.
ac_x_libraries=],
[LIBS="$ac_save_LIBS"
# First see if replacing the include by lib works.
# Check X11 before X11Rn because it is often a symlink to the current release.
for ac_dir in `echo "$ac_x_includes" | sed s/include/lib/` \
    /usr/X11/lib                        \
    /usr/X11R6/lib                      \
    /usr/X11R5/lib                      \
    /usr/X11R4/lib                      \
                                        \
    /usr/lib/X11                        \
    /usr/lib/X11R6                      \
    /usr/lib/X11R5                      \
    /usr/lib/X11R4                      \
                                        \
    /usr/local/X11/lib                  \
    /usr/local/X11R6/lib                \
    /usr/local/X11R5/lib                \
    /usr/local/X11R4/lib                \
                                        \
    /usr/local/lib/X11                  \
    /usr/local/lib/X11R6                \
    /usr/local/lib/X11R5                \
    /usr/local/lib/X11R4                \
                                        \
    /usr/X386/lib                       \
    /usr/x386/lib                       \
    /usr/XFree86/lib/X11                \
                                        \
    /usr/lib                            \
    /usr/local/lib                      \
    /usr/unsupported/lib                \
    /usr/athena/lib                     \
    /usr/local/x11r5/lib                \
    /usr/lpp/Xamples/lib                \
    /lib/usr/lib/X11                    \
                                        \
    /usr/openwin/lib                    \
    /usr/openwin/share/lib              \
    ; \
do
dnl Don't even attempt the hair of trying to link an X program!

dnl mrj: i removed the suffix {kdelibsuff} references above, so we
dnl have to also test them here, too. this is needed because not all 64 bit
dnl platforms use lib64 path exclusively (debian amd64 for one).

  for ac_extension in a so sl; do
    if test -r $ac_dir/lib${x_direct_test_library}.$ac_extension; then
      echo "tried x dir $ac_dir" >&AS_MESSAGE_LOG_FD()
      ac_x_libraries=$ac_dir
      break 2
    fi

    for kdelibsuff in 32 64; do
      ac_x_dir_with_suff=`echo $ac_dir | sed "s/lib/lib${kdelibsuff}/"`
      if test -r $ac_x_dir_with_suff/lib${x_direct_test_library}.$ac_extension; then
        echo "tried x dir $ac_dir" >&AS_MESSAGE_LOG_FD()
        ac_x_libraries=$ac_dir
        break 3
      fi
    done
  done
done])
fi # $ac_x_libraries = NO
])

AC_DEFUN([KDE_CHECK_LIB64],
[
    kdelibsuff=none
    AC_ARG_ENABLE(libsuffix,
    AS_HELP_STRING(--enable-libsuffix,/lib directory suffix (64,32,none)),
            kdelibsuff=$enableval)
    if test "$kdelibsuff" = "none"; then
        if test -d /lib64 ; then
            kdelibsuff=64
        else
            kdelibsuff=
        fi
    fi
    if test -z "$kdelibsuff"; then
        AC_MSG_RESULT([not using lib directory suffix])
        AC_DEFINE(KDELIBSUFF, [""], Suffix for lib directories)
    else
        if test "$libdir" = '${exec_prefix}/lib'; then
            libdir="$libdir${kdelibsuff}"
            AC_SUBST([libdir], ["$libdir"])  dnl ugly hack for lib64 platforms
        fi
        AC_DEFINE_UNQUOTED(KDELIBSUFF, ["\"${kdelibsuff}\""], Suffix for lib directories)
        AC_MSG_RESULT([using lib directory suffix $kdelibsuff])
    fi
])

AC_DEFUN([KDE_MOC_ERROR_MESSAGE],
[
    AC_MSG_ERROR([No Qt meta object compiler (moc) found!
Please check whether you installed Qt correctly.
You need to have a running moc binary.
configure tried to run $ac_cv_path_moc and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable MOC to the right one before running
configure.
])
])

AC_DEFUN([KDE_UIC_ERROR_MESSAGE],
[
    AC_MSG_WARN([No Qt ui compiler (uic) found!
Please check whether you installed Qt correctly.
You need to have a running uic binary.
configure tried to run $ac_cv_path_uic and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable UIC to the right one before running
configure.
])
])

AC_DEFUN([KDE_LRELEASE_ERROR_MESSAGE],
[
    AC_MSG_WARN([No lrelease found!
Please check whether you installed Qt correctly.
You need to have a running lrelease binary.
configure tried to run $ac_cv_path_lrelease and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable LRELEASE to the right one before running
configure.
])
])

AC_DEFUN([KDE_LUPDATE_ERROR_MESSAGE],
[
    AC_MSG_WARN([No lupdate found!
Please check whether you installed Qt correctly.
You need to have a running lupdate binary.
configure tried to run $ac_cv_path_lupdate and the test didn't
succeed. If configure shouldn't have tried this one, set
the environment variable LUPDATE to the right one before running
configure.
])
])

AC_DEFUN([KDE_CHECK_UIC_FLAG],
[
    AC_MSG_CHECKING([whether uic supports -$1 ])
    kde_cache=`echo $1 | sed 'y% .=/+-%____p_%'`
    AC_CACHE_VAL(kde_cv_prog_uic_$kde_cache,
    [
        cat >conftest.ui <<EOT
        <!DOCTYPE UI><UI version="3" stdsetdef="1"></UI>
EOT
        ac_uic_testrun="$UIC_PATH -$1 $2 conftest.ui >/dev/null"
        if AC_TRY_EVAL(ac_uic_testrun); then
            eval "kde_cv_prog_uic_$kde_cache=yes"
        else
            eval "kde_cv_prog_uic_$kde_cache=no"
        fi
        rm -f conftest*
    ])

    if eval "test \"`echo '$kde_cv_prog_uic_'$kde_cache`\" = yes"; then
        AC_MSG_RESULT([yes])
        :
        $3
    else
        AC_MSG_RESULT([no])
        :
        $4
    fi
])

dnl ------------------------------------------------------------------------
dnl mrj: hacked to find lrelease and lupdate, too
dnl Find the meta object compiler and the ui compiler in the PATH,
dnl in $QTDIR/bin, and some more usual places
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_PATH_QT_MOC_UIC],
[
   qt_bindirs=""
   for dir in $kde_qt_dirs; do
      qt_bindirs="$qt_bindirs $dir/bin $dir/src/moc"
   done
   qt_bindirs="$qt_bindirs /usr/bin /usr/X11R6/bin /usr/local/qt/bin"
   if test ! "$ac_qt_bindir" = "NO"; then
      qt_bindirs="$ac_qt_bindir $qt_bindirs"
   fi

   dnl AC_ARG_VAR([LRELEASE])
   dnl AC_ARG_VAR([LUPDATE])
   dnl AC_ARG_VAR([MOC])

   KDE_FIND_PATH(lrelease, LRELEASE, [$qt_bindirs], [KDE_LRELEASE_ERROR_MESSAGE])
   KDE_FIND_PATH(lupdate, LUPDATE, [$qt_bindirs], [KDE_LUPDATE_ERROR_MESSAGE])

   KDE_FIND_PATH(moc, MOC, [$qt_bindirs], [KDE_MOC_ERROR_MESSAGE])
   if test -z "$UIC_NOT_NEEDED"; then
     KDE_FIND_PATH(uic, UIC_PATH, [$qt_bindirs], [UIC_PATH=""])
     if test -z "$UIC_PATH" ; then
       KDE_UIC_ERROR_MESSAGE
       exit 1
     else
       KDE_CHECK_UIC_FLAG(L,[/nonexistant],ac_uic_supports_libpath=yes,ac_uic_supports_libpath=no)
       KDE_CHECK_UIC_FLAG(nounload,,ac_uic_supports_nounload=yes,ac_uic_supports_nounload=no)

       UIC=$UIC_PATH
       if test x$ac_uic_supports_libpath = xyes; then
           UIC="$UIC -L \$(kde_widgetdir)"
       fi
       if test x$ac_uic_supports_nounload = xyes; then
           UIC="$UIC -nounload"
       fi
     fi
   else
     UIC="echo uic not available: "
   fi

   AC_SUBST(LRELEASE)
   AC_SUBST(LUPDATE)
   AC_SUBST(MOC)
   AC_SUBST(UIC)
])

AC_DEFUN([KDE_MISC_TESTS],
[
   AC_LANG(C)
   dnl Checks for libraries.
   AC_CHECK_LIB(util, main, [LIBUTIL="-lutil"]) dnl for *BSD 
   AC_SUBST(LIBUTIL)
   AC_CHECK_LIB(compat, main, [LIBCOMPAT="-lcompat"]) dnl for *BSD
   AC_SUBST(LIBCOMPAT)
   kde_have_crypt=
   AC_CHECK_LIB(crypt, crypt, [LIBCRYPT="-lcrypt"; kde_have_crypt=yes],
      AC_CHECK_LIB(c, crypt, [kde_have_crypt=yes], [
        AC_MSG_WARN([you have no crypt in either libcrypt or libc.
You should install libcrypt from another source or configure with PAM
support])
	kde_have_crypt=no
      ]))
   AC_SUBST(LIBCRYPT)
   if test $kde_have_crypt = yes; then
      AC_DEFINE_UNQUOTED(HAVE_CRYPT, 1, [Defines if your system has the crypt function])
   fi
   AC_CHECK_SOCKLEN_T
   AC_LANG(C)
   AC_CHECK_LIB(dnet, dnet_ntoa, [X_EXTRA_LIBS="$X_EXTRA_LIBS -ldnet"])
   if test $ac_cv_lib_dnet_dnet_ntoa = no; then
      AC_CHECK_LIB(dnet_stub, dnet_ntoa,
        [X_EXTRA_LIBS="$X_EXTRA_LIBS -ldnet_stub"])
   fi
   AC_CHECK_FUNC(inet_ntoa)
   if test $ac_cv_func_inet_ntoa = no; then
     AC_CHECK_LIB(nsl, inet_ntoa, X_EXTRA_LIBS="$X_EXTRA_LIBS -lnsl")
   fi
   AC_CHECK_FUNC(connect)
   if test $ac_cv_func_connect = no; then
      AC_CHECK_LIB(socket, connect, X_EXTRA_LIBS="-lsocket $X_EXTRA_LIBS", ,
        $X_EXTRA_LIBS)
   fi

   AC_CHECK_FUNC(remove)
   if test $ac_cv_func_remove = no; then
      AC_CHECK_LIB(posix, remove, X_EXTRA_LIBS="$X_EXTRA_LIBS -lposix")
   fi

   # BSDI BSD/OS 2.1 needs -lipc for XOpenDisplay.
   AC_CHECK_FUNC(shmat, ,
     AC_CHECK_LIB(ipc, shmat, X_EXTRA_LIBS="$X_EXTRA_LIBS -lipc"))
   
   # darwin needs this to initialize the environment
   AC_CHECK_HEADERS(crt_externs.h)
   AC_CHECK_FUNC(_NSGetEnviron, [AC_DEFINE(HAVE_NSGETENVIRON, 1, [Define if your system needs _NSGetEnviron to set up the environment])])
 
   # more headers that need to be explicitly included on darwin
   AC_CHECK_HEADERS(sys/types.h stdint.h)

   # darwin requires a poll emulation library
   AC_CHECK_LIB(poll, poll, LIB_POLL="-lpoll")

   # CoreAudio framework
   AC_CHECK_HEADER(CoreAudio/CoreAudio.h, [
     AC_DEFINE(HAVE_COREAUDIO, 1, [Define if you have the CoreAudio API])
     FRAMEWORK_COREAUDIO="-framework CoreAudio"
   ])

   AC_CHECK_RES_INIT
   AC_SUBST(LIB_POLL)
   AC_SUBST(FRAMEWORK_COREAUDIO)
   LIBSOCKET="$X_EXTRA_LIBS"
   AC_SUBST(LIBSOCKET)
   AC_SUBST(X_EXTRA_LIBS)
   AC_CHECK_LIB(ucb, killpg, [LIBUCB="-lucb"]) dnl for Solaris2.4
   AC_SUBST(LIBUCB)

   case $host in  dnl this *is* LynxOS specific
   *-*-lynxos* )
        AC_MSG_CHECKING([LynxOS header file wrappers])
        [CFLAGS="$CFLAGS -D__NO_INCLUDE_WARN__"]
        AC_MSG_RESULT(disabled)
        AC_CHECK_LIB(bsd, gethostbyname, [LIBSOCKET="-lbsd"]) dnl for LynxOS
         ;;
    esac

   KDE_CHECK_TYPES
   KDE_CHECK_LIBDL

AH_VERBATIM(_AIX_STRINGS_H_BZERO,
[
/*
 * AIX defines FD_SET in terms of bzero, but fails to include <strings.h>
 * that defines bzero.
 */

#if defined(_AIX)
#include <strings.h>
#endif
])

AC_CHECK_FUNCS([vsnprintf snprintf])

AH_VERBATIM(_TRU64,[
/*
 * On HP-UX, the declaration of vsnprintf() is needed every time !
 */

#if !defined(HAVE_VSNPRINTF) || defined(hpux)
#if __STDC__
#include <stdarg.h>
#include <stdlib.h>
#else
#include <varargs.h>
#endif
#ifdef __cplusplus
extern "C"
#endif
int vsnprintf(char *str, size_t n, char const *fmt, va_list ap);
#ifdef __cplusplus
extern "C"
#endif
int snprintf(char *str, size_t n, char const *fmt, ...);
#endif
])

])

dnl Check for the type of the third argument of getsockname
AC_DEFUN([AC_CHECK_SOCKLEN_T], [
  AC_MSG_CHECKING(for socklen_t)
  AC_CACHE_VAL(ac_cv_socklen_t, [
    AC_LANG_PUSH(C++)
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[

#include <sys/types.h>
#include <sys/socket.h>
      ]],[[socklen_t a=0;getsockname(0,(struct sockaddr*)0, &a); ]])],
      ac_cv_socklen_t=socklen_t,
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[

#include <sys/types.h>
#include <sys/socket.h>
        ]],[[int a=0;getsockname(0,(struct sockaddr*)0, &a); ]])],
        ac_cv_socklen_t=int,
        ac_cv_socklen_t=size_t
      )
    )
    AC_LANG_POP([C++])
  ])

  AC_MSG_RESULT($ac_cv_socklen_t)
  if test "$ac_cv_socklen_t" != "socklen_t"; then
    AC_DEFINE_UNQUOTED(socklen_t, $ac_cv_socklen_t,
        [Define the real type of socklen_t])
  fi
  AC_DEFINE_UNQUOTED(ksize_t, socklen_t, [Compatibility define])

])

AC_DEFUN([AC_CHECK_RES_INIT],
[
  AC_MSG_CHECKING([if res_init needs -lresolv])
  kde_libs_safe="$LIBS"
  LIBS="$LIBS $X_EXTRA_LIBS -lresolv"
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
    ]],
    [[res_init(); ]])],
    [
      LIBRESOLV="-lresolv"
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_RES_INIT, 1, [Define if you have the res_init function])
    ],
    [ AC_MSG_RESULT(no) ]
  )
  LIBS="$kde_libs_safe"
  AC_SUBST(LIBRESOLV)

  AC_MSG_CHECKING([if res_init is available])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
    ]],
    [[
      res_init();
    ]])],
    [
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_RES_INIT, 1, [Define if you have the res_init function])
    ],
    [ AC_MSG_RESULT(no) ]
  )
])

dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_FIND_FILE],
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    echo "configure: __oline__: $i/$j" >&AS_MESSAGE_LOG_FD()
    if test -r "$i/$j"; then
      echo "taking that" >&AS_MESSAGE_LOG_FD()
      $3=$i
      break 2
    fi
  done
done
])

dnl KDE_FIND_PATH(programm-name, variable-name, list of directories,
dnl	if-not-found, test-parameter)
AC_DEFUN([KDE_FIND_PATH],
[
   AC_MSG_CHECKING([for $1])
   if test -n "$$2"; then
        kde_cv_path="$$2";
   else
        kde_cache=`echo $1 | sed 'y%./+-%__p_%'`

        AC_CACHE_VAL(kde_cv_path_$kde_cache,
        [
        kde_cv_path="NONE"
	dirs="$3"
	kde_save_IFS=$IFS
	IFS=':'
	for dir in $PATH; do
	  dirs="$dirs $dir"
        done

  dnl add qtdir/bin, because not everybody will have this in PATH
  dnl also, set it to be checked first. if the user has qtdir, then we
  dnl should try that binary first
  dirs="$QTDIR/bin $dirs"

	IFS=$kde_save_IFS

        for dir in $dirs; do
          if test -x "$dir/$1${EXEEXT}"; then
            if test -n "$5"; then
              evalstr="$dir/$1 $5 2>&1 "
              if eval $evalstr; then
                kde_cv_path="$dir/$1"
                break
              fi
            else
              kde_cv_path="$dir/$1"
              break
            fi
          fi
        done

        eval "kde_cv_path_$kde_cache=$kde_cv_path"

        ])

      eval "kde_cv_path=\"`echo '$kde_cv_path_'$kde_cache`\""

   fi

   if test -z "$kde_cv_path" || test "$kde_cv_path" = NONE; then
      AC_MSG_RESULT(not found)
      $4
   else
      AC_MSG_RESULT($kde_cv_path)
      $2=$kde_cv_path

   fi
])

AC_DEFUN([KDE_CHECK_TYPES],
[  AC_CHECK_SIZEOF(int, 4)dnl
  AC_CHECK_SIZEOF(long, 4)dnl
  AC_CHECK_SIZEOF(char *, 4)dnl
  AC_CHECK_SIZEOF(char, 1)dnl
])dnl

AC_DEFUN([KDE_CHECK_LIBDL],
[
AC_CHECK_LIB(dl, dlopen, [
LIBDL="-ldl"
ac_cv_have_dlfcn=yes
])

AC_CHECK_LIB(dld, shl_unload, [
LIBDL="-ldld"
ac_cv_have_shload=yes
])

AC_SUBST(LIBDL)
])

dnl AC_REMOVE_FORBIDDEN removes forbidden arguments from variables
dnl use: AC_REMOVE_FORBIDDEN(CC, [-forbid -bad-option whatever])
dnl it's all white-space separated
AC_DEFUN([AC_REMOVE_FORBIDDEN],
[ __val=$$1
  __forbid=" $2 "
  if test -n "$__val"; then
    __new=""
    ac_save_IFS=$IFS
    IFS=" 	"
    for i in $__val; do
      case "$__forbid" in
        *" $i "*) AC_MSG_WARN([found forbidden $i in $1, removing it]) ;;
	*) # Careful to not add spaces, where there were none, because otherwise
	   # libtool gets confused, if we change e.g. CXX
	   if test -z "$__new" ; then __new=$i ; else __new="$__new $i" ; fi ;;
      esac
    done
    IFS=$ac_save_IFS
    $1=$__new
  fi
])

dnl AC_VALIDIFY_CXXFLAGS checks for forbidden flags the user may have given
AC_DEFUN([AC_VALIDIFY_CXXFLAGS],
[dnl
if test "x$kde_use_qt_emb" != "xyes"; then
 AC_REMOVE_FORBIDDEN(CXX, [-fno-rtti -rpath])
 AC_REMOVE_FORBIDDEN(CXXFLAGS, [-fno-rtti -rpath])
else
 AC_REMOVE_FORBIDDEN(CXX, [-rpath])
 AC_REMOVE_FORBIDDEN(CXXFLAGS, [-rpath])
fi
])
