cmake_minimum_required(VERSION 3.16)

project(extcalc VERSION 0.9.3 LANGUAGES CXX)

# --- Qt 6 ---------------------------------------------------------------
# Add/Remove modules here as your code requires.
# You had QT_OPENGL + GUI usage; in Qt6, widgets-based GL is in OpenGLWidgets.
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

# Good defaults (available in newer Qt 6; safe to omit if unavailable)
# qt_standard_project_setup()

# Use modern AUTOMOC/AUTOUIC/AUTORCC instead of QT4_WRAP_* macros.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Build type (optional override)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# -----------------------------------------------------------------------
# Sources (unchanged lists from your file)
set(moc_h 
  src/buttons.h
  src/calcwidget.h
  src/graphinput.h
  src/table.h
  src/calcinput.h
  src/catalog.h
  src/graphout.h
  src/matrixwidget.h
  src/tablepref.h
  src/extcalc.h
  src/graphpref.h
  src/scriptedit.h
  src/functiontable.h
  src/graphsetdialog.h
  src/scriptio.h
  src/calcpref.h
  src/graphsolve.h
  src/scriptpref.h
  src/calctable.h
  src/graph.h
  src/importdialog.h
  src/tabwidget.h
  src/statistics.h
  src/scriptdisplay.h
  src/screenshotdialog.h
)

set(src_h
  src/buttons.h
  src/calcwidget.h
  src/graphinput.h
  src/table.h
  src/calcinput.h
  src/catalog.h
  src/graphout.h
  src/matrixwidget.h
  src/tablepref.h
  src/extcalc.h
  src/graphpref.h
  src/scriptedit.h
  src/functiontable.h
  src/graphsetdialog.h
  src/scriptio.h
  src/calcpref.h
  src/graphsolve.h
  src/scriptpref.h
  src/calctable.h
  src/graph.h
  src/importdialog.h
  src/statistics.h
  src/tabwidget.h
  src/list.h
  src/calclocale.h
  src/global.h
  src/scriptdisplay.h
  src/screenshotdialog.h
)

set(src_cxx 
  src/buttons.cpp
  src/calcwidget.cpp
  src/graphinput.cpp
  src/table.cpp
  src/calcinput.cpp
  src/catalog.cpp
  src/graphout.cpp
  src/matrixwidget.cpp
  src/tablepref.cpp
  src/extcalc.cpp
  src/graphpref.cpp
  src/scriptedit.cpp
  src/functiontable.cpp
  src/graphsetdialog.cpp
  src/scriptio.cpp
  src/calcpref.cpp
  src/graphsolve.cpp
  src/scriptpref.cpp
  src/calctable.cpp
  src/graph.cpp
  src/importdialog.cpp
  src/statistics.cpp
  src/tabwidget.cpp
  src/global.cpp
  src/scriptdisplay.cpp
  src/screenshotdialog.cpp
)

set(src_ui
  src/screenshotdialog.ui
)

set(src_qrc
  share/icons.qrc
)

# -----------------------------------------------------------------------
# Target
# You can also use qt_add_executable(... MANUAL_FINALIZATION ...) if you prefer.
add_executable(extcalc WIN32
  ${src_cxx}
  ${src_h}
  ${moc_h}
  ${src_ui}
  ${src_qrc}
)

# Include paths for your own headers
target_include_directories(extcalc PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Definitions
# Old: -DQT3_SUPPORT (removed in Qt6)
# Keep your INSTALLDIR define:
set(CMAKE_INSTALL_PREFIX "/usr/local")
add_definitions(-DINSTALLDIR=\"\\\"${CMAKE_INSTALL_PREFIX}/share/extcalc\\\"\")

# Link modern Qt6 imported targets
target_link_libraries(extcalc PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::OpenGL
  Qt6::OpenGLWidgets
)

# If you used qt_add_executable with MANUAL_FINALIZATION, call:
# qt_finalize_executable(extcalc)

# -----------------------------------------------------------------------
# Install layout (unchanged)
set(SHAREDIR     "share/extcalc/data")
set(DOCDIR       "share/extcalc/doc")
set(DESKTOPDIR   "share/applications")
set(ICONDIR      "share/icons")
set(MANDIR       "/usr/man")
set(BINDIR       "bin")

install(FILES
  share/binary.png
  share/catalog.png
  share/text.png
  share/scientific.png
  share/angle.png
  share/editcopy.png
  share/view_remove.png
  share/view_top_bottom.png
  share/undo.png
  share/modified.png
  share/redo.png
  share/rubber.png
  share/forward.png
  share/fileexport.png
  share/filenew.png
  share/back.png
  share/editpaste.png
  share/editdelete.png
  share/print.png
  share/save_all.png
  share/open.png
  share/zoomout.png
  share/filesaveas.png
  share/zoomin.png
  share/editcut.png
  share/exit.png
  share/fileimport.png
  share/exec.png
  share/rect.png
  share/free.png
  share/circle.png
  share/line.png
  share/icons.qrc
  DESTINATION ${SHAREDIR}
)

install(FILES
  share/qt_fr.qm
  share/qt_de.qm
  share/french.qm
  share/german.qm
  DESTINATION ${SHAREDIR}
)

install(FILES
  share/crossproduct
  share/dotproduct
  share/snake
  share/matrix-vector
  share/matrixproduct
  DESTINATION ${SHAREDIR}
)

install(FILES
  share/license.txt
  DESTINATION ${SHAREDIR}
)

install(FILES
  share/extcalc.desktop
  DESTINATION ${DESKTOPDIR}
)

install(FILES
  share/extcalc22.png
  share/extcalc48.png
  DESTINATION ${ICONDIR}
)

install(FILES
  doc/dev_docu.html
  doc/help_de.html
  doc/help_en.html
  doc/help_fr.html
  DESTINATION ${DOCDIR}
)

install(FILES
  doc/3dgraph.jpg
  doc/alpha.jpg
  doc/analyse.jpg
  doc/calculator.jpg
  doc/catalogbutton.jpg
  doc/curoot.jpg
  doc/diffbutton.jpg
  doc/diff.jpg
  doc/graph.jpg
  doc/integralbutton.jpg
  doc/integral.jpg
  doc/matvec.jpg
  doc/nrootbutton.jpg
  doc/nroot.jpg
  doc/randbutton.jpg
  doc/random.jpg
  doc/screenshot.jpg
  doc/scriptedit2.jpg
  doc/scriptedit.jpg
  doc/scriptrun.jpg
  doc/setbutton.jpg
  doc/setbutton.jpg
  doc/setvar.jpg
  doc/shift.jpg
  doc/sqrtbracket.jpg
  doc/sqrtbutton.jpg
  doc/sqrt.jpg
  doc/stat1.jpg
  doc/stat2.jpg
  doc/table.jpg
  doc/usevar.jpg
  DESTINATION ${DOCDIR}
)

install(TARGETS extcalc
  RUNTIME DESTINATION ${BINDIR}
  BUNDLE  DESTINATION .
  PERMISSIONS
    OWNER_EXECUTE OWNER_WRITE OWNER_READ
    GROUP_EXECUTE GROUP_READ
    WORLD_EXECUTE WORLD_READ
)

# -----------------------------------------------------------------------
# CPack (mostly kept as-is; consider updating distro deps to Qt6 names)
set(CPACK_INSTALL_CMAKE_PROJECTS "/home/user/Projekte/extcalc;Extcalc;ALL;/")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scientific Graphic Calculator")
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_NAME "extcalc")

set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_STRIP_FILES "bin/extcalc")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_PACKAGE_EXECUTABLES "extcalc;extcalc")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES "CVS/")

# Debian/RPM metadata were for Qt4-era packages. Update as needed for your distro.
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")
# Example (commented): set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libgl1, libglu1, qt6-base-dev")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Rainer Strobel <rainer1223@users.sourceforge.net>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Extcalc is a multifunctional scientific graphic calculator for Linux with features like graph drawing, graph analysis and calculation of scientific functions. Extcalc also provides an integrated programming language.")
set(CPACK_DEBIAN_PACKAGE_SECTION "math")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

set(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
set(CPACK_RPM_PACKAGE_LICENSE "GPL")
set(CPACK_RPM_PACKAGE_GROUP "Productivity/Schientific/Math")
set(CPACK_RPM_PACKAGE_VENDOR "Rainer Strobel <rainer1223@users.sourceforge.net>")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Extcalc is a multifunctional scientific graphic calculator for Linux with features like graph drawing, graph analysis and calculation of scientific functions. Extcalc also provides an integrated programming language.")

include(CPack)

