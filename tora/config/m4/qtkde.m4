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


dnl ------------------------------------------------------------------------
dnl mrj
dnl check for kde. if not, skip the rest of the kde tests.
AC_DEFUN([TORA_HAVE_KDE],
[
  AC_MSG_CHECKING([have kde])
  have_kde=yes
  AC_ARG_WITH(kde,
  [  --with-kde              compile against kde (default yes)],
  [
     have_kde=$withval
  ],)

  AC_MSG_RESULT($have_kde)
  if test $have_kde != yes; then
    kde_qtver=0
  else
    AC_PATH_KDE
    AC_DEFINE(TO_KDE, 1, [Define if you have KDE.])
  fi

  AC_PATH_QT

  KDE_CREATE_LIBS_ALIASES
  KDE_CHECK_STL
])


dnl mrj
dnl check for qscintilla, fail if no
AC_DEFUN([TORA_CHECK_QSCINTILLA],
[
  AC_REQUIRE([TORA_HAVE_KDE])
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
  [
     --with-qscintilla-libraries
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
  LIBS="-lqscintilla"

  AC_LANG_PUSH(C++)

  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <qextscintilla.h>
#include <qextscintillalexersql.h>
    ]],
    [[QextScintillaLexerSQL sqlLexer(0);]])],
    scin_works=yes,
    scin_works=no)

  AC_LANG_POP([C++])

  if test $scin_works = no; then
    AC_MSG_ERROR([Couldn't compile a simple QScintilla application. See config.log or specify its location with --with-qscintilla-includes])
  fi

  CXXFLAGS=$cflags_scin_save
  LDFLAGS=$ldflags_scin_save
  LIBS=$libs_scin_save

  AC_SUBST(QSCINTILLA_CXXFLAGS, $scin_cflags)
  AC_SUBST(QSCINTILLA_LDFLAGS, $scin_ldflags)
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
#include <qapplication.h>

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
  dnl AC_REQUIRE([K_PATH_X]) don't use until later, after qt native check
  AC_REQUIRE([AC_PATH_XTRA])

  dnl ------------------------------------------------------------------------
  dnl Add configure flag to enable linking to MT version of Qt library.
  dnl ------------------------------------------------------------------------

  AC_ARG_ENABLE(
    mt,
    [  --disable-mt            link to non-threaded Qt (deprecated)],
    kde_use_qt_mt=$enableval,
    [
      if test $kde_qtver = 3 || test $kde_qtver = 0; then
        kde_use_qt_mt=yes
      else
        kde_use_qt_mt=no
      fi
    ]
  )
  
  USING_QT_MT=""
  
  dnl ------------------------------------------------------------------------
  dnl If we not get --disable-qt-mt then adjust some vars for the host.
  dnl ------------------------------------------------------------------------
  
  KDE_MT_LDFLAGS=
  KDE_MT_LIBS=
  if test "x$kde_use_qt_mt" = "xyes"; then
    KDE_CHECK_THREADING
    if test "x$kde_use_threading" = "xyes"; then
      CXXFLAGS="$USE_THREADS -DQT_THREAD_SUPPORT $CXXFLAGS"
      KDE_MT_LDFLAGS="$USE_THREADS"
      KDE_MT_LIBS="$LIBPTHREAD"
    else
      kde_use_qt_mt=no
    fi
  fi
  AC_SUBST(KDE_MT_LDFLAGS)
  AC_SUBST(KDE_MT_LIBS)
  
  kde_qt_was_given=yes
  
  dnl ------------------------------------------------------------------------
  dnl If we haven't been told how to link to Qt, we work it out for ourselves.
  dnl ------------------------------------------------------------------------
  if test -z "$LIBQT_GLOB"; then
    if test "x$kde_use_qt_emb" = "xyes"; then
      LIBQT_GLOB="libqte.*"
    else
      LIBQT_GLOB="libqt.*"
    fi
  fi
  
  if test -z "$LIBQT"; then
  dnl ------------------------------------------------------------
  dnl If we got --enable-embedded then adjust the Qt library name.
  dnl ------------------------------------------------------------
    if test "x$kde_use_qt_emb" = "xyes"; then
      qtlib="qte"
    else
      qtlib="qt"
    fi
  
    kde_int_qt="-l$qtlib"
  else
    kde_int_qt="$LIBQT"
    kde_lib_qt_set=yes
  fi
  
  if test -z "$LIBQPE"; then
  dnl ------------------------------------------------------------
  dnl If we got --enable-palmtop then add -lqpe to the link line
  dnl ------------------------------------------------------------
    if test "x$kde_use_qt_emb" = "xyes"; then
      if test "x$kde_use_qt_emb_palm" = "xyes"; then
        LIB_QPE="-lqpe"
      else
        LIB_QPE=""
      fi
    else
      LIB_QPE=""
    fi
  fi
  
  dnl ------------------------------------------------------------------------
  dnl If we got --enable-qt-mt then adjust the Qt library name for the host.
  dnl ------------------------------------------------------------------------
  
  if test "x$kde_use_qt_mt" = "xyes"; then
    if test -z "$LIBQT"; then
      LIBQT="-l$qtlib-mt"
      kde_int_qt="-l$qtlib-mt"
    else
      LIBQT="-l$qtlib-mt"
      kde_int_qt="$qtlib-mt"
    fi
    LIBQT_GLOB="lib$qtlib-mt.*"
    USING_QT_MT="using -mt"
  else
    LIBQT="-l$qtlib"
  fi
  
  if test $kde_qtver != 1; then
    AC_FIND_PNG
    AC_FIND_JPEG
    LIBQT="$LIBQT $LIBPNG $LIBJPEG"
  fi
  
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

  kde_qt_libs_given=no
  
  AC_ARG_WITH(qt-libraries,
      [  --with-qt-libraries=DIR where the Qt library is installed.],
      [  ac_qt_libraries="$withval"
         kde_qt_libs_given=yes
      ])
  
  AC_CACHE_VAL(ac_cv_have_qt,
  [#try to guess Qt locations
  
  qt_incdirs=""
  for dir in $kde_qt_dirs; do
     qt_incdirs="$qt_incdirs $dir/include $dir"
  done
  qt_incdirs="$QTINC
              $qt_incdirs
              /usr/local/qt/include
              /usr/include/qt
              /usr/include/qt2
              /usr/include/qt3
              /usr/include
              /usr/X11R6/include/X11/qt
              /usr/X11R6/include/qt
              /usr/X11R6/include/qt2
              /usr/X11R6/include/qt3
              /Developer/qt/include
              /sw/include/qt
              /usr/qt/3/include
              $x_includes"
  if test "$ac_qt_includes" != "NO"; then
     qt_incdirs="$ac_qt_includes $qt_incdirs"
  fi
  
  if test $kde_qtver != 1 && test $kde_qtver != 0; then
    kde_qt_header=qstyle.h
  else
    kde_qt_header=qglobal.h
  fi
  
  AC_FIND_FILE($kde_qt_header, $qt_incdirs, qt_incdir)
  ac_qt_includes="$qt_incdir"
  
  qt_libdirs="$QTLIB
              $qt_libdirs
              /Developer/qt/lib
              /sw/lib
              /usr/X11R6/lib
              /usr/lib
              /usr/local/qt/lib
              /usr/qt/3/lib
              $x_libraries"
  if test "$ac_qt_libraries" != "NO"; then
    qt_libdir=$ac_qt_libraries
  else
    qt_libdirs="$ac_qt_libraries $qt_libdirs"
    # if the Qt was given, the chance is too big that libqt.* doesn't exist
    qt_libdir=NONE
    for dir in $qt_libdirs; do
      try="ls -1 $dir/${LIBQT_GLOB}"
      if test -n "`$try 2> /dev/null`"; then
        qt_libdir=$dir; break
      else
        echo "tried $dir" >&AS_MESSAGE_LOG_FD()
      fi
    done
  fi
  
  ac_qt_libraries="$qt_libdir"
  
  AC_LANG_PUSH(C++)
  
  dnl before we try to compile, check for native qt on the mac.
  dnl it requires additional flags.
  dnl this check must be run after the above tests to find the qt dir,
  dnl but before trying to compile because we have no other way to know
  dnl if we're compiling against qt/x11 or qt/mac. if we need to add
  dnl more platforms with custom libraries in this manner, i'll probably
  dnl separate the qt tests into testing linking/compiling and finding QTDIR.
  TORA_CHECK_MAC_NATIVE

  case $host in
  *-*-cygwin*)
      dnl no x for windows
      ;;
  *)
    if test $have_qt_mac_native != yes && test "x$kde_use_qt_emb" != "xyes"; then
      dnl don't append this until we're sure we're not using a native qt.
      K_PATH_X
      LIBQT="$LIBQT $X_PRE_LIBS -lXext -lX11 $LIBSM $LIBSOCKET"
    fi
    ;;
  esac
 
  ac_cxxflags_safe="$CXXFLAGS"
  ac_ldflags_safe="$LDFLAGS"
  ac_libs_safe="$LIBS"
  
  CXXFLAGS="$CXXFLAGS -I$qt_incdir $all_includes $X_CFLAGS"
  LDFLAGS="$LDFLAGS -L$qt_libdir $all_libraries $USER_LDFLAGS $KDE_MT_LDFLAGS"
  LIBS="$LIBS $LIBQT $KDE_MT_LIBS $X_LIBS"

  KDE_PRINT_QT_PROGRAM
  
  if AC_TRY_EVAL(ac_link) && test -s conftest; then
    rm -f conftest*
  else
    echo "configure: failed program was:" >&AS_MESSAGE_LOG_FD()
    cat conftest.$ac_ext >&AS_MESSAGE_LOG_FD()
    ac_qt_libraries="NO"
  fi
  rm -f conftest*
  CXXFLAGS="$ac_cxxflags_safe"
  LDFLAGS="$ac_ldflags_safe"
  LIBS="$ac_libs_safe"
  
  AC_LANG_POP([C++])
  if test "$ac_qt_includes" = NO || test "$ac_qt_libraries" = NO; then
    ac_cv_have_qt="have_qt=no"
    ac_qt_notfound=""
    missing_qt_mt=""
    if test "$ac_qt_includes" = NO; then
      if test "$ac_qt_libraries" = NO; then
        ac_qt_notfound="(headers and libraries)";
      else
        ac_qt_notfound="(headers)";
      fi
    else
      if test "x$kde_use_qt_mt" = "xyes"; then
         missing_qt_mt="Make sure that you have compiled Qt with thread support!"
         ac_qt_notfound="(library $qtlib-mt)";
      else
         ac_qt_notfound="(library $qtlib)";
      fi
    fi
  
    AC_MSG_ERROR([Qt ($kde_qt_minversion) $ac_qt_notfound not found.
      Please check your installation! For more details about this problem,
      look at the end of config.log.$missing_qt_mt])
  else
    have_qt="yes"
  fi
  ])
  
  eval "$ac_cv_have_qt"
  
  if test "$have_qt" != yes; then
    AC_MSG_RESULT([$have_qt]);
  else
    ac_cv_have_qt="have_qt=yes \
      ac_qt_includes=$ac_qt_includes ac_qt_libraries=$ac_qt_libraries"
    qt_libraries="$ac_qt_libraries"
    qt_includes="$ac_qt_includes"
  
    AC_SUBST(qt_libraries)
    AC_SUBST(qt_includes)
  
    AC_MSG_RESULT([      libqt:     $LIBQT,
      libraries: $ac_qt_libraries,
      headers:   $ac_qt_includes $USING_QT_MT])
  fi
  
  if test "$qt_includes" = "$x_includes" || test -z "$qt_includes"; then
   QT_INCLUDES=""
  else
   QT_INCLUDES="-I$qt_includes"
   all_includes="$QT_INCLUDES $all_includes"
  fi
  
  if test "$qt_libraries" = "$x_libraries" || test -z "$qt_libraries"; then
   QT_LDFLAGS=""
  else
   QT_LDFLAGS="-L$qt_libraries $LIBQT"
   all_libraries="$all_libraries $QT_LDFLAGS"
  fi
  test -z "$KDE_MT_LDFLAGS" || all_libraries="$all_libraries $KDE_MT_LDFLAGS"
  
  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_LDFLAGS)
  AC_PATH_QT_MOC_UIC
  
  dnl KDE_CHECK_QT_JPEG
  
  if test $have_qt_mac_native != yes; then
    if test "x$kde_use_qt_emb" != "xyes"; then
      LIB_QT="$kde_int_qt $LIBJPEG_QT "'$(LIBPNG) -lXext $(LIB_X11) $(LIBSM)'
    else
      LIB_QT="$kde_int_qt $LIBJPEG_QT "'$(LIBPNG)'
    fi
    test -z "$KDE_MT_LIBS" || LIB_QT="$LIB_QT $KDE_MT_LIBS"
  fi
  
  AC_SUBST(LIB_QT)
  AC_SUBST(LIB_QPE)
  
  AC_SUBST(kde_qtver)
])



AC_DEFUN([KDE_USE_QT],
[
  if test -z "$1"; then
    # Current default Qt version: 3.1
    kde_qtver=3
    kde_qtsubver=1
  else
    kde_qtsubver=`echo "$1" | sed -e 's#[0-9][0-9]*\.\([0-9][0-9]*\).*#\1#'`
    # following is the check if subversion isnt found in passed argument
    if test "$kde_qtsubver" = "$1"; then
      kde_qtsubver=1
    fi
    kde_qtver=`echo "$1" | sed -e 's#^\([0-9][0-9]*\)\..*#\1#'`
    if test "$kde_qtver" = "1"; then
      kde_qtsubver=42
    fi
  fi
  
  if test -z "$2"; then
    if test "$kde_qtver" = "2"; then
      if test $kde_qtsubver -gt 0; then
        kde_qt_minversion=">= Qt 2.2.2"
      else
        kde_qt_minversion=">= Qt 2.0.2"
      fi
    fi
    if test "$kde_qtver" = "3"; then
      if test $kde_qtsubver -gt 0; then
        kde_qt_minversion=">= Qt 3.1.0"
      else
        kde_qt_minversion=">= Qt 3.0"
      fi
    fi
    if test "$kde_qtver" = "1"; then
      kde_qt_minversion=">= 1.42 and < 2.0"
    fi
  else
     kde_qt_minversion=$2
  fi
  
  if test -z "$3"; then
     if test $kde_qtver = 3; then
       if test $kde_qtsubver -gt 0; then
         kde_qt_verstring="QT_VERSION >= 0x030100"
       else
         kde_qt_verstring="QT_VERSION >= 300"
       fi
     fi
     if test $kde_qtver = 2; then
       if test $kde_qtsubver -gt 0; then
         kde_qt_verstring="QT_VERSION >= 222"
       else
         kde_qt_verstring="QT_VERSION >= 200"
       fi
     fi
     if test $kde_qtver = 1; then
      kde_qt_verstring="QT_VERSION >= 142 && QT_VERSION < 200"
     fi
  else
     kde_qt_verstring=$3
  fi
  
  if test $kde_qtver = 3; then
    kde_qt_dirs="$QTDIR /usr/lib/qt3 /usr/lib/qt /usr/share/qt3"
  fi
  if test $kde_qtver = 2; then
     kde_qt_dirs="$QTDIR /usr/lib/qt2 /usr/lib/qt"
  fi
  if test $kde_qtver = 1; then
     kde_qt_dirs="$QTDIR /usr/lib/qt"
  fi
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
	IFS=$kde_save_IFS

        for dir in $dirs; do
	  if test -x "$dir/$1"; then
	    if test -n "$5"
	    then
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

   UIC_TR="i18n"
   if test $kde_qtver = 3; then
     UIC_TR="tr2i18n"
   fi

   AC_SUBST(UIC_TR)
])

AC_DEFUN([KDE_SET_PATHS],
[
  kde_cv_all_paths="kde_have_all_paths=\"yes\" \
	kde_htmldir=\"$kde_htmldir\" \
	kde_appsdir=\"$kde_appsdir\" \
	kde_icondir=\"$kde_icondir\" \
	kde_sounddir=\"$kde_sounddir\" \
	kde_datadir=\"$kde_datadir\" \
	kde_locale=\"$kde_locale\" \
	kde_cgidir=\"$kde_cgidir\" \
	kde_confdir=\"$kde_confdir\" \
	kde_mimedir=\"$kde_mimedir\" \
	kde_toolbardir=\"$kde_toolbardir\" \
	kde_wallpaperdir=\"$kde_wallpaperdir\" \
	kde_templatesdir=\"$kde_templatesdir\" \
	kde_bindir=\"$kde_bindir\" \
	kde_servicesdir=\"$kde_servicesdir\" \
	kde_servicetypesdir=\"$kde_servicetypesdir\" \
	kde_moduledir=\"$kde_moduledir\" \
   kde_styledir=\"$kde_styledir\" \
	kde_widgetdir=\"$kde_widgetdir\" \
	kde_result=$1"
])

AC_DEFUN([KDE_SET_DEFAULT_PATHS],
[
if test "$1" = "default"; then

  if test -z "$kde_htmldir"; then
    kde_htmldir='\${prefix}/share/doc/HTML'
  fi
  if test -z "$kde_appsdir"; then
    kde_appsdir='\${prefix}/share/applnk'
  fi
  if test -z "$kde_icondir"; then
    kde_icondir='\${prefix}/share/icons'
  fi
  if test -z "$kde_sounddir"; then
    kde_sounddir='\${prefix}/share/sounds'
  fi
  if test -z "$kde_datadir"; then
    kde_datadir='\${prefix}/share/apps'
  fi
  if test -z "$kde_locale"; then
    kde_locale='\${prefix}/share/locale'
  fi
  if test -z "$kde_cgidir"; then
    kde_cgidir='\${exec_prefix}/cgi-bin'
  fi
  if test -z "$kde_confdir"; then
    kde_confdir='\${prefix}/share/config'
  fi
  if test -z "$kde_mimedir"; then
    kde_mimedir='\${prefix}/share/mimelnk'
  fi
  if test -z "$kde_toolbardir"; then
    kde_toolbardir='\${prefix}/share/toolbar'
  fi
  if test -z "$kde_wallpaperdir"; then
    kde_wallpaperdir='\${prefix}/share/wallpapers'
  fi
  if test -z "$kde_templatesdir"; then
    kde_templatesdir='\${prefix}/share/templates'
  fi
  if test -z "$kde_bindir"; then
    kde_bindir='\${exec_prefix}/bin'
  fi
  if test -z "$kde_servicesdir"; then
    kde_servicesdir='\${prefix}/share/services'
  fi
  if test -z "$kde_servicetypesdir"; then
    kde_servicetypesdir='\${prefix}/share/servicetypes'
  fi
  if test -z "$kde_moduledir"; then
    if test "$kde_qtver" = "2"; then
      kde_moduledir='\${libdir}/kde2'
    else
      kde_moduledir='\${libdir}/kde3'
    fi
  fi
  if test -z "$kde_styledir"; then
    kde_styledir='\${libdir}/kde3/plugins/styles'
  fi
  if test -z "$kde_widgetdir"; then
    kde_widgetdir='\${libdir}/kde3/plugins/designer'
  fi

  KDE_SET_PATHS(defaults)

else

  if test $kde_qtver = 1; then
     AC_MSG_ERROR([KDE 1 is too old])
  else
     AC_MSG_ERROR([path checking not yet supported for KDE 2])
  fi

fi
])

AC_DEFUN([KDE_CHECK_PATHS_FOR_COMPLETENESS],
[ if test -z "$kde_htmldir" || test -z "$kde_appsdir" ||
   test -z "$kde_icondir" || test -z "$kde_sounddir" ||
   test -z "$kde_datadir" || test -z "$kde_locale"  ||
   test -z "$kde_cgidir"  || test -z "$kde_confdir" ||
   test -z "$kde_mimedir" || test -z "$kde_toolbardir" ||
   test -z "$kde_wallpaperdir" || test -z "$kde_templatesdir" ||
   test -z "$kde_bindir" || test -z "$kde_servicesdir" ||
   test -z "$kde_servicetypesdir" || test -z "$kde_moduledir" ||
   test -z "$kde_styledir" || test -z "kde_widgetdir" 
   test "x$kde_have_all_paths" != "xyes"; then
     kde_have_all_paths=no
  fi
])

AC_DEFUN([KDE_MISSING_PROG_ERROR],
[
    AC_MSG_ERROR([The important program $1 was not found!
Please check whether you installed KDE correctly.
])
])

AC_DEFUN([KDE_MISSING_ARTS_ERROR],
[
    AC_MSG_ERROR([The important program $1 was not found!
Please check whether you installed aRts correctly.
])
])

AC_DEFUN([KDE_SUBST_PROGRAMS],
[

        kde_default_bindirs="/usr/bin /usr/local/bin /opt/local/bin /usr/X11R6/bin /opt/kde/bin /opt/kde3/bin /usr/kde/bin /usr/local/kde/bin"
        test -n "$KDEDIR" && kde_default_bindirs="$KDEDIR/bin $kde_default_bindirs"
        if test -n "$KDEDIRS"; then
           kde_save_IFS=$IFS
           IFS=:
           for dir in $KDEDIRS; do
                kde_default_bindirs="$dir/bin $kde_default_bindirs "
           done
           IFS=$kde_save_IFS
        fi
        kde_default_bindirs="$exec_prefix/bin $prefix/bin $kde_default_bindirs"
        KDE_FIND_PATH(dcopidl, DCOPIDL, [$kde_default_bindirs], [KDE_MISSING_PROG_ERROR(dcopidl)])
        KDE_FIND_PATH(dcopidl2cpp, DCOPIDL2CPP, [$kde_default_bindirs], [KDE_MISSING_PROG_ERROR(dcopidl2cpp)])
        KDE_FIND_PATH(mcopidl, MCOPIDL, [$kde_default_bindirs], [KDE_MISSING_ARTS_ERROR(mcopidl)])
        KDE_FIND_PATH(artsc-config, ARTSCCONFIG, [$kde_default_bindirs], [KDE_MISSING_ARTS_ERROR(artsc-config)])
        KDE_FIND_PATH(kde-config, KDECONFIG, [$kde_default_bindirs])
        KDE_FIND_PATH(meinproc, MEINPROC, [$kde_default_bindirs])
      
        if test -n "$MEINPROC" && test ! "$MEINPROC" = "compiled"; then  
 	    kde_sharedirs="/usr/share/kde /usr/local/share /usr/share /opt/kde3/share /opt/kde/share $prefix/share"
            test -n "$KDEDIR" && kde_sharedirs="$KDEDIR/share $kde_sharedirs"
            AC_FIND_FILE(apps/ksgmltools2/customization/kde-chunk.xsl, $kde_sharedirs, KDE_XSL_STYLESHEET)
	    if test "$KDE_XSL_STYLESHEET" = "NO"; then
		KDE_XSL_STYLESHEET=""
	    else
                KDE_XSL_STYLESHEET="$KDE_XSL_STYLESHEET/apps/ksgmltools2/customization/kde-chunk.xsl"
	    fi
        fi

        DCOP_DEPENDENCIES='$(DCOPIDL)'
        AC_SUBST(DCOPIDL)
        AC_SUBST(DCOPIDL2CPP)
        AC_SUBST(DCOP_DEPENDENCIES)
        AC_SUBST(MCOPIDL)
        AC_SUBST(ARTSCCONFIG)
        AC_SUBST(KDECONFIG)
	AC_SUBST(MEINPROC)
 	AC_SUBST(KDE_XSL_STYLESHEET)

        if test -x "$KDECONFIG"; then # it can be "compiled"
          kde_libs_prefix=`$KDECONFIG --prefix`
          if test -z "$kde_libs_prefix" || test ! -x "$kde_libs_prefix"; then
               AC_MSG_ERROR([$KDECONFIG --prefix outputed the non existant prefix '$kde_libs_prefix' for kdelibs.
                          This means it has been moved since you installed it.
                          This won't work. Please recompile kdelibs for the new prefix.
                          ])
           fi
           kde_libs_htmldir=`$KDECONFIG --install html --expandvars`
        else
           kde_libs_prefix='$(prefix)'
           kde_libs_htmldir='$(kde_htmldir)'
        fi
        AC_SUBST(kde_libs_prefix)
        AC_SUBST(kde_libs_htmldir)
])dnl

AC_DEFUN([AC_CREATE_KFSSTND],
[

AC_MSG_CHECKING([for KDE paths])
kde_result=""
kde_cached_paths=yes
AC_CACHE_VAL(kde_cv_all_paths,
[
  KDE_SET_DEFAULT_PATHS($1)
  kde_cached_paths=no
])

eval "$ac_cv_have_kde"
# mrj, don't test if we don't have kde
if test $have_kde = "no"; then
  AC_MSG_RESULT(no)
else
  eval "$kde_cv_all_paths"
  KDE_CHECK_PATHS_FOR_COMPLETENESS
  if test "$kde_have_all_paths" = "no" && test "$kde_cached_paths" = "yes"; then
    # wrong values were cached, may be, we can set better ones
    kde_result=
    kde_htmldir= kde_appsdir= kde_icondir= kde_sounddir=
    kde_datadir= kde_locale=  kde_cgidir=  kde_confdir=
    kde_mimedir= kde_toolbardir= kde_wallpaperdir= kde_templatesdir=
    kde_bindir= kde_servicesdir= kde_servicetypesdir= kde_moduledir=
    kde_have_all_paths=
    kde_styledir=
    kde_widgetdir=
    KDE_SET_DEFAULT_PATHS($1)
    eval "$kde_cv_all_paths"
    KDE_CHECK_PATHS_FOR_COMPLETENESS
    kde_result="$kde_result (cache overridden)"
  fi

  if test "$kde_have_all_paths" = "no"; then
    AC_MSG_ERROR([configure could not run a little KDE program to test the environment.
Since it had compiled and linked before, it must be a strange problem on your system.
Look at config.log for details. If you are not able to fix this, look at
http://www.kde.org/faq/installation.html or any www.kde.org mirror.
(If you're using an egcs version on Linux, you may update binutils!)
])
  else
    rm -f conftest*
    AC_MSG_RESULT($kde_result)
  fi

  bindir=$kde_bindir

  KDE_SUBST_PROGRAMS
fi
])

AC_DEFUN([AC_SUBST_KFSSTND],
[
AC_SUBST(kde_htmldir)
AC_SUBST(kde_appsdir)
AC_SUBST(kde_icondir)
AC_SUBST(kde_sounddir)
AC_SUBST(kde_datadir)
AC_SUBST(kde_locale)
AC_SUBST(kde_confdir)
AC_SUBST(kde_mimedir)
AC_SUBST(kde_wallpaperdir)
AC_SUBST(kde_bindir)
dnl for KDE 2
AC_SUBST(kde_templatesdir)
AC_SUBST(kde_servicesdir)
AC_SUBST(kde_servicetypesdir)
AC_SUBST(kde_moduledir)
AC_SUBST(kde_styledir)
AC_SUBST(kde_widgetdir)
if test "$kde_qtver" = 1; then
  kde_minidir="$kde_icondir/mini"
else
# for KDE 1 - this breaks KDE2 apps using minidir, but
# that's the plan ;-/
  kde_minidir="/dev/null"
fi
dnl AC_SUBST(kde_minidir)
dnl AC_SUBST(kde_cgidir)
dnl AC_SUBST(kde_toolbardir)
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
AC_SUBST(QTE_NORTTI)
AC_SUBST(LIB_XEXT)


AC_LANG_POP([C])

])

AC_DEFUN([KDE_PRINT_QT_PROGRAM],
[
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include <qglobal.h>
#include <qapplication.h>
#include <qevent.h>
#include <qstring.h>
/* #include <qstyle.h> */

int main() {
  QString::fromLatin1("Elvis is alive");
  return 0;
}
EOF
])

AC_DEFUN([KDE_CHECK_UIC_PLUGINS],
[
AC_REQUIRE([AC_PATH_QT_MOC_UIC])

if test x$ac_uic_supports_libpath = xyes; then

AC_MSG_CHECKING([if UIC has KDE plugins available])
AC_CACHE_VAL(kde_cv_uic_plugins,
[
cat > actest.ui << EOF
<!DOCTYPE UI><UI version="3.0" stdsetdef="1">
<class>NewConnectionDialog</class>
<widget class="QDialog">
   <widget class="KLineEdit">
        <property name="name">
           <cstring>testInput</cstring>
        </property>
   </widget>
</widget>
</UI>
EOF
       


kde_cv_uic_plugins=no
kde_line="$UIC_PATH -L $kde_widgetdir"
if test x$ac_uic_supports_nounload = xyes; then
   kde_line="$kde_line -nounload"
fi
kde_line="$kde_line -impl actest.h actest.ui > actest.cpp"
if AC_TRY_EVAL(kde_line); then
	if test -f actest.cpp && grep klineedit actest.cpp > /dev/null; then
		kde_cv_uic_plugins=yes
	fi
fi
rm -f actest.ui actest.cpp
])

if test "$kde_cv_uic_plugins" = yes; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_ERROR([not found - you need to install kdelibs first.])
fi
fi
])

AC_DEFUN([KDE_CHECK_FINAL],
[
  AC_ARG_ENABLE(final, [  --enable-final          build size optimized apps (experimental - needs lots of memory)],
	kde_use_final=$enableval, kde_use_final=no)

  KDE_COMPILER_REPO
  if test "x$kde_use_final" = "xyes"; then
      KDE_USE_FINAL_TRUE=""
      KDE_USE_FINAL_FALSE="#"
   else
      KDE_USE_FINAL_TRUE="#"
      KDE_USE_FINAL_FALSE=""
  fi
  AC_SUBST(KDE_USE_FINAL_TRUE)
  AC_SUBST(KDE_USE_FINAL_FALSE)

  AC_ARG_ENABLE(closure, [  --disable-closure       don't delay template instantiation],
  	kde_use_closure=$enableval, kde_use_closure=yes)

  if test "x$kde_use_closure" = "xyes"; then
       KDE_USE_CLOSURE_TRUE=""
       KDE_USE_CLOSURE_FALSE="#"
#       CXXFLAGS="$CXXFLAGS $REPO"
  else
       KDE_USE_CLOSURE_TRUE="#"
       KDE_USE_CLOSURE_FALSE=""
  fi
  AC_SUBST(KDE_USE_CLOSURE_TRUE)
  AC_SUBST(KDE_USE_CLOSURE_FALSE)
])


dnl ------------------------------------------------------------------------
dnl Now, the same with KDE
dnl $(KDE_LDFLAGS) will be the kdeliblocation (if needed)
dnl and $(kde_includes) will be the kdehdrlocation (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_BASE_PATH_KDE],
[
KDE_USE_QT
KDE_CHECK_LIB64

AC_MSG_CHECKING([for KDE])

if test "${prefix}" != NONE; then
  kde_includes=${prefix}/include
  ac_kde_includes=$prefix/include

  if test "${exec_prefix}" != NONE; then
     kde_libraries=${libdir}
     ac_kde_libraries=$libdir
     if test "$ac_kde_libraries" = '${exec_prefix}/lib'${kdelibsuff}; then
	ac_kde_libraries=$exec_prefix/lib${kdelibsuff}
     fi
  else
     kde_libraries=${prefix}/lib${kdelibsuff}
     ac_kde_libraries=$prefix/lib${kdelibsuff}
  fi
else
  ac_kde_includes=
  ac_kde_libraries=
  kde_libraries=""
  kde_includes=""
fi

AC_CACHE_VAL(ac_cv_have_kde,
[#try to guess kde locations

if test "$kde_qtver" = 1; then
  kde_check_header="ksock.h"
  kde_check_lib="libkdecore.la"
else
  kde_check_header="ksharedptr.h"
  kde_check_lib="libkio.la"
fi

if test -z "$1"; then

kde_incdirs="/usr/lib/kde/include /usr/local/kde/include /usr/local/include /usr/kde/include /usr/include/kde /usr/include /opt/kde3/include /opt/kde/include $x_includes $qt_includes"
test -n "$KDEDIR" && kde_incdirs="$KDEDIR/include $KDEDIR/include/kde $KDEDIR $kde_incdirs"
kde_incdirs="$ac_kde_includes $kde_incdirs"
AC_FIND_FILE($kde_check_header, $kde_incdirs, kde_incdir)
ac_kde_includes="$kde_incdir"

if test -n "$ac_kde_includes" && test ! -r "$ac_kde_includes/$kde_check_header"; then
 AC_MSG_ERROR([
In the prefix you've chosen there are no KDE headers installed. This will fail.
Check this please and use another prefix (or use --without-kde)!])
fi

kde_libdirs="/usr/lib/kde/lib${kdelibsuff} /usr/local/kde/lib${kdelibsuff} /usr/kde/lib${kdelibsuff} /usr/lib${kdelibsuff}/kde /usr/lib${kdelibsuff}/kde3 /usr/lib${kdelibsuff} /usr/X11R6/lib${kdelibsuff} /usr/local/lib${kdelibsuff} /opt/kde3/lib${kdelibsuff} /opt/kde/lib${kdelibsuff} /usr/X11R6/kde/lib${kdelibsuff}"
test -n "$KDEDIR" && kde_libdirs="$KDEDIR/lib${kdelibsuff} $KDEDIR $kde_libdirs"
kde_libdirs="$ac_kde_libraries $libdir $kde_libdirs"
AC_FIND_FILE($kde_check_lib, $kde_libdirs, kde_libdir)
ac_kde_libraries="$kde_libdir"

kde_widgetdir=NO
dnl this might be somewhere else
AC_FIND_FILE("kde3/plugins/designer/kdewidgets.la", $kde_libdirs, kde_widgetdir)

if test -n "$ac_kde_libraries" && test ! -r "$ac_kde_libraries/$kde_check_lib"; then
AC_MSG_ERROR([
in the prefix, you've chosen, are no KDE libraries installed. This will fail.
So, check this please and use another prefix!])
fi

if test -n "$kde_widgetdir" && test ! -r "$kde_widgetdir/kde3/plugins/designer/kdewidgets.la"; then
AC_MSG_ERROR([
I can't find the designer plugins. These are required and should have been installed
by kdelibs])
fi

if test -n "$kde_widgetdir"; then
    kde_widgetdir="$kde_widgetdir/kde3/plugins/designer"
fi


if test "$ac_kde_includes" = NO || test "$ac_kde_libraries" = NO || test "$kde_widgetdir" = NO; then
  ac_cv_have_kde="have_kde=no"
else
  ac_cv_have_kde="have_kde=yes \
    ac_kde_includes=$ac_kde_includes ac_kde_libraries=$ac_kde_libraries"
fi

else dnl test -z $1

  ac_cv_have_kde="have_kde=no"

fi
])dnl

eval "$ac_cv_have_kde"

if test $have_kde = "yes"; then
  AC_DEFINE(TO_KDE, 1, [Define if you have KDE.])
fi

if test "$have_kde" != "yes"; then
 if test "${prefix}" = NONE; then
  ac_kde_prefix="$ac_default_prefix"
 else
  ac_kde_prefix="$prefix"
 fi
 if test "$exec_prefix" = NONE; then
  ac_kde_exec_prefix="$ac_kde_prefix"
  AC_MSG_RESULT([will be installed in $ac_kde_prefix])
 else
  ac_kde_exec_prefix="$exec_prefix"
  AC_MSG_RESULT([will be installed in $ac_kde_prefix and $ac_kde_exec_prefix])
 fi

 kde_libraries="${libdir}"
 kde_includes=${ac_kde_prefix}/include

else
  ac_cv_have_kde="have_kde=yes \
    ac_kde_includes=$ac_kde_includes ac_kde_libraries=$ac_kde_libraries"
  AC_MSG_RESULT([libraries $ac_kde_libraries, headers $ac_kde_includes])

  kde_libraries="$ac_kde_libraries"
  kde_includes="$ac_kde_includes"
fi
AC_SUBST(kde_libraries)
AC_SUBST(kde_includes)

if test "$kde_includes" = "$x_includes" || test "$kde_includes" = "$qt_includes"  || test "$kde_includes" = "/usr/include"; then
 KDE_INCLUDES=""
else
 KDE_INCLUDES="-I$kde_includes"
 all_includes="$KDE_INCLUDES $all_includes"
fi
 
KDE_LDFLAGS="-L$kde_libraries"
if test ! "$kde_libraries" = "$x_libraries" && test ! "$kde_libraries" = "$qt_libraries" ; then 
 all_libraries="$all_libraries $KDE_LDFLAGS"
fi

AC_SUBST(KDE_LDFLAGS)
AC_SUBST(KDE_INCLUDES)

AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])

all_libraries="$all_libraries $USER_LDFLAGS"
all_includes="$all_includes $USER_INCLUDES"
AC_SUBST(all_includes)
AC_SUBST(all_libraries)

dnl will re-run test in some cases
dnl if test -z "$1"; then
dnl KDE_CHECK_UIC_PLUGINS
dnl fi

ac_kde_libraries="$kde_libdir"

AC_SUBST(AUTODIRS)


])

AC_DEFUN([KDE_CHECK_EXTRA_LIBS],
[
AC_MSG_CHECKING(for extra includes)
AC_ARG_WITH(extra-includes, [  --with-extra-includes=DIR
                          adds non standard include paths],
  kde_use_extra_includes="$withval",
  kde_use_extra_includes=NONE
)
kde_extra_includes=
if test -n "$kde_use_extra_includes" && \
   test "$kde_use_extra_includes" != "NONE"; then

   ac_save_ifs=$IFS
   IFS=':'
   for dir in $kde_use_extra_includes; do
     kde_extra_includes="$kde_extra_includes $dir"
     USER_INCLUDES="$USER_INCLUDES -I$dir"
   done
   IFS=$ac_save_ifs
   kde_use_extra_includes="added"
else
   kde_use_extra_includes="no"
fi
AC_SUBST(USER_INCLUDES)

AC_MSG_RESULT($kde_use_extra_includes)

kde_extra_libs=
AC_MSG_CHECKING(for extra libs)
AC_ARG_WITH(extra-libs, [  --with-extra-libs=DIR   adds non standard library paths],
  kde_use_extra_libs=$withval,
  kde_use_extra_libs=NONE
)
if test -n "$kde_use_extra_libs" && \
   test "$kde_use_extra_libs" != "NONE"; then

   ac_save_ifs=$IFS
   IFS=':'
   for dir in $kde_use_extra_libs; do
     kde_extra_libs="$kde_extra_libs $dir"
     KDE_EXTRA_RPATH="$KDE_EXTRA_RPATH -R $dir"
     USER_LDFLAGS="$USER_LDFLAGS -L$dir"
   done
   IFS=$ac_save_ifs
   kde_use_extra_libs="added"
else
   kde_use_extra_libs="no"
fi

AC_SUBST(USER_LDFLAGS)

AC_MSG_RESULT($kde_use_extra_libs)

])

AC_DEFUN([KDE_CHECK_KDEQTADDON],
[
AC_MSG_CHECKING(for kde-qt-addon)
AC_CACHE_VAL(kde_cv_have_kdeqtaddon,
[
 kde_ldflags_safe="$LDFLAGS"
 kde_libs_safe="$LIBS"
 kde_cxxflags_safe="$CXXFLAGS"

 LIBS="-lkde-qt-addon $LIBQT $LIBS"
 CXXFLAGS="$CXXFLAGS -I$prefix/include -I$prefix/include/kde $all_includes"
 LDFLAGS="$LDFLAGS $all_libraries $USER_LDFLAGS"

	AC_LINK_IFELSE([AC_LANG_PROGRAM([[
   #include <qdom.h>
 ]],
 [[
   QDomDocument doc;
 ]])],
  kde_cv_have_kdeqtaddon=yes,
  kde_cv_have_kdeqtaddon=no
 )

 LDFLAGS=$kde_ldflags_safe
 LIBS=$kde_libs_safe
 CXXFLAGS=$kde_cxxflags_safe
])

AC_MSG_RESULT($kde_cv_have_kdeqtaddon)

if test "$kde_cv_have_kdeqtaddon" = "no"; then
  AC_MSG_ERROR([Can't find libkde-qt-addon. You need to install it first.
It is a separate package (and CVS module) named kde-qt-addon.])
fi
])

AC_DEFUN([KDE_CHECK_KIMGIO],
[
   AC_BASE_PATH_KDE
   KDE_CHECK_EXTRA_LIBS
   AC_FIND_TIFF
   AC_FIND_JPEG
   AC_FIND_PNG
   KDE_CREATE_LIBS_ALIASES

   if test "$1" = "existance"; then
     AC_LANG_PUSH(C++)
     kde_save_LIBS="$LIBS"
     LIBS="$LIBS $all_libraries $LIBJPEG $LIBTIFF $LIBPNG $LIBQT -lm"
     AC_CHECK_LIB(kimgio, kimgioRegister, [
      LIBKIMGIO_EXISTS=yes],LIBKIMGIO_EXISTS=no)
     LIBS="$kde_save_LIBS"
     AC_LANG_POP([C++])
   else
     LIBKIMGIO_EXISTS=yes
   fi

   if test "$LIBKIMGIO_EXISTS" = "yes"; then
     LIB_KIMGIO='-lkimgio'
   else
     LIB_KIMGIO=''
   fi
   AC_SUBST(LIB_KIMGIO)
])

AC_DEFUN([KDE_CREATE_LIBS_ALIASES],
[
if test $kde_qtver = 3; then
   AC_SUBST(LIB_KDECORE, "-lkdecore")
   AC_SUBST(LIB_KDEUI, "-lkdeui")
   AC_SUBST(LIB_KIO, "-lkio")
   AC_SUBST(LIB_SMB, "-lsmb")
   AC_SUBST(LIB_KAB, "-lkab")
   AC_SUBST(LIB_KABC, "-lkabc")
   AC_SUBST(LIB_KHTML, "-lkhtml")
   AC_SUBST(LIB_KSPELL, "-lkspell")
   AC_SUBST(LIB_KPARTS, "-lkparts")
   AC_SUBST(LIB_KDEPRINT, "-lkdeprint")
   AC_SUBST(LIB_DCOP, "-lDCOP")
# these are for backward compatibility
   AC_SUBST(LIB_KSYCOCA, "-lkio")
   AC_SUBST(LIB_KFILE, "-lkio")
elif test $kde_qtver = 2; then
   AC_SUBST(LIB_KDECORE, "-lkdecore")
   AC_SUBST(LIB_KDEUI, "-lkdeui")
   AC_SUBST(LIB_KIO, "-lkio")
   AC_SUBST(LIB_KSYCOCA, "-lksycoca")
   AC_SUBST(LIB_SMB, "-lsmb")
   AC_SUBST(LIB_KFILE, "-lkfile")
   AC_SUBST(LIB_KAB, "-lkab")
   AC_SUBST(LIB_KHTML, "-lkhtml")
   AC_SUBST(LIB_KSPELL, "-lkspell")
   AC_SUBST(LIB_KPARTS, "-lkparts")
   AC_SUBST(LIB_KDEPRINT, "-lkdeprint")
   AC_SUBST(LIB_DCOP, "-lDCOP")
else
   AC_SUBST(LIB_KDECORE, "")
   AC_SUBST(LIB_KDEUI, "")
   AC_SUBST(LIB_KIO, "")
   AC_SUBST(LIB_SMB, "")
   AC_SUBST(LIB_KAB, "")
   AC_SUBST(LIB_KABC, "")
   AC_SUBST(LIB_KHTML, "")
   AC_SUBST(LIB_KSPELL, "")
   AC_SUBST(LIB_KPARTS, "")
   AC_SUBST(LIB_KDEPRINT, "")
   AC_SUBST(LIB_DCOP, "")
fi
])

AC_DEFUN([AC_PATH_KDE],
[
  KDE_MISC_TESTS
  K_PATH_X
  AC_BASE_PATH_KDE
  AC_ARG_ENABLE(path-check, [  --disable-path-check    don't try to find out, where to install],
  [
  if test "$enableval" = "no";
    then ac_use_path_checking="default"
    else ac_use_path_checking=""
  fi
  ],
  [
  if test "$kde_qtver" = 1;
    then ac_use_path_checking=""
    else ac_use_path_checking="default"
  fi
  ]
  )

  AC_CREATE_KFSSTND($ac_use_path_checking)

  AC_SUBST_KFSSTND
])

dnl KDE_CHECK_FUNC_EXT(<func>, [headers], [sample-use], [C prototype], [autoheader define], [call if found])
AC_DEFUN([KDE_CHECK_FUNC_EXT],
[
AC_MSG_CHECKING(for $1)
AC_CACHE_VAL(kde_cv_func_$1,
[
AC_LANG_PUSH(C++)
save_CXXFLAGS="$CXXFLAGS"
kde_safe_LIBS="$LIBS"
LIBS="$LIBS $X_EXTRA_LIBS"
if test "$GXX" = "yes"; then
  CXXFLAGS="$CXXFLAGS -pedantic-errors"
fi
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
$2
]],
[[
$3
]])],
kde_cv_func_$1=yes,
kde_cv_func_$1=no)
CXXFLAGS="$save_CXXFLAGS"
LIBS=$kde_safe_LIBS
AC_LANG_POP([C++])
])

AC_MSG_RESULT($kde_cv_func_$1)

AC_MSG_CHECKING([if $1 needs custom prototype])
AC_CACHE_VAL(kde_cv_proto_$1,
[
if test "x$kde_cv_func_$1" = xyes; then
  kde_cv_proto_$1=no
else
  case "$1" in
	setenv|unsetenv|usleep|random|srandom|seteuid|mkstemps|mkstemp|revoke|vsnprintf|strlcpy|strlcat)
		kde_cv_proto_$1="yes - in libkdefakes"
		;;
	*)
		kde_cv_proto_$1=unknown
		;;
  esac
fi

if test "x$kde_cv_proto_$1" = xunknown; then

AC_LANG_PUSH(C++)
  kde_safe_libs=$LIBS
  LIBS="$LIBS $X_EXTRA_LIBS"
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[
  $2
  extern "C" $4;
]],
[[
$3
]])],
[ kde_cv_func_$1=yes
  kde_cv_proto_$1=yes ],
  [kde_cv_proto_$1="$1 unavailable"]
)
LIBS=$kde_safe_libs
AC_LANG_POP([C++])
fi
])
AC_MSG_RESULT($kde_cv_proto_$1)

if test "x$kde_cv_func_$1" = xyes; then
  AC_DEFINE(HAVE_$5, 1, [Define if you have $1])
  $6
fi
if test "x$kde_cv_proto_$1" = xno; then
  AC_DEFINE(HAVE_$5_PROTO, 1,
  [Define if you have the $1 prototype])
fi

AH_VERBATIM([_HAVE_$5_PROTO],
[
#if !defined(HAVE_$5_PROTO)
#ifdef __cplusplus
extern "C"
#endif
$4;
#endif
])
])

AC_DEFUN([AC_CHECK_SETENV],
[
	KDE_CHECK_FUNC_EXT(setenv, [
#include <stdlib.h>
], 
		[setenv("VAR", "VALUE", 1);],
	        [int setenv (const char *, const char *, int)],
		[SETENV])
])

AC_DEFUN([AC_CHECK_UNSETENV],
[
	KDE_CHECK_FUNC_EXT(unsetenv, [
#include <stdlib.h>
], 
		[unsetenv("VAR");],
	        [void unsetenv (const char *)],
		[UNSETENV])
])

AC_DEFUN([AC_CHECK_GETDOMAINNAME],
[
	KDE_CHECK_FUNC_EXT(getdomainname, [
#include <stdlib.h>
#include <unistd.h>
], 
		[
char buffer[200];
getdomainname(buffer, 200);
], 	
	        [int getdomainname (char *, unsigned int)],
		[GETDOMAINNAME])
])

AC_DEFUN([AC_CHECK_GETHOSTNAME],
[
	KDE_CHECK_FUNC_EXT(gethostname, [
#include <stdlib.h>
#include <unistd.h>
], 
		[
char buffer[200];
gethostname(buffer, 200);
], 	
	        [int gethostname (char *, unsigned int)],
		[GETHOSTNAME])
])

AC_DEFUN([AC_CHECK_USLEEP],
[
	KDE_CHECK_FUNC_EXT(usleep, [
#include <unistd.h>
], 
		[
usleep(200);
], 	
	        [int usleep (unsigned int)],
		[USLEEP])
])


AC_DEFUN([AC_CHECK_RANDOM],
[
	KDE_CHECK_FUNC_EXT(random, [
#include <stdlib.h>
], 
		[
random();
], 	
	        [long int random(void)],
		[RANDOM])

	KDE_CHECK_FUNC_EXT(srandom, [
#include <stdlib.h>
], 
		[
srandom(27);
], 	
	        [void srandom(unsigned int)],
		[SRANDOM])

])

AC_DEFUN([AC_CHECK_INITGROUPS],
[
	KDE_CHECK_FUNC_EXT(initgroups, [
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
],
	[
char buffer[200];
initgroups(buffer, 27);
],
	[int initgroups(const char *, gid_t)],
	[INITGROUPS])
])

AC_DEFUN([AC_CHECK_MKSTEMP],
[
	KDE_CHECK_FUNC_EXT(mkstemp, [
#include <stdlib.h>
],
	[
mkstemp("/tmp/aaaXXXXXX");
],
	[int mkstemp(char *)],
	[MKSTEMP])
])

AC_DEFUN([AC_CHECK_MKSTEMPS],
[
	KDE_CHECK_FUNC_EXT(mkstemps, [
#include <stdlib.h>
#include <unistd.h>
],
	[
mkstemps("/tmp/aaaXXXXXX", 6);
],
	[int mkstemps(char *, int)],
	[MKSTEMPS])
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

AC_DEFUN([AC_CHECK_STRLCPY],
[
	KDE_CHECK_FUNC_EXT(strlcpy, [
#include <string.h>
],
[ char buf[20];
  strlcpy(buf, "KDE function test", sizeof(buf));
],
 	[unsigned long strlcpy(char*, const char*, unsigned long)],
	[STRLCPY])
])

AC_DEFUN([AC_CHECK_STRLCAT],
[
	KDE_CHECK_FUNC_EXT(strlcat, [
#include <string.h>
],
[ char buf[20];
  buf[0]='\0';
  strlcat(buf, "KDE function test", sizeof(buf));
],
 	[unsigned long strlcat(char*, const char*, unsigned long)],
	[STRLCAT])
])

AC_DEFUN([AC_FIND_GIF],
   [AC_MSG_CHECKING([for giflib])
AC_CACHE_VAL(ac_cv_lib_gif,
[ac_save_LIBS="$LIBS"
if test "x$kde_use_qt_emb" != "xyes"; then
LIBS="$all_libraries -lgif -lX11 $LIBSOCKET"
else
LIBS="$all_libraries -lgif"
fi
AC_LINK_IFELSE([AC_LANG_PROGRAM([[			
#ifdef __cplusplus
extern "C" {
#endif
int GifLastError(void);
#ifdef __cplusplus
}
#endif
/* We use char because int might match the return type of a gcc2
    builtin and then its argument prototype would still apply.  */
]],
            [[return GifLastError();]])],
            eval "ac_cv_lib_gif=yes",
            eval "ac_cv_lib_gif=no")
LIBS="$ac_save_LIBS"
])dnl
if eval "test \"`echo $ac_cv_lib_gif`\" = yes"; then
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_LIBGIF, 1, [Define if you have libgif])
else
  AC_MSG_ERROR(You need giflib30. Please install the kdesupport package)
fi
])

AC_DEFUN([KDE_FIND_JPEG_HELPER],
[
AC_MSG_CHECKING([for libjpeg$2])
AC_CACHE_VAL(ac_cv_lib_jpeg_$1,
[
AC_LANG(C)
ac_save_LIBS="$LIBS"
LIBS="$all_libraries $USER_LDFLAGS -ljpeg$2 -lm"
ac_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $all_includes $USER_INCLUDES"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[			
/* Override any gcc2 internal prototype to avoid an error.  */
struct jpeg_decompress_struct;
typedef struct jpeg_decompress_struct * j_decompress_ptr;
typedef int size_t;
#ifdef __cplusplus
extern "C" {
#endif
    void jpeg_CreateDecompress(j_decompress_ptr cinfo,
                                    int version, size_t structsize);
#ifdef __cplusplus
}
#endif
/* We use char because int might match the return type of a gcc2
    builtin and then its argument prototype would still apply.  */
]],
            [[jpeg_CreateDecompress(0L, 0, 0);]])],
            eval "ac_cv_lib_jpeg_$1=-ljpeg$2",
            eval "ac_cv_lib_jpeg_$1=no")
LIBS="$ac_save_LIBS"
CFLAGS="$ac_save_CFLAGS"
])

if eval "test ! \"`echo $ac_cv_lib_jpeg_$1`\" = no"; then
  LIBJPEG="$ac_cv_lib_jpeg_$1"
  AC_MSG_RESULT($ac_cv_lib_jpeg_$1)
else
  AC_MSG_RESULT(no)
  $3
fi

])

AC_DEFUN([AC_FIND_JPEG],
[
dnl first look for libraries
KDE_FIND_JPEG_HELPER(6b, 6b,
   KDE_FIND_JPEG_HELPER(normal, [],
    [
       LIBJPEG=
    ]
   )
)

dnl then search the headers (can't use simply AC_TRY_xxx, as jpeglib.h
dnl requires system dependent includes loaded before it)
jpeg_incdirs="$includedir /usr/include /usr/local/include $kde_extra_includes"
AC_FIND_FILE(jpeglib.h, $jpeg_incdirs, jpeg_incdir)
test "x$jpeg_incdir" = xNO && jpeg_incdir=

dnl if headers _and_ libraries are missing, this is no error, and we
dnl continue with a warning (the user will get no jpeg support in khtml)
dnl if only one is missing, it means a configuration error, but we still
dnl only warn
if test -n "$jpeg_incdir" && test -n "$LIBJPEG" ; then
  AC_DEFINE_UNQUOTED(HAVE_LIBJPEG, 1, [Define if you have libjpeg])
else
  if test -n "$jpeg_incdir" || test -n "$LIBJPEG" ; then
    AC_MSG_WARN([
There is an installation error in jpeg support. You seem to have only one
of either the headers _or_ the libraries installed. You may need to either
provide correct --with-extra-... options, or the development package of
libjpeg6b. You can get a source package of libjpeg from http://www.ijg.org/
Disabling JPEG support.
])
  else
    AC_MSG_WARN([libjpeg not found. disable JPEG support.])
  fi
  jpeg_incdir=
  LIBJPEG=
fi

AC_SUBST(LIBJPEG)
AH_VERBATIM(_AC_CHECK_JPEG,
[/*
 * jpeg.h needs HAVE_BOOLEAN, when the system uses boolean in system
 * headers and I'm too lazy to write a configure test as long as only
 * unixware is related
 */
#ifdef _UNIXWARE
#define HAVE_BOOLEAN
#endif
])
])

AC_DEFUN([KDE_CHECK_QT_JPEG],
[
AC_MSG_CHECKING([if Qt needs $LIBJPEG])
AC_CACHE_VAL(kde_cv_qt_jpeg,
[
AC_LANG_PUSH(C++)
ac_save_LIBS="$LIBS"
LIBS="$all_libraries $USER_LDFLAGS $LIBQT"
LIBS=`echo $LIBS | sed "s/$LIBJPEG//"`
ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS $all_includes $USER_INCLUDES"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[			
#include <qapplication.h>
]],
            [[
            int argc;
            char** argv;
            QApplication app(argc, argv);]])],
            eval "kde_cv_qt_jpeg=no",
            eval "kde_cv_qt_jpeg=yes")
LIBS="$ac_save_LIBS"
CXXFLAGS="$ac_save_CXXFLAGS"
AC_LANG_POP([C++])
])

if eval "test ! \"`echo $kde_cv_qt_jpeg`\" = no"; then
  AC_MSG_RESULT(yes)
  LIBJPEG_QT='$(LIBJPEG)'
else
  AC_MSG_RESULT(no)
  LIBJPEG_QT=
fi

])

AC_DEFUN([AC_FIND_ZLIB],
[
AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])
AC_MSG_CHECKING([for libz])
AC_CACHE_VAL(ac_cv_lib_z,
[
AC_LANG([C])
kde_save_LIBS="$LIBS"
LIBS="$all_libraries $USER_LDFLAGS -lz $LIBSOCKET"
kde_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $all_includes $USER_INCLUDES"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[

#include<zlib.h>
]], [[return (zlibVersion() == ZLIB_VERSION); ]])],[eval "ac_cv_lib_z='-lz'"],[eval "ac_cv_lib_z=no"])
LIBS="$kde_save_LIBS"
CFLAGS="$kde_save_CFLAGS"
])
if test ! "$ac_cv_lib_z" = no; then
  AC_DEFINE_UNQUOTED(HAVE_LIBZ, 1, [Define if you have libz])
  LIBZ="$ac_cv_lib_z"
  AC_SUBST(LIBZ)
  AC_MSG_RESULT($ac_cv_lib_z)
else
  AC_MSG_ERROR(not found. Check your installation and look into config.log)
  LIBZ=""
  AC_SUBST(LIBZ)
fi
])

AC_DEFUN([KDE_TRY_TIFFLIB],
[
AC_MSG_CHECKING([for libtiff $1])

AC_CACHE_VAL(kde_cv_libtiff_$1,
[
AC_LANG_PUSH(C++)
kde_save_LIBS="$LIBS"
if test "x$kde_use_qt_emb" != "xyes"; then
LIBS="$all_libraries $USER_LDFLAGS -l$1 $LIBJPEG $LIBZ -lX11 $LIBSOCKET -lm"
else
LIBS="$all_libraries $USER_LDFLAGS -l$1 $LIBJPEG $LIBZ -lm"
fi
kde_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS $all_includes $USER_INCLUDES"

AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include<tiffio.h>
]],
    [[return (TIFFOpen( "", "r") == 0); ]])],
[
    kde_cv_libtiff_$1="-l$1 $LIBJPEG $LIBZ"
], [
    kde_cv_libtiff_$1=no
])

LIBS="$kde_save_LIBS"
CXXFLAGS="$kde_save_CXXFLAGS"
AC_LANG_POP([C++])
])

if test "$kde_cv_libtiff_$1" = "no"; then
    AC_MSG_RESULT(no)
    LIBTIFF=""
    $3
else
    LIBTIFF="$kde_cv_libtiff_$1"
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED(HAVE_LIBTIFF, 1, [Define if you have libtiff])
    $2
fi

])

AC_DEFUN([AC_FIND_TIFF],
[
AC_REQUIRE([K_PATH_X])
AC_REQUIRE([AC_FIND_ZLIB])
AC_REQUIRE([AC_FIND_JPEG])
AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])

KDE_TRY_TIFFLIB(tiff, [],
   KDE_TRY_TIFFLIB(tiff34))

AC_SUBST(LIBTIFF)
])


AC_DEFUN([AC_FIND_PNG],
[
AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])
AC_REQUIRE([AC_FIND_ZLIB])
AC_MSG_CHECKING([for libpng])
AC_CACHE_VAL(ac_cv_lib_png,
[
kde_save_LIBS="$LIBS"
if test "x$kde_use_qt_emb" != "xyes"; then
LIBS="$LIBS $all_libraries $USER_LDFLAGS -lpng $LIBZ -lm -lX11 $LIBSOCKET"
else
LIBS="$LIBS $all_libraries $USER_LDFLAGS -lpng $LIBZ -lm"
fi
kde_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $all_includes $USER_INCLUDES"
AC_LANG(C)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
    #include<png.h>
    ]],
    [[
    png_structp png_ptr = png_create_read_struct(  /* image ptr */
		PNG_LIBPNG_VER_STRING, 0, 0, 0 );
    return( png_ptr != 0 );
    ]])],
    eval "ac_cv_lib_png='-lpng $LIBZ -lm'",
    eval "ac_cv_lib_png=no"
)
LIBS="$kde_save_LIBS"
CFLAGS="$kde_save_CFLAGS"
])dnl
if eval "test ! \"`echo $ac_cv_lib_png`\" = no"; then
  AC_DEFINE_UNQUOTED(HAVE_LIBPNG, 1, [Define if you have libpng])
  LIBPNG="$ac_cv_lib_png"
  AC_SUBST(LIBPNG)
  AC_MSG_RESULT($ac_cv_lib_png)
else
  AC_MSG_RESULT(no)
  LIBPNG=""
  AC_SUBST(LIBPNG)
fi
])

AC_DEFUN([AC_CHECK_BOOL],
[
  AC_DEFINE_UNQUOTED(HAVE_BOOL, 1, [You _must_ have bool])
])

AC_DEFUN([AC_CHECK_GNU_EXTENSIONS],
[
AC_MSG_CHECKING(if you need GNU extensions)
AC_CACHE_VAL(ac_cv_gnu_extensions,
[
cat > conftest.c << EOF
#include <features.h>

#ifdef __GNU_LIBRARY__
yes
#endif
EOF

if (eval "$ac_cpp conftest.c") 2>&5 |
  egrep "yes" >/dev/null 2>&1; then
  rm -rf conftest*
  ac_cv_gnu_extensions=yes
else
  ac_cv_gnu_extensions=no
fi
])

AC_MSG_RESULT($ac_cv_gnu_extensions)
if test "$ac_cv_gnu_extensions" = "yes"; then
  AC_DEFINE_UNQUOTED(_GNU_SOURCE, 1, [Define if you need to use the GNU extensions])
fi
])

AC_DEFUN([KDE_CHECK_COMPILER_FLAG],
[
AC_MSG_CHECKING(whether $CXX supports -$1)
kde_cache=`echo $1 | sed 'y% .=/+-%____p_%'`
AC_CACHE_VAL(kde_cv_prog_cxx_$kde_cache,
[
  AC_LANG_PUSH(C++)
  save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS -$1"

 AC_LINK_IFELSE([AC_LANG_PROGRAM([[
]],[[ return 0; ]])], [eval "kde_cv_prog_cxx_$kde_cache=yes"], [])
  CXXFLAGS="$save_CXXFLAGS"
  AC_LANG_POP([C++])
])
if eval "test \"`echo '$kde_cv_prog_cxx_'$kde_cache`\" = yes"; then
 AC_MSG_RESULT(yes)
 :
 $2
else
 AC_MSG_RESULT(no)
 :
 $3
fi
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

AC_DEFUN([AC_CHECK_COMPILERS],
[
  AC_ARG_ENABLE(debug,[  --enable-debug[=ARG]    enables debug symbols (yes|no|full) [default=no]],
  [
    case $enableval in
      yes)
        kde_use_debug_code="yes"
        kde_use_debug_define=no
        ;;
      full)
        kde_use_debug_code="full"
        kde_use_debug_define=no
        ;;
      *)
        kde_use_debug_code="no"
        kde_use_debug_define=yes
        ;;
    esac
  ], 
    [kde_use_debug_code="no"
      kde_use_debug_define=no
  ])

  dnl Just for configure --help
  AC_ARG_ENABLE(dummyoption,[  --disable-debug         disables debug output and debug symbols [default=no]],[],[])

  AC_ARG_ENABLE(strict,[  --enable-strict         compiles with strict compiler options (may not work!)],
   [
    if test $enableval = "no"; then
         kde_use_strict_options="no"
       else
         kde_use_strict_options="yes"
    fi
   ], [kde_use_strict_options="no"])

  AC_ARG_ENABLE(warnings,[  --disable-warnings      disables compilation with -Wall and similiar],
   [
    if test $enableval = "no"; then
         kde_use_warnings="no"
       else
         kde_use_warnings="yes"
    fi
   ], [kde_use_warnings="yes"])

  dnl enable warnings for debug build
  if test "$kde_use_debug_code" != "no"; then
    kde_use_warnings=yes
  fi

  AC_ARG_ENABLE(profile,[  --enable-profile        creates profiling infos [default=no]],
    [kde_use_profiling=$enableval],
    [kde_use_profiling="no"]
  )

  AC_ARG_ENABLE(gcov,[  --enable-gcov           enables gcov test coverage support [default=no]],
    [kde_use_gcov=$enableval],
    [kde_use_gcov=no]
  )

  dnl this prevents stupid AC_PROG_CC to add "-g" to the default CFLAGS
  CFLAGS=" $CFLAGS"

  AC_PROG_CC 

  AC_PROG_CPP

  if test "$GCC" = "yes"; then
    if test "$kde_use_debug_code" != "no"; then
      if test $kde_use_debug_code = "full" || test $kde_use_gcov = "yes"; then
        CFLAGS="-g3 $CFLAGS"
      else
        CFLAGS="-g -O2 $CFLAGS"
      fi
    else
      CFLAGS="-O2 $CFLAGS"
    fi
  fi

  if test "$kde_use_debug_define" = "yes"; then
    CFLAGS="-DNDEBUG $CFLAGS"
  fi


  case "$host" in
  *-*-sysv4.2uw*) CFLAGS="-D_UNIXWARE $CFLAGS";;
  *-*-sysv5uw7*) CFLAGS="-D_UNIXWARE7 $CFLAGS";;
  esac

  if test -z "$LDFLAGS" && test "$kde_use_debug_code" = "no" && test "$GCC" = "yes"; then
     LDFLAGS=""
  fi

  CXXFLAGS=" $CXXFLAGS"

  AC_PROG_CXX

  if test "$GXX" = "yes" || test "$CXX" = "KCC"; then
    if test "$kde_use_debug_code" != "no"; then
      if test "$CXX" = "KCC"; then
        CXXFLAGS="+K0 -Wall -pedantic -W -Wpointer-arith -Wmissing-prototypes -Wwrite-strings $CXXFLAGS"
      else
        if test "$kde_use_debug_code" = "full"; then
          CXXFLAGS="-g3 $CXXFLAGS"
        else
          CXXFLAGS="-g -O2 $CXXFLAGS"
        fi
      fi
      KDE_CHECK_COMPILER_FLAG(fno-builtin,[CXXFLAGS="-fno-builtin $CXXFLAGS"])

      dnl convenience compiler flags
      KDE_CHECK_COMPILER_FLAG(Woverloaded-virtual, [WOVERLOADED_VIRTUAL="-Woverloaded-virtual"], [WOVERLOADED_VRITUAL=""])
      AC_SUBST(WOVERLOADED_VIRTUAL)
    else
      if test "$CXX" = "KCC"; then
        CXXFLAGS="+K3 $CXXFLAGS"
      else
        CXXFLAGS="-O2 $CXXFLAGS"
      fi  
    fi
  fi

  if test "$kde_use_debug_define" = "yes"; then
    CXXFLAGS="-DNDEBUG -DNO_DEBUG $CXXFLAGS"
  fi  

  if test "$kde_use_profiling" = "yes"; then
    KDE_CHECK_COMPILER_FLAG(pg,
    [
      CFLAGS="-pg $CFLAGS"
      CXXFLAGS="-pg $CXXFLAGS"
    ])
  fi

  if test "$kde_use_gcov" = "yes"; then
    KDE_CHECK_COMPILER_FLAG(fprofile-arcs,
    [
      CFLAGS="-fprofile-arcs -ftest-coverage $CFLAGS"
      CXXFLAGS="-fprofile-arcs -ftest-coverage $CXXFLAGS"
    ])
  fi

  if test "$kde_use_warnings" = "yes"; then
      if test "$GCC" = "yes"; then
        case $host in
          *-*-linux-gnu)	
            CFLAGS="-ansi -W -Wall -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -D_XOPEN_SOURCE=500 -D_BSD_SOURCE $CFLAGS"
            CXXFLAGS="-ansi -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -Wcast-align -Wconversion $CXXFLAGS"
          ;;
        esac
        CXXFLAGS="-Wall -pedantic -W -Wpointer-arith -Wmissing-prototypes -Wwrite-strings $CXXFLAGS"
        KDE_CHECK_COMPILER_FLAG(Wundef,[CXXFLAGS="-Wundef $CXXFLAGS"])
        KDE_CHECK_COMPILER_FLAG(Wno-long-long,[CXXFLAGS="-Wno-long-long $CXXFLAGS"])
        KDE_CHECK_COMPILER_FLAG(Wnon-virtual-dtor,[CXXFLAGS="-Wnon-virtual-dtor $CXXFLAGS"])
     fi
  fi

  if test "$GCC" = "yes" && test "$kde_use_strict_options" = "yes"; then
    CXXFLAGS="-Wcast-qual -Wshadow -Wcast-align $CXXFLAGS"
  fi
    
  if test "$GXX" = "yes"; then
    dnl mrj, don't do that. tora source uses exceptions.
    dnl KDE_CHECK_COMPILER_FLAG(fno-exceptions,[CXXFLAGS="$CXXFLAGS -fno-exceptions"])
    KDE_CHECK_COMPILER_FLAG(fno-check-new, [CXXFLAGS="$CXXFLAGS -fno-check-new"])
    KDE_CHECK_COMPILER_FLAG(fexceptions, [USE_EXCEPTIONS="-fexceptions"], USE_EXCEPTIONS=	)
  fi
  if test "$CXX" = "KCC"; then
    dnl unfortunately we currently cannot disable exception support in KCC
    dnl because doing so is binary incompatible and Qt by default links with exceptions :-(
    dnl KDE_CHECK_COMPILER_FLAG(-no_exceptions,[CXXFLAGS="$CXXFLAGS --no_exceptions"])
    dnl KDE_CHECK_COMPILER_FLAG(-exceptions, [USE_EXCEPTIONS="--exceptions"], USE_EXCEPTIONS=	)

    AC_ARG_ENABLE(pch,[  --enable-pch            enables precompiled header support (currently only KCC) [default=no]],
    [
      kde_use_pch=$enableval
    ],[kde_use_pch=no])
 
    if test "$kde_use_pch" = "yes"; then
      dnl TODO: support --pch-dir!
      KDE_CHECK_COMPILER_FLAG(-pch,[CXXFLAGS="$CXXFLAGS --pch"])
      dnl the below works (but the dir must exist), but it's
      dnl useless for a whole package.
      dnl The are precompiled headers for each source file, so when compiling
      dnl from scratch, it doesn't make a difference, and they take up
      dnl around ~5Mb _per_ sourcefile.
      dnl KDE_CHECK_COMPILER_FLAG(-pch_dir /tmp,
      dnl   [CXXFLAGS="$CXXFLAGS --pch_dir `pwd`/pcheaders"])
    fi
    dnl this flag controls inlining. by default KCC inlines in optimisation mode
    dnl all implementations that are defined inside the class {} declaration. 
    dnl because of templates-compatibility with broken gcc compilers, this
    dnl can cause excessive inlining. This flag limits it to a sane level
    KDE_CHECK_COMPILER_FLAG(-inline_keyword_space_time=6,[CXXFLAGS="$CXXFLAGS --inline_keyword_space_time=6"])
    KDE_CHECK_COMPILER_FLAG(-inline_auto_space_time=2,[CXXFLAGS="$CXXFLAGS --inline_auto_space_time=2"])
    KDE_CHECK_COMPILER_FLAG(-inline_implicit_space_time=2.0,[CXXFLAGS="$CXXFLAGS --inline_implicit_space_time=2.0"])
    KDE_CHECK_COMPILER_FLAG(-inline_generated_space_time=2.0,[CXXFLAGS="$CXXFLAGS --inline_generated_space_time=2.0"])
    dnl Some source files are shared between multiple executables
    dnl (or libraries) and some of those need template instantiations.
    dnl In that case KCC needs to compile those sources with
    dnl --one_instantiation_per_object.  To make it easy for us we compile
    dnl _all_ objects with that flag (--one_per is a shorthand).
    KDE_CHECK_COMPILER_FLAG(-one_per, [CXXFLAGS="$CXXFLAGS --one_per"])
  fi
  AC_SUBST(USE_EXCEPTIONS)
  dnl obsolete macro - provided to keep things going
  USE_RTTI=
  AC_SUBST(USE_RTTI)

  case "$host" in
      *-*-irix*)  test "$GXX" = yes && CXXFLAGS="-D_LANGUAGE_C_PLUS_PLUS -D__LANGUAGE_C_PLUS_PLUS $CXXFLAGS" ;;
      *-*-sysv4.2uw*) CXXFLAGS="-D_UNIXWARE $CXXFLAGS";;
      *-*-sysv5uw7*) CXXFLAGS="-D_UNIXWARE7 $CXXFLAGS";;
      *-*-solaris*) 
        if test "$GXX" = yes; then
          libstdcpp=`$CXX -print-file-name=libstdc++.so`
          if test ! -f $libstdcpp; then
             AC_MSG_ERROR([You've compiled gcc without --enable-shared. This doesn't work with KDE. Please recompile gcc with --enable-shared to receive a libstdc++.so])
          fi
        fi
        ;;
  esac

  AC_VALIDIFY_CXXFLAGS

  AC_PROG_CXXCPP

  if test "$GCC" = yes; then
     NOOPT_CXXFLAGS=-O0
     NOOPT_CFLAGS=-O0
  fi

  AC_SUBST(NOOPT_CXXFLAGS)
  AC_SUBST(NOOPT_CFLAGS)

  KDE_CHECK_FINAL

  ifdef([AM_DEPENDENCIES], AC_REQUIRE([KDE_ADD_DEPENDENCIES]), [])
])

AC_DEFUN([KDE_ADD_DEPENDENCIES],
[
   [A]M_DEPENDENCIES(CC)
   [A]M_DEPENDENCIES(CXX)
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

AC_DEFUN([KDE_CHECK_TYPES],
[  AC_CHECK_SIZEOF(int, 4)dnl
  AC_CHECK_SIZEOF(long, 4)dnl
  AC_CHECK_SIZEOF(char *, 4)dnl
  AC_CHECK_SIZEOF(char, 1)dnl
])dnl

AC_DEFUN([KDE_DO_IT_ALL],
[
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM
AM_INIT_AUTOMAKE($1, $2)
AM_DISABLE_LIBRARIES
AC_PREFIX_DEFAULT(${KDEDIR:-/usr/local/kde})
AC_CHECK_COMPILERS
AM_KDE_WITH_NLS
AC_PATH_KDE
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

dnl This is a merge of some macros out of the gettext aclocal.m4
dnl since we don't need anything, I took the things we need
dnl the copyright for them is:
dnl >
dnl Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
dnl This Makefile.in is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.
dnl >
dnl for this file it is relicensed under LGPL

AC_DEFUN([AM_KDE_WITH_NLS],
  [
    dnl If we use NLS figure out what method

    AM_PATH_PROG_WITH_TEST_KDE(MSGFMT, msgfmt,
        [test -n "`$ac_dir/$ac_word --version 2>&1 | grep 'GNU gettext'`"], msgfmt)
    AC_PATH_PROG(GMSGFMT, gmsgfmt, $MSGFMT)

     if test -z "`$GMSGFMT --version 2>&1 | grep 'GNU gettext'`"; then
        AC_MSG_RESULT([found msgfmt program is not GNU msgfmt; ignore it])
        GMSGFMT=":"
      fi
      MSGFMT=$GMSGFMT
      AC_SUBST(GMSGFMT)
      AC_SUBST(MSGFMT)

      AM_PATH_PROG_WITH_TEST_KDE(XGETTEXT, xgettext,
	[test -z "`$ac_dir/$ac_word -h 2>&1 | grep '(HELP)'`"], :)

      dnl Test whether we really found GNU xgettext.
      if test "$XGETTEXT" != ":"; then
	dnl If it is no GNU xgettext we define it as : so that the
	dnl Makefiles still can work.
	if $XGETTEXT --omit-header /dev/null 2> /dev/null; then
	  : ;
	else
	  AC_MSG_RESULT(
	    [found xgettext programs is not GNU xgettext; ignore it])
	  XGETTEXT=":"
	fi
      fi
     AC_SUBST(XGETTEXT)

  ])

# Search path for a program which passes the given test.
# Ulrich Drepper <drepper@cygnus.com>, 1996.

# serial 1
# Stephan Kulow: I appended a _KDE against name conflicts

dnl AM_PATH_PROG_WITH_TEST_KDE(VARIABLE, PROG-TO-CHECK-FOR,
dnl   TEST-PERFORMED-ON-FOUND_PROGRAM [, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN([AM_PATH_PROG_WITH_TEST_KDE],
[# Extract the first word of "$2", so it can be a program name with args.
set dummy $2; ac_word=[$]2
AC_MSG_CHECKING([for $ac_word])
AC_CACHE_VAL(ac_cv_path_$1,
[case "[$]$1" in
  /*)
  ac_cv_path_$1="[$]$1" # Let the user override the test with a path.
  ;;
  *)
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in ifelse([$5], , $PATH, [$5]); do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/$ac_word; then
      if [$3]; then
	ac_cv_path_$1="$ac_dir/$ac_word"
	break
      fi
    fi
  done
  IFS="$ac_save_ifs"
dnl If no 4th arg is given, leave the cache variable unset,
dnl so AC_PATH_PROGS will keep looking.
ifelse([$4], , , [  test -z "[$]ac_cv_path_$1" && ac_cv_path_$1="$4"
])dnl
  ;;
esac])dnl
$1="$ac_cv_path_$1"
if test -n "[$]$1"; then
  AC_MSG_RESULT([$]$1)
else
  AC_MSG_RESULT(no)
fi
AC_SUBST($1)dnl
])


# Check whether LC_MESSAGES is available in <locale.h>.
# Ulrich Drepper <drepper@cygnus.com>, 1995.

# serial 1

AC_DEFUN([AM_LC_MESSAGES],
  [if test $ac_cv_header_locale_h = yes; then
    AC_CACHE_CHECK([for LC_MESSAGES], am_cv_val_LC_MESSAGES,
      [
       AC_LINK_IFELSE([AC_LANG_PROGRAM([[
        #include <locale.h>]], [[return LC_MESSAGES]])],
       am_cv_val_LC_MESSAGES=yes, am_cv_val_LC_MESSAGES=no)])
    if test $am_cv_val_LC_MESSAGES = yes; then
      AC_DEFINE(HAVE_LC_MESSAGES, 1, [Define if your locale.h file contains LC_MESSAGES])
    fi
  fi])

dnl From Jim Meyering.
dnl FIXME: migrate into libit.

AC_DEFUN([AM_FUNC_OBSTACK],
[AC_CACHE_CHECK([for obstacks], am_cv_func_obstack,
 [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include "obstack.h"]],
	      [[struct obstack *mem;obstack_free(mem,(char *) 0)]])],
	      am_cv_func_obstack=yes,
	      am_cv_func_obstack=no)])
 if test $am_cv_func_obstack = yes; then
   AC_DEFINE(HAVE_OBSTACK)
 else
   LIBOBJS="$LIBOBJS obstack.o"
 fi
])

dnl From Jim Meyering.  Use this if you use the GNU error.[ch].
dnl FIXME: Migrate into libit

AC_DEFUN([AM_FUNC_ERROR_AT_LINE],
[AC_CACHE_CHECK([for error_at_line], am_cv_lib_error_at_line,
 [AC_LINK_IFELSE([AC_LANG_PROGRAM([[]],[[error_at_line(0, 0, "", 0, "");]])],
              am_cv_lib_error_at_line=yes,
	      am_cv_lib_error_at_line=no)])
 if test $am_cv_lib_error_at_line = no; then
   LIBOBJS="$LIBOBJS error.o"
 fi
 AC_SUBST(LIBOBJS)dnl
])

# Macro to add for using GNU gettext.
# Ulrich Drepper <drepper@cygnus.com>, 1995.

# serial 1
# Stephan Kulow: I put a KDE in it to avoid name conflicts

AC_DEFUN([AM_KDE_GNU_GETTEXT],
  [AC_REQUIRE([AC_PROG_MAKE_SET])dnl
   AC_REQUIRE([AC_PROG_RANLIB])dnl
   AC_REQUIRE([AC_HEADER_STDC])dnl
   AC_REQUIRE([AC_TYPE_OFF_T])dnl
   AC_REQUIRE([AC_TYPE_SIZE_T])dnl
   AC_REQUIRE([AC_FUNC_ALLOCA])dnl
   AC_REQUIRE([AC_FUNC_MMAP])dnl
   AC_REQUIRE([AM_KDE_WITH_NLS])dnl
   AC_CHECK_HEADERS([limits.h locale.h nl_types.h string.h values.h alloca.h])
   AC_CHECK_FUNCS([getcwd munmap putenv setlocale strchr strcasecmp \
__argz_count __argz_stringify __argz_next])

   AC_MSG_CHECKING(for stpcpy)
   AC_CACHE_VAL(kde_cv_func_stpcpy,
   [
   kde_safe_cxxflags=$CXXFLAGS
   CXXFLAGS="-Wmissing-prototypes -Werror"
   AC_LANG_PUSH(C++)
   AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
   #include <string.h>
   ]],
   [[
   char buffer[200];
   stpcpy(buffer, buffer);
   ]])],
   kde_cv_func_stpcpy=yes,
   kde_cv_func_stpcpy=no)
   AC_LANG_POP([C++])
   CXXFLAGS=$kde_safe_cxxflags
   ])
   AC_MSG_RESULT($kde_cv_func_stpcpy)
   if eval "test \"`echo $kde_cv_func_stpcpy`\" = yes"; then
     AC_DEFINE(HAVE_STPCPY, 1, [Define if you have stpcpy])
   fi

   AM_LC_MESSAGES

   if test "x$CATOBJEXT" != "x"; then
     if test "x$ALL_LINGUAS" = "x"; then
       LINGUAS=
     else
       AC_MSG_CHECKING(for catalogs to be installed)
       NEW_LINGUAS=
       for lang in ${LINGUAS=$ALL_LINGUAS}; do
         case "$ALL_LINGUAS" in
          *$lang*) NEW_LINGUAS="$NEW_LINGUAS $lang" ;;
         esac
       done
       LINGUAS=$NEW_LINGUAS
       AC_MSG_RESULT($LINGUAS)
     fi

     dnl Construct list of names of catalog files to be constructed.
     if test -n "$LINGUAS"; then
       for lang in $LINGUAS; do CATALOGS="$CATALOGS $lang$CATOBJEXT"; done
     fi
   fi

  ])

AC_DEFUN([AC_HAVE_XPM],
 [AC_REQUIRE_CPP()dnl
  AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])

 test -z "$XPM_LDFLAGS" && XPM_LDFLAGS=
 test -z "$XPM_INCLUDE" && XPM_INCLUDE=

 AC_ARG_WITH(xpm, [  --without-xpm           disable color pixmap XPM tests],
	xpm_test=$withval, xpm_test="yes")
 if test "x$xpm_test" = xno; then
   ac_cv_have_xpm=no
 else
   AC_MSG_CHECKING(for XPM)
   AC_CACHE_VAL(ac_cv_have_xpm,
   [
    AC_LANG(C)
    ac_save_ldflags="$LDFLAGS"
    ac_save_cflags="$CFLAGS"
    if test "x$kde_use_qt_emb" != "xyes"; then
      LDFLAGS="$LDFLAGS $X_LDFLAGS $USER_LDFLAGS $LDFLAGS $XPM_LDFLAGS $all_libraries -lXpm -lX11 -lXext $LIBZ $LIBSOCKET"
    else
      LDFLAGS="$LDFLAGS $X_LDFLAGS $USER_LDFLAGS $LDFLAGS $XPM_LDFLAGS $all_libraries -lXpm $LIBZ $LIBSOCKET"
    fi
    CFLAGS="$CFLAGS $X_INCLUDES $USER_INCLUDES"
    test -n "$XPM_INCLUDE" && CFLAGS="-I$XPM_INCLUDE $CFLAGS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <X11/xpm.h>]],[[]])],
	ac_cv_have_xpm="yes",ac_cv_have_xpm="no")
    LDFLAGS="$ac_save_ldflags"
    CFLAGS="$ac_save_cflags"
   ])dnl

  if test "$ac_cv_have_xpm" = no; then
    AC_MSG_RESULT(no)
    XPM_LDFLAGS=""
    XPMINC=""
    $2
  else
    AC_DEFINE(HAVE_XPM, 1, [Define if you have XPM support])
    if test "$XPM_LDFLAGS" = ""; then
       XPMLIB='-lXpm $(LIB_X11)'
    else
       XPMLIB="-L$XPM_LDFLAGS -lXpm "'$(LIB_X11)'
    fi
    if test "$XPM_INCLUDE" = ""; then
       XPMINC=""
    else
       XPMINC="-I$XPM_INCLUDE"
    fi
    AC_MSG_RESULT(yes)
    $1
  fi
 fi
 AC_SUBST(XPMINC)
 AC_SUBST(XPMLIB)
])

AC_DEFUN([AC_HAVE_DPMS],
 [AC_REQUIRE_CPP()dnl
  AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])

 test -z "$DPMS_LDFLAGS" && DPMS_LDFLAGS=
 test -z "$DPMS_INCLUDE" && DPMS_INCLUDE=
 DPMS_LIB=

 AC_ARG_WITH(dpms, [  --without-dpms          disable DPMS power saving],
	dpms_test=$withval, dpms_test="yes")
 if test "x$dpms_test" = xno; then
   ac_cv_have_dpms=no
 else
   AC_MSG_CHECKING(for DPMS)
   dnl Note: ac_cv_have_dpms can be no, yes, or -lXdpms.
   dnl 'yes' means DPMS_LIB="", '-lXdpms' means DPMS_LIB="-lXdpms".
   AC_CACHE_VAL(ac_cv_have_dpms,
   [
    if test "x$kde_use_qt_emb" = "xyes"; then
      AC_MSG_RESULT(no)
      ac_cv_have_dpms="no"
    else
      AC_LANG(C)
      ac_save_ldflags="$LDFLAGS"
      ac_save_cflags="$CFLAGS"
      ac_save_libs="$LIBS"
      LDFLAGS="$LDFLAGS $DPMS_LDFLAGS $all_libraries -lX11 -lXext $LIBSOCKET"
      CFLAGS="$CFLAGS $X_INCLUDES"
      test -n "$DPMS_INCLUDE" && CFLAGS="-I$DPMS_INCLUDE $CFLAGS"
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[											 
	  #include <X11/Xproto.h>
	  #include <X11/X.h>
	  #include <X11/Xlib.h>
	  #include <X11/extensions/dpms.h>
	  int foo_test_dpms()
	  { return DPMSSetTimeouts( 0, 0, 0, 0 ); }]],[[]])],
	  ac_cv_have_dpms="yes", [
              LDFLAGS="$ac_save_ldflags"
              CFLAGS="$ac_save_cflags"
              LDFLAGS="$LDFLAGS $DPMS_LDFLAGS $all_libraries -lX11 -lXext $LIBSOCKET"
              LIBS="$LIBS -lXdpms"
              CFLAGS="$CFLAGS $X_INCLUDES"
              test -n "$DPMS_INCLUDE" && CFLAGS="-I$DPMS_INCLUDE $CFLAGS"
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[
	          #include <X11/Xproto.h>
        	  #include <X11/X.h>
        	  #include <X11/Xlib.h>
        	  #include <X11/extensions/dpms.h>
        	  int foo_test_dpms()
        	  { return DPMSSetTimeouts( 0, 0, 0, 0 ); }]],[[]])],
        	  [
                  ac_cv_have_dpms="-lXdpms"
                  ],ac_cv_have_dpms="no")
              ])
      LDFLAGS="$ac_save_ldflags"
      CFLAGS="$ac_save_cflags"
      LIBS="$ac_save_libs"
    fi
   ])dnl

  if test "$ac_cv_have_dpms" = no; then
    AC_MSG_RESULT(no)
    DPMS_LDFLAGS=""
    DPMSINC=""
    $2
  else
    AC_DEFINE(HAVE_DPMS, 1, [Define if you have DPMS support])
    if test "$ac_cv_have_dpms" = "-lXdpms"; then
       DPMS_LIB="-lXdpms"
    fi
    if test "$DPMS_LDFLAGS" = ""; then
       DPMSLIB="$DPMS_LIB "'$(LIB_X11)'
    else
       DPMSLIB="$DPMS_LDFLAGS $DPMS_LIB "'$(LIB_X11)'
    fi
    if test "$DPMS_INCLUDE" = ""; then
       DPMSINC=""
    else
       DPMSINC="-I$DPMS_INCLUDE"
    fi
    AC_MSG_RESULT(yes)
    $1
  fi
 fi
 AC_SUBST(DPMSINC)
 AC_SUBST(DPMSLIB)
])

AC_DEFUN([AC_HAVE_GL],
 [AC_REQUIRE_CPP()dnl
  AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])

 test -z "$GL_LDFLAGS" && GL_LDFLAGS=
 test -z "$GL_INCLUDE" && GL_INCLUDE=

 AC_ARG_WITH(gl, [  --without-gl            disable 3D GL modes],
	gl_test=$withval, gl_test="yes")
 if test "x$kde_use_qt_emb" = "xyes"; then
   # GL and Qt Embedded is a no-go for now.
   ac_cv_have_gl=no
 elif test "x$gl_test" = xno; then
   ac_cv_have_gl=no
 else
   AC_MSG_CHECKING(for GL)
   AC_CACHE_VAL(ac_cv_have_gl,
   [
    AC_LANG_PUSH(C++)
    ac_save_ldflags="$LDFLAGS"
    ac_save_cxxflags="$CXXFLAGS"
    LDFLAGS="$LDFLAGS $GL_LDFLAGS $X_LDFLAGS $all_libraries -lMesaGL -lMesaGLU"
    test "x$kde_use_qt_emb" != xyes && LDFLAGS="$LDFLAGS -lX11"
    LDFLAGS="$LDFLAGS $LIB_XEXT -lm $LIBSOCKET"
    CXXFLAGS="$CFLAGS $X_INCLUDES"
    test -n "$GL_INCLUDE" && CFLAGS="-I$GL_INCLUDE $CFLAGS"
     AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <GL/gl.h>
#include <GL/glu.h>
]], [[]])],
	ac_cv_have_gl="mesa", ac_cv_have_gl="no")
    if test "x$ac_cv_have_gl" = "xno"; then
      LDFLAGS="$ac_save_ldflags $X_LDFLAGS $GL_LDFLAGS $all_libraries -lGLU -lGL"
      test "x$kde_use_qt_emb" != xyes && LDFLAGS="$LDFLAGS -lX11"
      LDFLAGS="$LDFLAGS $LIB_XEXT -lm $LIBSOCKET"
      CXXFLAGS="$ac_save_cflags $X_INCLUDES"
      test -n "$GL_INCLUDE" && CFLAGS="-I$GL_INCLUDE $CFLAGS"
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <GL/gl.h>
#include <GL/glu.h>
]], [[]])],
	  ac_cv_have_gl="yes", ac_cv_have_gl="no")
    fi
    AC_LANG_POP([C++])
    LDFLAGS="$ac_save_ldflags"
    CXXFLAGS="$ac_save_cxxflags"
   ])dnl

  if test "$ac_cv_have_gl" = "no"; then
    AC_MSG_RESULT(no)
    GL_LDFLAGS=""
    GLINC=""
    $2
  else
    AC_DEFINE(HAVE_GL, 1, [Defines if you have GL (Mesa, OpenGL, ...)])
    if test "$GL_LDFLAGS" = ""; then
       if test "$ac_cv_have_gl" = "mesa"; then
          GLLIB='-lMesaGLU -lMesaGL $(LIB_X11)'
       else
          GLLIB='-lGLU -lGL $(LIB_X11)'
       fi
    else
       if test "$ac_cv_have_gl" = "mesa"; then
          GLLIB="$GL_LDFLAGS -lMesaGLU -lMesaGL "'$(LIB_X11)'
       else
          GLLIB="$GL_LDFLAGS -lGLU -lGL "'$(LIB_X11)'
       fi
    fi
    if test "$GL_INCLUDE" = ""; then
       GLINC=""
    else
       GLINC="-I$GL_INCLUDE"
    fi
    AC_MSG_RESULT($ac_cv_have_gl)
    $1
  fi
 fi
 AC_SUBST(GLINC)
 AC_SUBST(GLLIB)
])


 dnl shadow password and PAM magic - maintained by ossi@kde.org

AC_DEFUN([KDE_PAM], [
  AC_REQUIRE([KDE_CHECK_LIBDL])

  AC_ARG_WITH(pam,
    [  --with-pam[=ARG]        enable support for PAM: ARG=[yes|no|service name]],
    [ if test "x$withval" = "xyes"; then
        use_pam=yes
        pam_service=kde
      elif test "x$withval" = "xno"; then
        use_pam=no
      else
        use_pam=yes
        pam_service=$withval
      fi
      ac_cv_path_pam="use_pam=$use_pam pam_service=$pam_service"
    ], [
      AC_CACHE_VAL(ac_cv_path_pam,
        [ use_pam=no
          AC_CHECK_LIB(pam, pam_start,
            [ AC_CHECK_HEADER(security/pam_appl.h,
                [ use_pam=yes
                  pam_service=kde ]) 
            ], , $LIBDL)
          ac_cv_path_pam="use_pam=$use_pam pam_service=$pam_service"
        ])
    ])
  eval "$ac_cv_path_pam"

  AC_MSG_CHECKING(for PAM)
  if test "x$use_pam" = xno; then
    AC_MSG_RESULT(no)
    PAMLIBS=""
  else
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_PAM, 1, [Defines if you have PAM (Pluggable Authentication Modules)])
    PAMLIBS="$PAM_MISC_LIB -lpam $LIBDL"

    dnl test whether struct pam_message is const (Linux) or not (Sun)
    AC_MSG_CHECKING(for const pam_message)
    AC_EGREP_HEADER([struct pam_message], security/pam_appl.h,
      [ AC_EGREP_HEADER([const struct pam_message], security/pam_appl.h,
                        [AC_MSG_RESULT([const: Linux-type PAM])],
                        [AC_MSG_RESULT([nonconst: Sun-type PAM])
                        AC_DEFINE(PAM_MESSAGE_NONCONST, 1, [Define if your PAM support takes non-const arguments (Solaris)])]
                        )],
      [AC_MSG_RESULT([not found - assume const, Linux-type PAM])])
  fi

  AC_SUBST(PAMLIBS)
])

dnl DEF_PAM_SERVICE(arg name, full name, define name)
AC_DEFUN([DEF_PAM_SERVICE], [
  AC_ARG_WITH($1-pam,
    [  --with-$1-pam=[val]    override PAM service from --with-pam for $2],
    [ if test "x$use_pam" = xyes; then
        $3_PAM_SERVICE="$withval"
      else
        AC_MSG_ERROR([Cannot use use --with-$1-pam, as no PAM was detected.
You may want to enforce it by using --with-pam.])
      fi
    ], 
    [ if test "x$use_pam" = xyes; then
        $3_PAM_SERVICE="$pam_service"
      fi
    ])
    if test -n "$$3_PAM_SERVICE"; then
      AC_MSG_RESULT([The PAM service used by $2 will be $$3_PAM_SERVICE])
      AC_DEFINE_UNQUOTED($3_PAM_SERVICE, "$$3_PAM_SERVICE", [The PAM service to be used by $2])
    fi
    AC_SUBST($3_PAM_SERVICE)
])

AC_DEFUN([KDE_SHADOWPASSWD], [
  AC_REQUIRE([KDE_PAM])

  AC_CHECK_LIB(shadow, getspent,
    [ LIBSHADOW="-lshadow"
      ac_use_shadow=yes
    ],
    [ dnl for UnixWare
      AC_CHECK_LIB(gen, getspent, 
        [ LIBGEN="-lgen"
          ac_use_shadow=yes
        ], 
        [ AC_CHECK_FUNC(getspent, 
            [ ac_use_shadow=yes ],
            [ ac_use_shadow=no ])
	])
    ])
  AC_SUBST(LIBSHADOW)
  AC_SUBST(LIBGEN)
  
  AC_MSG_CHECKING([for shadow passwords])

  AC_ARG_WITH(shadow,
    [  --with-shadow		  If you want shadow password support ],
    [ if test "x$withval" != "xno"; then
        use_shadow=yes
      else
        use_shadow=no
      fi
    ], [
      use_shadow="$ac_use_shadow"
    ])

  if test "x$use_shadow" = xyes; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_SHADOW, 1, [Define if you use shadow passwords])
  else
    AC_MSG_RESULT(no)
    LIBSHADOW=
    LIBGEN=
  fi

  dnl finally make the relevant binaries setuid root, if we have shadow passwds.
  dnl this still applies, if we could use it indirectly through pam.
  if test "x$use_shadow" = xyes || 
     ( test "x$use_pam" = xyes && test "x$ac_use_shadow" = xyes ); then
      case $host in
      *-*-freebsd* | *-*-netbsd* | *-*-openbsd*)
	SETUIDFLAGS="-m 4755 -o root";;
      *)
	SETUIDFLAGS="-m 4755";;
      esac
  fi
  AC_SUBST(SETUIDFLAGS)

])

AC_DEFUN([KDE_PASSWDLIBS], [
  AC_REQUIRE([KDE_MISC_TESTS]) dnl for LIBCRYPT
  AC_REQUIRE([KDE_PAM])
  AC_REQUIRE([KDE_SHADOWPASSWD])

  if test "x$use_pam" = "xyes"; then 
    PASSWDLIBS="$PAMLIBS"
  else
    PASSWDLIBS="$LIBCRYPT $LIBSHADOW $LIBGEN"
  fi

  dnl FreeBSD uses a shadow-like setup, where /etc/passwd holds the users, but
  dnl /etc/master.passwd holds the actual passwords.  /etc/master.passwd requires
  dnl root to read, so kcheckpass needs to be root (even when using pam, since pam
  dnl may need to read /etc/master.passwd).
  case $host in
  *-*-freebsd*)
    SETUIDFLAGS="-m 4755 -o root"
    ;;
  *)
    ;;
  esac

  AC_SUBST(PASSWDLIBS)
])

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

AC_DEFUN([KDE_CHECK_DLOPEN],
[
KDE_CHECK_LIBDL
AC_CHECK_HEADERS(dlfcn.h dl.h)
if test "$ac_cv_header_dlfcn_h" = "no"; then
  ac_cv_have_dlfcn=no
fi

if test "$ac_cv_header_dl_h" = "no"; then
  ac_cv_have_shload=no
fi

dnl XXX why change enable_dlopen? its already set by autoconf's AC_ARG_ENABLE
dnl (MM)
AC_ARG_ENABLE(dlopen,
[  --disable-dlopen        link statically [default=no]] ,
enable_dlopen=$enableval,
enable_dlopen=yes)

# override the user's opinion, if we know it better ;)
if test "$ac_cv_have_dlfcn" = "no" && test "$ac_cv_have_shload" = "no"; then
  enable_dlopen=no
fi

if test "$ac_cv_have_dlfcn" = "yes"; then
  AC_DEFINE_UNQUOTED(HAVE_DLFCN, 1, [Define if you have dlfcn])
fi

if test "$ac_cv_have_shload" = "yes"; then
  AC_DEFINE_UNQUOTED(HAVE_SHLOAD, 1, [Define if you have shload])
fi

if test "$enable_dlopen" = no ; then
  test -n "$1" && eval $1
else
  test -n "$2" && eval $2
fi

])

AC_DEFUN([KDE_ADD_INCLUDES],
[
if test -z "$1"; then
  test_include="Pix.h"
else
  test_include="$1"
fi

AC_MSG_CHECKING([for libg++ ($test_include)])

AC_CACHE_VAL(kde_cv_libgpp_includes,
[
kde_cv_libgpp_includes=no

   for ac_dir in               \
                               \
     /usr/include/g++          \
     /usr/include              \
     /usr/unsupported/include  \
     /opt/include              \
     $extra_include            \
     ; \
   do
     if test -r "$ac_dir/$test_include"; then
       kde_cv_libgpp_includes=$ac_dir
       break
     fi
   done
])

AC_MSG_RESULT($kde_cv_libgpp_includes)
if test "$kde_cv_libgpp_includes" != "no"; then
  all_includes="-I$kde_cv_libgpp_includes $all_includes $USER_INCLUDES"
fi
])
])


AC_DEFUN([KDE_CHECK_MICO],
[
AC_REQUIRE([KDE_CHECK_LIBDL])
AC_REQUIRE([KDE_MISC_TESTS])
AC_MSG_CHECKING(for MICO)

if test -z "$MICODIR"; then
    kde_micodir=/usr/local
 else
    kde_micodir="$MICODIR"
fi

AC_ARG_WITH(micodir,
  [  --with-micodir=micodir  where mico is installed ],
  kde_micodir=$withval,
  kde_micodir=$kde_micodir
)

AC_CACHE_VAL(kde_cv_mico_incdir,
[
  mico_incdirs="$kde_micodir/include /usr/include /usr/local/include /usr/local/include /opt/local/include $kde_extra_includes"
AC_FIND_FILE(CORBA.h, $mico_incdirs, kde_cv_mico_incdir)

])
kde_micodir=`echo $kde_cv_mico_incdir | sed -e 's#/include##'`

if test ! -r  $kde_micodir/include/CORBA.h; then
  AC_MSG_ERROR([No CORBA.h found, specify another micodir])
fi

AC_MSG_RESULT($kde_micodir)

MICO_INCLUDES=-I$kde_micodir/include
AC_SUBST(MICO_INCLUDES)
MICO_LDFLAGS=-L$kde_micodir/lib
AC_SUBST(MICO_LDFLAGS)
micodir=$kde_micodir
AC_SUBST(micodir)

AC_MSG_CHECKING([for MICO version])
AC_CACHE_VAL(kde_cv_mico_version,
[
AC_LANG(C)
cat >conftest.$ac_ext <<EOF
#include <stdio.h>
#include <mico/version.h>
int main() {

   printf("MICO_VERSION=%s\n",MICO_VERSION);
   return (0);
}
EOF
ac_compile='${CC-gcc} $CFLAGS $MICO_INCLUDES conftest.$ac_ext -o conftest'
if AC_TRY_EVAL(ac_compile); then
  if eval `./conftest 2>&5`; then
    kde_cv_mico_version=$MICO_VERSION
  else
    AC_MSG_ERROR([your system is not able to execute a small application to
    find MICO version! Check $kde_micodir/include/mico/version.h])
  fi
else
  AC_MSG_ERROR([your system is not able to compile a small application to
  find MICO version! Check $kde_micodir/include/mico/version.h])
fi
])

dnl installed MICO version
mico_v_maj=`echo $kde_cv_mico_version | sed -e 's/^\(.*\)\..*\..*$/\1/'`
mico_v_mid=`echo $kde_cv_mico_version | sed -e 's/^.*\.\(.*\)\..*$/\1/'`
mico_v_min=`echo $kde_cv_mico_version | sed -e 's/^.*\..*\.\(.*\)$/\1/'`

if test "x$1" = "x"; then
 req_version="2.3.0"
else
 req_version=$1
fi

dnl required MICO version
req_v_maj=`echo $req_version | sed -e 's/^\(.*\)\..*\..*$/\1/'`
req_v_mid=`echo $req_version | sed -e 's/^.*\.\(.*\)\..*$/\1/'`
req_v_min=`echo $req_version | sed -e 's/^.*\..*\.\(.*\)$/\1/'`

if test "$mico_v_maj" -lt "$req_v_maj" || \
   ( test "$mico_v_maj" -eq "$req_v_maj" && \
        test "$mico_v_mid" -lt "$req_v_mid" ) || \
   ( test "$mico_v_mid" -eq "$req_v_mid" && \
        test "$mico_v_min" -lt "$req_v_min" )

then
  AC_MSG_ERROR([found MICO version $kde_cv_mico_version but version $req_version \
at least is required. You should upgrade MICO.])
else
  AC_MSG_RESULT([$kde_cv_mico_version (minimum version $req_version, ok)])
fi

LIBMICO="-lmico$kde_cv_mico_version $LIBCRYPT $LIBSOCKET $LIBDL"
AC_SUBST(LIBMICO)
if test -z "$IDL"; then
  IDL='$(kde_bindir)/cuteidl'
fi
AC_SUBST(IDL)
IDL_DEPENDENCIES='$(kde_includes)/CUTE.h'
AC_SUBST(IDL_DEPENDENCIES)

idldir="\$(includedir)/idl"
AC_SUBST(idldir)

])

AC_DEFUN([KDE_CHECK_MINI_STL],
[
AC_REQUIRE([KDE_CHECK_MICO])

AC_MSG_CHECKING(if we use mico's mini-STL)
AC_CACHE_VAL(kde_cv_have_mini_stl,
[
AC_LANG_PUSH(C++)
kde_save_cxxflags="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS $MICO_INCLUDES"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <mico/config.h>
]],
[[
#ifdef HAVE_MINI_STL
#error "nothing"
#endif
]])],
kde_cv_have_mini_stl=no,
kde_cv_have_mini_stl=yes)
CXXFLAGS="$kde_save_cxxflags"
AC_LANG_POP([C++])
])

if test "x$kde_cv_have_mini_stl" = "xyes"; then
   AC_MSG_RESULT(yes)
   $1
else
   AC_MSG_RESULT(no)
   $2
fi
])

])


AC_DEFUN([KDE_CHECK_LIBPTHREAD],
[
AC_CHECK_LIB(pthread, pthread_create, [LIBPTHREAD="-lpthread"] )
AC_SUBST(LIBPTHREAD)
])

AC_DEFUN([KDE_CHECK_PTHREAD_OPTION],
[
    AC_ARG_ENABLE(
      kernel-threads,
      [  --enable-kernel-threads Enable the use of the LinuxThreads port on FreeBSD/i386 only.],
      kde_use_kernthreads=$enableval,
      kde_use_kernthreads=no)

    if test "$kde_use_kernthreads" = "yes"; then
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_CFLAGS="$CFLAGS"
      CXXFLAGS="-I/usr/local/include/pthread/linuxthreads $CXXFLAGS"
      CFLAGS="-I/usr/local/include/pthread/linuxthreads $CFLAGS"
      AC_CHECK_HEADERS(pthread/linuxthreads/pthread.h)
      CXXFLAGS="$ac_save_CXXFLAGS"
      CFLAGS="$ac_save_CFLAGS"
      if test "$ac_cv_header_pthread_linuxthreads_pthread_h" = "no"; then
        kde_use_kernthreads=no
      else
        dnl Add proper -I and -l statements
        AC_CHECK_LIB(lthread, pthread_join, [LIBPTHREAD="-llthread -llgcc_r"]) dnl for FreeBSD
        if test "x$LIBPTHREAD" = "x"; then
          kde_use_kernthreads=no
        else
          USE_THREADS="-D_THREAD_SAFE -I/usr/local/include/pthread/linuxthreads"
        fi
      fi
    else 
      USE_THREADS=""
      if test -z "$LIBPTHREAD"; then
        KDE_CHECK_COMPILER_FLAG(pthread, [USE_THREADS="-pthread"] )
      fi
    fi

    AH_VERBATIM(__svr_define, [
#if defined(__SVR4) && !defined(__svr4__)
#define __svr4__ 1
#endif
])
    case $host_os in
 	solaris*)
		KDE_CHECK_COMPILER_FLAG(mt, [USE_THREADS="-mt"])
                CXXFLAGS="$CXXFLAGS -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DUSE_SOLARIS -DSVR4"
    		;;
        freebsd*)
                CXXFLAGS="$CXXFLAGS -D_THREAD_SAFE"
                ;;
        aix*)
                CXXFLAGS="$CXXFLAGS -D_THREAD_SAFE"
                LIBPTHREAD="$LIBPTHREAD -lc_r"
                ;;
        linux*) CXXFLAGS="$CXXFLAGS -D_REENTRANT"
                if test "$CXX" = "KCC"; then
                  CXXFLAGS="$CXXFLAGS --thread_safe"
                  NOOPT_CXXFLAGS="$NOOPT_CXXFLAGS --thread_safe"
                fi
                ;;
	*)
		;;
    esac
    AC_SUBST(USE_THREADS)
    AC_SUBST(LIBPTHREAD)
])

AC_DEFUN([KDE_CHECK_THREADING],
[
  AC_REQUIRE([KDE_CHECK_LIBPTHREAD])
  AC_REQUIRE([KDE_CHECK_PTHREAD_OPTION])
  dnl default is yes if libpthread is found and no if no libpthread is available
  if test -z "$LIBPTHREAD"; then
    if test -z "$USE_THREADS"; then
      kde_check_threading_default=no
    else
      kde_check_threading_default=yes
    fi
  else
    kde_check_threading_default=yes
  fi

  AC_ARG_ENABLE(
    threading,
    [  --disable-threading     disables threading even if libpthread found ],
    kde_use_threading=$enableval, kde_use_threading=$kde_check_threading_default)

  if test "x$kde_use_threading" = "xyes"; then
    AC_DEFINE(HAVE_LIBPTHREAD, 1, [Define if you have a working libpthread])
  fi
])

AC_DEFUN([KDE_CHECK_STL_SGI],
[
    AC_MSG_CHECKING([if STL implementation is SGI like])
    AC_CACHE_VAL(kde_cv_stl_type_sgi,
    [
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <string>
using namespace std;
]],[[
  string astring="Hallo Welt.";
  astring.erase(0, 6); // now astring is "Welt"
  return 0;
]])], kde_cv_stl_type_sgi=yes,
   kde_cv_stl_type_sgi=no)
])

   AC_MSG_RESULT($kde_cv_stl_type_sgi)

   if test "$kde_cv_stl_type_sgi" = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_SGI_STL, 1, [Define if you have a STL implementation by SGI])
   fi
])

AC_DEFUN([KDE_CHECK_STL_HP],
[
    AC_MSG_CHECKING([if STL implementation is HP like])
    AC_CACHE_VAL(kde_cv_stl_type_hp,
    [
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <string>
using namespace std;
]],[[
  string astring="Hello World";
  astring.remove(0, 6); // now astring is "World"
  return 0;
]])], kde_cv_stl_type_hp=yes,
   kde_cv_stl_type_hp=no)
])
   AC_MSG_RESULT($kde_cv_stl_type_hp)

   if test "$kde_cv_stl_type_hp" = "yes"; then
	AC_DEFINE_UNQUOTED(HAVE_HP_STL, 1, [Define if you have a STL implementation by HP])
   fi
])

AC_DEFUN([KDE_CHECK_STL],
[
    AC_LANG_PUSH(C++)
    ac_save_CXXFLAGS="$CXXFLAGS"
    CXXFLAGS="`echo $CXXFLAGS | sed s/-fno-exceptions//`"
    KDE_CHECK_STL_SGI

    if test "$kde_cv_stl_type_sgi" = "no"; then
       KDE_CHECK_STL_HP

       if test "$kde_cv_stl_type_hp" = "no"; then
         AC_MSG_ERROR("no known STL type found - did you forget to install libstdc++[-devel] ?")
       fi
    fi

    CXXFLAGS="$ac_save_CXXFLAGS"
    AC_LANG_POP([C++])
])

AC_DEFUN([AC_FIND_QIMGIO],
   [AC_REQUIRE([AC_FIND_JPEG])
AC_REQUIRE([KDE_CHECK_EXTRA_LIBS])
AC_MSG_CHECKING([for qimgio])
AC_CACHE_VAL(ac_cv_lib_qimgio,
[
AC_LANG_PUSH(C++)
ac_save_LIBS="$LIBS"
ac_save_CXXFLAGS="$CXXFLAGS"
LIBS="$all_libraries -lqimgio -lpng -lz $LIBJPEG $LIBQT"
CXXFLAGS="$CXXFLAGS -I$qt_incdir $all_includes"
AC_TRY_RUN(dnl
[
#include <qimageio.h>
#include <qstring.h>
int main() {
		QString t = "hallo";
		t.fill('t');
		qInitImageIO();
}
],
            ac_cv_lib_qimgio=yes,
            ac_cv_lib_qimgio=no,
	    ac_cv_lib_qimgio=no)
LIBS="$ac_save_LIBS"
CXXFLAGS="$ac_save_CXXFLAGS"
AC_LANG_POP(C++)
])dnl
if eval "test \"`echo $ac_cv_lib_qimgio`\" = yes"; then
  LIBQIMGIO="-lqimgio -lpng -lz $LIBJPEG"
  AC_MSG_RESULT(yes)
  AC_DEFINE_UNQUOTED(HAVE_QIMGIO, 1, [Define if you have the Qt extension qimgio available])
  AC_SUBST(LIBQIMGIO)
else
  AC_MSG_RESULT(not found)
fi
])

AC_DEFUN([KDE_CHECK_ANSI],
[
])

AC_DEFUN([KDE_CHECK_INSURE],
[
  AC_ARG_ENABLE(insure, [  --enable-insure             use insure++ for debugging [default=no]],
  [
  if test $enableval = "no"; dnl
	then ac_use_insure="no"
	else ac_use_insure="yes"
   fi
  ], [ac_use_insure="no"])

  AC_MSG_CHECKING(if we will use Insure++ to debug)
  AC_MSG_RESULT($ac_use_insure)
  if test "$ac_use_insure" = "yes"; dnl
       then CC="insure"; CXX="insure"; dnl CFLAGS="$CLAGS -fno-rtti -fno-exceptions "????
   fi
])

AC_DEFUN([AM_DISABLE_LIBRARIES],
[
    AC_PROVIDE([AM_ENABLE_STATIC])
    AC_PROVIDE([AM_ENABLE_SHARED])
    enable_static=no
    enable_shared=yes
])


AC_DEFUN([AC_CHECK_UTMP_FILE],
[
    AC_MSG_CHECKING([for utmp file])

    AC_CACHE_VAL(kde_cv_utmp_file,
    [
    kde_cv_utmp_file=no

    for ac_file in    \
                      \
	/var/run/utmp \
	/var/adm/utmp \
	/etc/utmp     \
     ; \
    do
     if test -r "$ac_file"; then
       kde_cv_utmp_file=$ac_file
       break
     fi
    done
    ])

    if test "$kde_cv_utmp_file" != "no"; then
	AC_DEFINE_UNQUOTED(UTMP, "$kde_cv_utmp_file", [Define the file for utmp entries])
	$1
	AC_MSG_RESULT($kde_cv_utmp_file)
    else
    	$2
	AC_MSG_RESULT([non found])
    fi
])


AC_DEFUN([KDE_CREATE_SUBDIRSLIST],
[

DO_NOT_COMPILE="$DO_NOT_COMPILE CVS debian bsd-port admin"

if test ! -s $srcdir/subdirs; then
  dnl Note: Makefile.common creates subdirs, so this is just a fallback
  TOPSUBDIRS=""
  files=`cd $srcdir && ls -1`
  dirs=`for i in $files; do if test -d $i; then echo $i; fi; done`
  for i in $dirs; do
    echo $i >> $srcdir/subdirs
  done
fi

if test -s $srcdir/inst-apps; then
  ac_topsubdirs="`cat $srcdir/inst-apps`"
else
  ac_topsubdirs="`cat $srcdir/subdirs`"
fi

for i in $ac_topsubdirs; do
  AC_MSG_CHECKING([if $i should be compiled])
  if test -d $srcdir/$i; then
    install_it="yes"
    for j in $DO_NOT_COMPILE; do
      if test $i = $j; then
        install_it="no"
      fi
    done
  else
    install_it="no"
  fi
  AC_MSG_RESULT($install_it)
  vari=`echo $i | sed -e 's,[[-+.]],_,g'`
  if test $install_it = "yes"; then
    TOPSUBDIRS="$TOPSUBDIRS $i"
    eval "$vari""_SUBDIR_included=yes"
  else
    eval "$vari""_SUBDIR_included=no"
  fi
done

AC_SUBST(TOPSUBDIRS)
])

AC_DEFUN([KDE_CHECK_NAMESPACES],
[
AC_MSG_CHECKING(whether C++ compiler supports namespaces)
AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[
namespace Foo {
  extern int i;
  namespace Bar {
    extern int i;
  }
}

int Foo::i = 0;
int Foo::Bar::i = 1;
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_NAMESPACES)
], [
AC_MSG_RESULT(no)
])
AC_LANG_POP([C++])
])

AC_DEFUN([KDE_CHECK_NEWLIBS],
[

])

dnl ------------------------------------------------------------------------
dnl Check for S_ISSOCK macro. Doesn't exist on Unix SCO. faure@kde.org
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_CHECK_S_ISSOCK],
[
AC_MSG_CHECKING(for S_ISSOCK)
AC_CACHE_VAL(ac_cv_have_s_issock,
[
AC_LANG_PUSH(C)
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <sys/stat.h>
]],
[[
struct stat buff;
int b = S_ISSOCK( buff.st_mode );
]])],
ac_cv_have_s_issock=yes,
ac_cv_have_s_issock=no)
AC_LANG_POP([C])
])
AC_MSG_RESULT($ac_cv_have_s_issock)
if test "$ac_cv_have_s_issock" = "yes"; then
  AC_DEFINE_UNQUOTED(HAVE_S_ISSOCK, 1, [Define if sys/stat.h declares S_ISSOCK.])
fi

AH_VERBATIM(_ISSOCK,
[
#ifndef HAVE_S_ISSOCK
#define HAVE_S_ISSOCK
#define S_ISSOCK(mode) (1==0)
#endif
])

])

dnl ------------------------------------------------------------------------
dnl Check for MAXPATHLEN macro, defines KDEMAXPATHLEN. faure@kde.org
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([AC_CHECK_KDEMAXPATHLEN],
[
AC_MSG_CHECKING(for MAXPATHLEN)
AC_CACHE_VAL(ac_cv_maxpathlen,
[
AC_LANG(C)
cat > conftest.$ac_ext <<EOF
#ifdef STDC_HEADERS
# include <stdlib.h>
#endif
#include <stdio.h>
#include <sys/param.h>
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

KDE_HELLO MAXPATHLEN

EOF

ac_try="$ac_cpp conftest.$ac_ext 2>/dev/null | grep '^KDE_HELLO' >conftest.out"

if AC_TRY_EVAL(ac_try) && test -s conftest.out; then
    ac_cv_maxpathlen=`sed 's#KDE_HELLO ##' conftest.out`
else
    ac_cv_maxpathlen=1024
fi

rm conftest.*

])
AC_MSG_RESULT($ac_cv_maxpathlen)
AC_DEFINE_UNQUOTED(KDEMAXPATHLEN,$ac_cv_maxpathlen, [Define a safe value for MAXPATHLEN] )
])

dnl -------------------------------------------------------------------------
dnl See if the compiler supports a template repository         bero@redhat.de
dnl -------------------------------------------------------------------------
AC_DEFUN([KDE_COMPILER_REPO],
[
  REPO=""
  NOREPO=""

  KDE_CHECK_COMPILER_FLAG(frepo,
   [
     REPO="-frepo"
     NOREPO="-fno-repo"
   ])

  if test -z "$REPO"; then
  KDE_CHECK_COMPILER_FLAG(instances=explicit,
  [
     REPO="-instances=explicit"
     NOREPO="-instances=extern"
  ])
  fi

  if test -n "$REPO"; then
     AC_DEFINE_UNQUOTED(HAVE_TEMPLATE_REPOSITORY, 1,
		[C++ compiler supports template repository])
     $1
  fi

  AC_SUBST(REPO)
  AC_SUBST(NOREPO)
])

AC_DEFUN([KDE_CHECK_HEADER],
[
   AC_LANG_PUSH(C++)
   kde_safe_cppflags=$CPPFLAGS
   CPPFLAGS="$CPPFLAGS $all_includes"
   AC_CHECK_HEADER($1, $2, $3, $4)
   CPPFLAGS=$kde_safe_cppflags
   AC_LANG_POP([C++])
])

AC_DEFUN([KDE_CHECK_HEADERS],
[
   AH_CHECK_HEADERS([$1])
   AC_LANG_PUSH(C++)
   kde_safe_cppflags=$CPPFLAGS
   CPPFLAGS="$CPPFLAGS $all_includes"
   AC_CHECK_HEADERS([$1], [$2], [$3], [$4])
   CPPFLAGS=$kde_safe_cppflags
   AC_LANG_POP([C++])
])

AC_DEFUN([KDE_FAST_CONFIGURE],
[
  dnl makes configure fast (needs perl)
  AC_ARG_ENABLE(fast-perl, [  --disable-fast-perl     disable fast Makefile generation (needs perl)],
      with_fast_perl=$enableval, with_fast_perl=yes)
])

AC_DEFUN([KDE_CONF_FILES],
[
  val=
  if test -f $srcdir/configure.files ; then
    val=`sed -e 's%^%\$(top_srcdir)/%' $srcdir/configure.files`
  fi
  CONF_FILES=
  if test -n "$val" ; then
    for i in $val ; do
      CONF_FILES="$CONF_FILES $i"
    done
  fi
  AC_SUBST(CONF_FILES)
])dnl

pushdef([AC_PROG_INSTALL],
[
  dnl our own version, testing for a -p flag
  popdef([AC_PROG_INSTALL])
  dnl as AC_PROG_INSTALL works as it works we first have
  dnl to save if the user didn't specify INSTALL, as the
  dnl autoconf one overwrites INSTALL and we have no chance to find
  dnl out afterwards
  test -n "$INSTALL" && kde_save_INSTALL_given=$INSTALL
  test -n "$INSTALL_PROGRAM" && kde_save_INSTALL_PROGRAM_given=$INSTALL_PROGRAM
  test -n "$INSTALL_SCRIPT" && kde_save_INSTALL_SCRIPT_given=$INSTALL_SCRIPT
  AC_PROG_INSTALL

  if test -z "$kde_save_INSTALL_given" ; then
    # OK, user hasn't given any INSTALL, autoconf found one for us
    # now we test, if it supports the -p flag
    AC_MSG_CHECKING(for -p flag to install)
    rm -f confinst.$$.* > /dev/null 2>&1
    echo "Testtest" > confinst.$$.orig
    ac_res=no
    if ${INSTALL} -p confinst.$$.orig confinst.$$.new > /dev/null 2>&1 ; then
      if test -f confinst.$$.new ; then
        # OK, -p seems to do no harm to install
	INSTALL="${INSTALL} -p"
	ac_res=yes
      fi
    fi
    rm -f confinst.$$.*
    AC_MSG_RESULT($ac_res)
  fi
  dnl the following tries to resolve some signs and wonders coming up
  dnl with different autoconf/automake versions
  dnl e.g.:
  dnl  *automake 1.4 install-strip sets A_M_INSTALL_PROGRAM_FLAGS to -s
  dnl   and has INSTALL_PROGRAM = @INSTALL_PROGRAM@ $(A_M_INSTALL_PROGRAM_FLAGS)
  dnl   it header-vars.am, so there the actual INSTALL_PROGRAM gets the -s
  dnl  *automake 1.4a (and above) use INSTALL_STRIP_FLAG and only has
  dnl   INSTALL_PROGRAM = @INSTALL_PROGRAM@ there, but changes the
  dnl   install-@DIR@PROGRAMS targets to explicitly use that flag
  dnl  *autoconf 2.13 is dumb, and thinks it can use INSTALL_PROGRAM as
  dnl   INSTALL_SCRIPT, which breaks with automake <= 1.4
  dnl  *autoconf >2.13 (since 10.Apr 1999) has not that failure
  dnl  *sometimes KDE does not use the install-@DIR@PROGRAM targets from
  dnl   automake (due to broken Makefile.am or whatever) to install programs,
  dnl   and so does not see the -s flag in automake > 1.4
  dnl to clean up that mess we:
  dnl  +set INSTALL_PROGRAM to use INSTALL_STRIP_FLAG
  dnl   which cleans KDE's program with automake > 1.4;
  dnl  +set INSTALL_SCRIPT to only use INSTALL, to clean up autoconf's problems
  dnl   with automake<=1.4
  dnl  note that dues to this sometimes two '-s' flags are used (if KDE
  dnl   properly uses install-@DIR@PROGRAMS, but I don't care
  dnl
  dnl And to all this comes, that I even can't write in comments variable
  dnl  names used by automake, because it is so stupid to think I wanted to
  dnl  _use_ them, therefor I have written A_M_... instead of AM_
  dnl hmm, I wanted to say something ... ahh yes: Arghhh.

  if test -z "$kde_save_INSTALL_PROGRAM_given" ; then
    INSTALL_PROGRAM='${INSTALL} $(INSTALL_STRIP_FLAG)'
  fi
  if test -z "$kde_save_INSTALL_SCRIPT_given" ; then
    INSTALL_SCRIPT='${INSTALL}'
  fi
])dnl

AC_DEFUN([KDE_LANG_CPLUSPLUS],
[AC_LANG_CPLUSPLUS
ac_link='rm -rf SunWS_cache; ${CXX-g++} -o conftest${ac_exeext} $CXXFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext $LIBS 1>&AS_MESSAGE_LOG_FD()'
pushdef([AC_LANG_CPLUSPLUS], [popdef([AC_LANG_CPLUSPLUS]) KDE_LANG_CPLUSPLUS])
])

pushdef([AC_LANG_CPLUSPLUS],
[popdef([AC_LANG_CPLUSPLUS])
KDE_LANG_CPLUSPLUS
])

AC_DEFUN([KDE_CHECK_LONG_LONG],
[
AC_MSG_CHECKING(for long long)
AC_CACHE_VAL(kde_cv_c_long_long,
[
  AC_LANG_PUSH(C++)
   AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[
  long long foo = 0;
  foo = foo+1;
  ]])],
  kde_cv_c_long_long=yes, kde_cv_c_long_long=no)
  AC_LANG_POP([C++])
])
AC_MSG_RESULT($kde_cv_c_long_long)
if test "$kde_cv_c_long_long" = yes; then
   AC_DEFINE(HAVE_LONG_LONG, 1, [Define if you have long long as datatype])
fi
])

AC_DEFUN([KDE_CHECK_LIB],
[
     kde_save_LDFLAGS="$LDFLAGS"
     dnl AC_CHECK_LIB modifies LIBS, so save it here
     kde_save_LIBS="$LIBS"
     LDFLAGS="$LDFLAGS $all_libraries"
     case $host_os in
      aix*) LDFLAGS="-brtl $LDFLAGS"
	test "$GCC" = yes && LDFLAGS="-Wl,$LDFLAGS"
	;;
     esac
     AC_CHECK_LIB($1, $2, $3, $4, $5)
     LDFLAGS="$kde_save_LDFLAGS"
     LIBS="$kde_save_LIBS"
])

AC_DEFUN([KDE_JAVA_PREFIX],
[
	dir=`dirname "$1"`
	base=`basename "$1"`
	list=`ls -1 $dir 2> /dev/null`
	for entry in $list; do 
		if test -d $dir/$entry/bin; then
			case $entry in
			   $base)
				javadirs="$javadirs $dir/$entry/bin"
				;;
			esac
		elif test -d $dir/$entry/jre/bin; then
			case $entry in
			   $base)
				javadirs="$javadirs $dir/$entry/jre/bin"
				;;
			esac
		fi
	done
])

dnl KDE_CHEC_JAVA_DIR(onlyjre)
AC_DEFUN([KDE_CHECK_JAVA_DIR],
[

AC_ARG_WITH(java,
[  --with-java=javadir     use java installed in javadir, --without-java disables ],
[  ac_java_dir=$withval
], ac_java_dir=""
)

AC_MSG_CHECKING([for Java])

dnl at this point ac_java_dir is either a dir, 'no' to disable, or '' to say look in $PATH
if test "x$ac_java_dir" = "xno"; then
   kde_java_bindir=no
   kde_java_includedir=no
   kde_java_libjvmdir=no
   kde_java_libhpidir=no
else
  if test "x$ac_java_dir" = "x"; then
     
     
      dnl No option set -> look in $PATH
      KDE_JAVA_PREFIX(/usr/j2se)
      KDE_JAVA_PREFIX(/usr/j*dk*)
      KDE_JAVA_PREFIX(/usr/lib/j*dk*)
      KDE_JAVA_PREFIX(/opt/j*sdk*)
      KDE_JAVA_PREFIX(/usr/lib/java*)
      KDE_JAVA_PREFIX(/usr/java*)
      KDE_JAVA_PREFIX(/usr/java/j*dk*)
      KDE_JAVA_PREFIX(/usr/java/j*re*)
      KDE_JAVA_PREFIX(/usr/lib/SunJava2*)
      KDE_JAVA_PREFIX(/usr/lib/SunJava*)
      KDE_JAVA_PREFIX(/usr/lib/IBMJava2*)
      KDE_JAVA_PREFIX(/usr/lib/IBMJava*)
      KDE_JAVA_PREFIX(/opt/java*)
    
      kde_cv_path="NONE"
      kde_save_IFS=$IFS
      IFS=':'
      for dir in $PATH; do
	  javadirs="$javadirs $dir"
      done
      IFS=$kde_save_IFS
      jredirs=

      for dir in $javadirs; do
	  if test ! -d $dir; then break; fi
          if test -x "$dir/java"; then
                libjvmdir=`find $dir/.. -name libjvm.so | sed 's,libjvm.so,,'|head -n 1`
		if test ! -f $libjvmdir/libjvm.so; then continue; fi
		jredirs="$jredirs $dir"
	  fi
      done

      JAVAC=
      JAVA=
      kde_java_bindir=no
      for dir in $jredirs; do
	  JAVA="$dir/java"
	  kde_java_bindir=$dir
	  if test -x "$dir/javac"; then
		JAVAC="$dir/javac"
                break
	  fi
      done

      if test -n "$JAVAC"; then
          dnl this substitution might not work - well, we test for jni.h below
          kde_java_includedir=`echo $JAVAC | sed -e 's,bin/javac$,include/,'`
      else
          kde_java_includedir=no
      fi
  else
    dnl config option set
    kde_java_bindir=$ac_java_dir/bin
    if test -x $ac_java_dir/bin/java && test ! -x $ac_java_dir/bin/javac; then
	kde_java_includedir=no
    else
        kde_java_includedir=$ac_java_dir/include
    fi
  fi
fi

dnl At this point kde_java_bindir and kde_java_includedir are either set or "no"
if test "x$kde_java_bindir" != "xno"; then

  dnl Look for libjvm.so
  kde_java_libjvmdir=`find $kde_java_bindir/.. -name libjvm.so | sed 's,libjvm.so,,'|head -n 1`
  dnl Look for libhpi.so and avoid green threads
  kde_java_libhpidir=`find $kde_java_bindir/.. -name libhpi.so | grep -v green | sed 's,libhpi.so,,' | head -n 1`

  dnl Now check everything's fine under there
  dnl the include dir is our flag for having the JDK
  if test -d "$kde_java_includedir"; then
    if test ! -x "$kde_java_bindir/javac"; then
      AC_MSG_ERROR([javac not found under $kde_java_bindir - it seems you passed a wrong --with-java.])
    fi
    if test ! -x "$kde_java_bindir/javah"; then
      AC_MSG_ERROR([javah not found under $kde_java_bindir. javac was found though! Use --with-java or --without-java.])
    fi
    if test ! -x "$kde_java_bindir/jar"; then
      AC_MSG_ERROR([jar not found under $kde_java_bindir. javac was found though! Use --with-java or --without-java.])
    fi
    if test ! -r "$kde_java_includedir/jni.h"; then
      AC_MSG_ERROR([jni.h not found under $kde_java_includedir. Use --with-java or --without-java.])
    fi

    jni_includes="-I$kde_java_includedir"
    dnl Strange thing, jni.h requires jni_md.h which is under genunix here..
    dnl and under linux here..
    test -d "$kde_java_includedir/linux" && jni_includes="$jni_includes -I$kde_java_includedir/linux"
    test -d "$kde_java_includedir/solaris" && jni_includes="$jni_includes -I$kde_java_includedir/solaris"
    test -d "$kde_java_includedir/genunix" && jni_includes="$jni_includes -I$kde_java_includedir/genunix"

  else
    JAVAC=
    jni_includes=
  fi

  if test ! -r "$kde_java_libjvmdir/libjvm.so"; then
     AC_MSG_ERROR([libjvm.so not found under $kde_java_libjvmdir. Use --without-java.])
  fi 

  if test ! -x "$kde_java_bindir/java"; then
      AC_MSG_ERROR([java not found under $kde_java_bindir. javac was found though! Use --with-java or --without-java.])
  fi

  if test ! -r "$kde_java_libhpidir/libhpi.so"; then
    AC_MSG_ERROR([libhpi.so not found under $kde_java_libhpidir. Use --without-java.])
  fi

  if test -n "$jni_includes"; then
    dnl Check for JNI version
    AC_LANG_PUSH(C++)
    ac_cxxflags_safe="$CXXFLAGS"
    CXXFLAGS="$CXXFLAGS $all_includes $jni_includes"

    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  #include <jni.h>
	      ]],
	      [[
  #ifndef JNI_VERSION_1_2
  Syntax Error
  #endif
	      ]])],[ kde_jni_works=yes ],
	      [ kde_jni_works=no ])

    if test $kde_jni_works = no; then
      AC_MSG_ERROR([Incorrect version of $kde_java_includedir/jni.h.
		    You need to have Java Development Kit (JDK) version 1.2. 

		    Use --with-java to specify another location.
		    Use --without-java to configure without java support.
		    Or download a newer JDK and try again. 
		    See e.g. http://java.sun.com/products/jdk/1.2 ])
    fi

    CXXFLAGS="$ac_cxxflags_safe"    
    AC_LANG_POP([C++])

    dnl All tests ok, inform and subst the variables

    JAVAC=$kde_java_bindir/javac
    JAVAH=$kde_java_bindir/javah
    JAR=$kde_java_bindir/jar
    AC_DEFINE_UNQUOTED(PATH_JAVA, "$kde_java_bindir/java", [Define where your java executable is])
    JVMLIBS="-L$kde_java_libjvmdir -ljvm -L$kde_java_libhpidir -lhpi"
    AC_MSG_RESULT([java JDK in $kde_java_bindir])

  else
      AC_DEFINE_UNQUOTED(PATH_JAVA, "$kde_java_bindir/java", [Define where your java executable is])
      AC_MSG_RESULT([java JRE in $kde_java_bindir])
  fi
else # no
  AC_MSG_RESULT([none found])
fi

AC_SUBST(JAVAC)
AC_SUBST(JAVAH)
AC_SUBST(JAR)
AC_SUBST(JVMLIBS)
AC_SUBST(jni_includes)

# for backward compat
kde_cv_java_includedir=$kde_java_includedir
kde_cv_java_bindir=$kde_java_bindir
])

dnl this is a redefinition of autoconf 2.5x's AC_FOREACH.
dnl When the argument list becomes big, as in KDE for AC_OUTPUT in
dnl big packages, m4_foreach is dog-slow.  So use our own version of
dnl it.  (matz@kde.org)
m4_define([mm_foreach],
[m4_pushdef([$1])_mm_foreach($@)m4_popdef([$1])])
m4_define([mm_car], [[$1]])
m4_define([mm_car2], [[$@]])
m4_define([_mm_foreach],
[m4_if(m4_quote($2), [], [],
       [m4_define([$1], [mm_car($2)])$3[]_mm_foreach([$1],
                                                     mm_car2(m4_shift($2)),
                                                     [$3])])])
m4_define([AC_FOREACH],
[mm_foreach([$1], m4_split(m4_normalize([$2])), [$3])])

AC_DEFUN([KDE_NEED_FLEX],
[
kde_libs_safe=$LIBS
LIBS="$LIBS $USER_LDFLAGS"
AM_PROG_LEX
LIBS=$kde_libs_safe
if test -z "$LEXLIB"; then
    AC_MSG_ERROR([You need to have flex installed.])
fi
AC_SUBST(LEXLIB)
])

AC_DEFUN([AC_PATH_QTOPIA],
[
  dnl TODO: use AC_CACHE_VAL

  if test -z "$1"; then
    qtopia_minver_maj=1
    qtopia_minver_min=5
    qtopia_minver_pat=0
  else
    qtopia_minver_maj=`echo "$1" | sed -e "s/^\(.*\)\..*\..*$/\1/"`
    qtopia_minver_min=`echo "$1" | sed -e "s/^.*\.\(.*\)\..*$/\1/"`
    qtopia_minver_pat=`echo "$1" | sed -e "s/^.*\..*\.\(.*\)$/\1/"`
  fi

  qtopia_minver="$qtopia_minver_maj$qtopia_minver_min$qtopia_minver_pat"
  qtopia_minverstr="$qtopia_minver_maj.$qtopia_minver_min.$qtopia_minver_pat"

  dnl mrj, don't do that   AC_REQUIRE([AC_PATH_QT])

  AC_MSG_CHECKING([for Qtopia])

  LIB_QTOPIA="-lqpe"
  AC_SUBST(LIB_QTOPIA)

  kde_qtopia_dirs="$QPEDIR /opt/Qtopia"

  ac_qtopia_incdir=NO

  AC_ARG_WITH(qtopia-dir,
              [  --with-qtopia-dir=DIR   where the root of Qtopia is installed ],
              [  ac_qtopia_incdir="$withval"/include] ) 
  
  qtopia_incdirs=""
  for dir in $kde_qtopia_dirs; do
    qtopia_incdirs="$qtopia_incdirs $dir/include"
  done

  if test ! "$ac_qtopia_incdir" = "NO"; then
    qtopia_incdirs="$ac_qtopia_incdir $qtopia_incdirs"
  fi

  qtopia_incdir=""
  AC_FIND_FILE(qpe/qpeapplication.h, $qtopia_incdirs, qtopia_incdir)
  ac_qtopia_incdir="$qtopia_incdir"

  if test -z "$qtopia_incdir"; then
    AC_MSG_ERROR([Cannot find Qtopia headers. Please check your installation.])
  fi

  qtopia_ver_maj=`cat $qtopia_incdir/qpe/version.h | sed -n -e 's,.*QPE_VERSION "\(.*\)\..*\..*".*,\1,p'`;
  qtopia_ver_min=`cat $qtopia_incdir/qpe/version.h | sed -n -e 's,.*QPE_VERSION ".*\.\(.*\)\..*".*,\1,p'`;
  qtopia_ver_pat=`cat $qtopia_incdir/qpe/version.h | sed -n -e 's,.*QPE_VERSION ".*\..*\.\(.*\)".*,\1,p'`;

  qtopia_ver="$qtopia_ver_maj$qtopia_ver_min$qtopia_ver_pat"
  qtopia_verstr="$qtopia_ver_maj.$qtopia_ver_min.$qtopia_ver_pat"
  if test "$qtopia_ver" -lt "$qtopia_minver"; then
    AC_MSG_ERROR([found Qtopia version $qtopia_verstr but version $qtopia_minverstr
is required.])
  fi

  AC_LANG_PUSH(C++)

  ac_cxxflags_safe="$CXXFLAGS"
  ac_ldflags_safe="$LDFLAGS"
  ac_libs_safe="$LIBS"

  CXXFLAGS="$CXXFLAGS -I$qtopia_incdir $all_includes"
  LDFLAGS="$LDFLAGS $QT_LDFLAGS $all_libraries $USER_LDFLAGS $KDE_MT_LDFLAGS"
  LIBS="$LIBS $LIB_QTOPIA $LIBQT"

  cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include <qpe/qpeapplication.h>
#include <qpe/version.h>

int main( int argc, char **argv )
{
    QPEApplication app( argc, argv );
    return 0;
}
EOF

  if AC_TRY_EVAL(ac_link) && test -s conftest; then
    rm -f conftest*
  else
    rm -f conftest*
    AC_MSG_ERROR([Cannot link small Qtopia Application. For more details look at
the end of config.log])
  fi

  CXXFLAGS="$ac_cxxflags_safe"
  LDFLAGS="$ac_ldflags_safe"
  LIBS="$ac_libs_safe"

  AC_LANG_POP([C++])

  QTOPIA_INCLUDES="-I$qtopia_incdir"
  AC_SUBST(QTOPIA_INCLUDES)

  AC_MSG_RESULT([found version $qtopia_verstr with headers at $qtopia_incdir])
])


AC_DEFUN([KDE_INIT_DOXYGEN],
[
AC_MSG_CHECKING([for Qt docs])
kde_qtdir=
if test "${with_qt_dir+set}" = set; then
  kde_qtdir="$with_qt_dir"
fi

AC_FIND_FILE(qsql.html, [ $kde_qtdir/doc/html $QTDIR/doc/html /usr/share/doc/packages/qt3/html /usr/lib/qt/doc /usr/lib/qt3/doc /usr/lib/qt3/doc/html /usr/doc/qt3/html /usr/doc/qt3 /usr/share/doc/qt3-doc /usr/share/qt3/doc/html ], QTDOCDIR)
AC_MSG_RESULT($QTDOCDIR)

AC_SUBST(QTDOCDIR)

KDE_FIND_PATH(dot, DOT, [], [])
if test -n "$DOT"; then
  KDE_HAVE_DOT="YES"
else
  KDE_HAVE_DOT="NO"
fi
AC_SUBST(KDE_HAVE_DOT)
KDE_FIND_PATH(doxygen, DOXYGEN, [], [])
AC_SUBST(DOXYGEN)

DOXYGEN_PROJECT_NAME="$1"
DOXYGEN_PROJECT_NUMBER="$2"
AC_SUBST(DOXYGEN_PROJECT_NAME)
AC_SUBST(DOXYGEN_PROJECT_NUMBER)

KDE_HAS_DOXYGEN=no
if test -n "$DOXYGEN" && test -x "$DOXYGEN" && test -f $QTDOCDIR/qsql.html; then
  KDE_HAS_DOXYGEN=yes
fi
AC_SUBST(KDE_HAS_DOXYGEN)

])


AC_DEFUN([AC_FIND_BZIP2],
[
AC_MSG_CHECKING([for bzDecompress in libbz2])
AC_CACHE_VAL(ac_cv_lib_bzip2,
[
AC_LANG(C)
kde_save_LIBS="$LIBS"
LIBS="$all_libraries $USER_LDFLAGS -lbz2 $LIBSOCKET"
kde_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $all_includes $USER_INCLUDES"
 AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#define BZ_NO_STDIO
#include<bzlib.h>
]],
            [[ bz_stream s; (void) bzDecompress(&s); ]])],
            eval "ac_cv_lib_bzip2='-lbz2'",
            eval "ac_cv_lib_bzip2=no")
LIBS="$kde_save_LIBS"
CFLAGS="$kde_save_CFLAGS"
])dnl
AC_MSG_RESULT($ac_cv_lib_bzip2)

if test ! "$ac_cv_lib_bzip2" = no; then
  BZIP2DIR=bzip2

  LIBBZ2="$ac_cv_lib_bzip2"
  AC_SUBST(LIBBZ2)

else

   cxx_shared_flag=
   ld_shared_flag=
   KDE_CHECK_COMPILER_FLAG(shared, [
	ld_shared_flag="-shared"
   ])
   KDE_CHECK_COMPILER_FLAG(fPIC, [
        cxx_shared_flag="-fPIC"
   ])

   AC_MSG_CHECKING([for BZ2_bzDecompress in (shared) libbz2])
   AC_CACHE_VAL(ac_cv_lib_bzip2_prefix,
   [
   AC_LANG_CPLUSPLUS
   kde_save_LIBS="$LIBS"
   LIBS="$all_libraries $USER_LDFLAGS $ld_shared_flag -lbz2 $LIBSOCKET"
   kde_save_CXXFLAGS="$CXXFLAGS"
   CXXFLAGS="$CFLAGS $cxx_shared_flag $all_includes $USER_INCLUDES"

	 AC_LINK_IFELSE([AC_LANG_PROGRAM([[		
   #define BZ_NO_STDIO
   #include<bzlib.h>
   ]],
               [[ bz_stream s; (void) BZ2_bzDecompress(&s); ]])],
               eval "ac_cv_lib_bzip2_prefix='-lbz2'",
               eval "ac_cv_lib_bzip2_prefix=no")
   LIBS="$kde_save_LIBS"
   CXXFLAGS="$kde_save_CXXFLAGS"
   ])dnl

   AC_MSG_RESULT($ac_cv_lib_bzip2_prefix)
   


   if test ! "$ac_cv_lib_bzip2_prefix" = no; then
     BZIP2DIR=bzip2
    
     LIBBZ2="$ac_cv_lib_bzip2_prefix"
     AC_SUBST(LIBBZ2)

     AC_DEFINE(NEED_BZ2_PREFIX, 1, [Define if the libbz2 functions need the BZ2_ prefix])
   dnl else, we just ignore this
   fi

fi
AM_CONDITIONAL(include_BZIP2, test -n "$BZIP2DIR")
])

dnl ------------------------------------------------------------------------
dnl Try to find the SSL headers and libraries.
dnl $(SSL_LDFLAGS) will be -Lsslliblocation (if needed)
dnl and $(SSL_INCLUDES) will be -Isslhdrlocation (if needed)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([KDE_CHECK_SSL],
[
LIBSSL="-lssl -lcrypto"
AC_REQUIRE([KDE_CHECK_LIB64])

ac_ssl_includes=NO ac_ssl_libraries=NO
ssl_libraries=""
ssl_includes=""
AC_ARG_WITH(ssl-dir,
    [  --with-ssl-dir=DIR      where the root of OpenSSL is installed],
    [  ac_ssl_includes="$withval"/include
       ac_ssl_libraries="$withval"/lib$kdelibsuff
    ])

want_ssl=yes
AC_ARG_WITH(ssl,
    [  --without-ssl           disable SSL checks],
    [want_ssl=$withval])

if test $want_ssl = yes; then

AC_MSG_CHECKING(for OpenSSL)

AC_CACHE_VAL(ac_cv_have_ssl,
[#try to guess OpenSSL locations
  
  ssl_incdirs="/usr/include /usr/local/include /usr/ssl/include /usr/local/ssl/include $prefix/include $kde_extra_includes"
  ssl_incdirs="$ac_ssl_includes $ssl_incdirs"
  AC_FIND_FILE(openssl/ssl.h, $ssl_incdirs, ssl_incdir)
  ac_ssl_includes="$ssl_incdir"

  ssl_libdirs="/usr/lib$kdelibsuff /usr/local/lib$kdelibsuff /usr/ssl/lib$kdelibsuff /usr/local/ssl/lib$kdelibsuff $libdir $prefix/lib$kdelibsuff $exec_prefix/lib$kdelibsuff $kde_extra_libs"
  if test ! "$ac_ssl_libraries" = "NO"; then
    ssl_libdirs="$ac_ssl_libraries $ssl_libdirs"
  fi

  test=NONE
  ssl_libdir=NONE
  for dir in $ssl_libdirs; do
    try="ls -1 $dir/libssl*"
    if test=`eval $try 2> /dev/null`; then ssl_libdir=$dir; break; else echo "tried $dir" >&AS_MESSAGE_LOG_FD()  ; fi
  done

  ac_ssl_libraries="$ssl_libdir"

  AC_LANG_PUSH(C)

  ac_cflags_safe="$CFLAGS"
  ac_ldflags_safe="$LDFLAGS"
  ac_libs_safe="$LIBS"

  CFLAGS="$CFLAGS -I$ssl_incdir $all_includes"
  LDFLAGS="$LDFLAGS -L$ssl_libdir $all_libraries"
  LIBS="$LIBS $LIBSSL -lRSAglue -lrsaref"

	 AC_LINK_IFELSE([AC_LANG_PROGRAM([[]]
  ,[[void RSAPrivateEncrypt(void);RSAPrivateEncrypt();]])],
  ac_ssl_rsaref="yes", ac_ssl_rsaref="no"
  )

  CFLAGS="$ac_cflags_safe"
  LDFLAGS="$ac_ldflags_safe"
  LIBS="$ac_libs_safe"

  AC_LANG_POP([C])

  if test "$ac_ssl_includes" = NO || test "$ac_ssl_libraries" = NO; then
    have_ssl=no
  else
    have_ssl=yes;
  fi

  ])

  eval "$ac_cv_have_ssl"

  AC_MSG_RESULT([libraries $ac_ssl_libraries, headers $ac_ssl_includes])

  AC_MSG_CHECKING([whether OpenSSL uses rsaref])
  AC_MSG_RESULT($ac_ssl_rsaref)

  AC_MSG_CHECKING([for easter eggs])
  AC_MSG_RESULT([none found])

else
  have_ssl=no
fi

if test "$have_ssl" = yes; then
  AC_MSG_CHECKING(for OpenSSL version)
  dnl Check for SSL version
  AC_CACHE_VAL(ac_cv_ssl_version,
  [
    AC_LANG_PUSH(C)

    cat >conftest.$ac_ext <<EOF
#include <openssl/opensslv.h>
#include <stdio.h>
    int main() {
 
#ifndef OPENSSL_VERSION_NUMBER
      printf("ssl_version=\\"error\\"\n");
#else
      if (OPENSSL_VERSION_NUMBER < 0x00906000)
        printf("ssl_version=\\"old\\"\n");
      else
        printf("ssl_version=\\"ok\\"\n");
#endif
     return (0);
    }
EOF

    ac_compile='${CC-gcc} $CFLAGS -I$ac_ssl_includes conftest.$ac_ext -o conftest'
    if AC_TRY_EVAL(ac_compile); then 

      if eval `./conftest 2>&5`; then
        if test $ssl_version = error; then
          AC_MSG_ERROR([$ssl_incdir/openssl/opensslv.h doesn't define OPENSSL_VERSION_NUMBER !])
        else
          if test $ssl_version = old; then
            AC_MSG_WARN([OpenSSL version too old. Upgrade to 0.9.6 at least, see http://www.openssl.org. SSL support disabled.])
            have_ssl=no
          fi
        fi
        ac_cv_ssl_version="ssl_version=$ssl_version"
      else
        AC_MSG_ERROR([Your system couldn't run a small SSL test program.
        Check config.log, and if you can't figure it out, send a mail to 
        David Faure <faure@kde.org>, attaching your config.log])
      fi

    else
      AC_MSG_ERROR([Your system couldn't link a small SSL test program.
      Check config.log, and if you can't figure it out, send a mail to 
      David Faure <faure@kde.org>, attaching your config.log])
    fi 

    AC_LANG_POP([C])

  ])

  eval "$ac_cv_ssl_version"
  AC_MSG_RESULT($ssl_version)
fi

if test "$have_ssl" != yes; then
  LIBSSL="";
else
  AC_DEFINE(HAVE_SSL, 1, [If we are going to use OpenSSL])
  ac_cv_have_ssl="have_ssl=yes \
    ac_ssl_includes=$ac_ssl_includes ac_ssl_libraries=$ac_ssl_libraries ac_ssl_rsaref=$ac_ssl_rsaref"
  
  
  ssl_libraries="$ac_ssl_libraries"
  ssl_includes="$ac_ssl_includes"

  if test "$ac_ssl_rsaref" = yes; then
    LIBSSL="-lssl -lcrypto -lRSAglue -lrsaref" 
  fi

  if test $ssl_version = "old"; then
    AC_DEFINE(HAVE_OLD_SSL_API, 1, [Define if you have OpenSSL < 0.9.6])
  fi
fi

if test "$ssl_includes" = "/usr/include" || test  "$ssl_includes" = "/usr/local/include" || test -z "$ssl_includes"; then
 SSL_INCLUDES="";
else
 SSL_INCLUDES="-I$ssl_includes"
fi

if test "$ssl_libraries" = "/usr/lib" || test "$ssl_libraries" = "/usr/local/lib" || test -z "$ssl_libraries"; then
 SSL_LDFLAGS=""
else
 SSL_LDFLAGS="-L$ssl_libraries -R$ssl_libraries"
fi

AC_SUBST(SSL_INCLUDES)
AC_SUBST(SSL_LDFLAGS)
AC_SUBST(LIBSSL)
])

AC_DEFUN([KDE_CHECK_STRLCPY],
[
  AC_CHECK_STRLCPY
  AC_CHECK_STRLCAT
  AC_CHECK_SIZEOF(size_t)
  AC_CHECK_SIZEOF(unsigned long)

  AC_MSG_CHECKING([sizeof(size_t) == sizeof(unsigned long)])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],[[
    #if SIZEOF_SIZE_T != SIZEOF_UNSIGNED_LONG
       choke me
    #endif
    ]])],[AC_MSG_RESULT([yes])],[
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([
       Apparently on your system our assumption sizeof(size_t) == sizeof(unsigned long)
       does not apply. Oops.
      ])
  ])
])


