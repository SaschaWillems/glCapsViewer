# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

TEMPLATE = app
TARGET = glCapsViewer
QT += core network widgets gui
CONFIG += c++11
DEFINES += QT_DLL QT_NETWORK_LIB QT_WIDGETS_LIB

win32 {
    LIBS += Advapi32.lib User32.lib gdi32.lib shell32.lib opengl32.lib
}

win32:contains(QMAKE_HOST.arch, x86_64) {
    DEFINES += WIN64
    LIBS += "$$PWD/external/libs/x64/glew32.lib"
    LIBS += "$$PWD/external/libs/x64/glfw3.lib"
    DESTDIR = ./x86_64
} else {
    LIBS += "$$PWD/external/libs/Win32/glew32.lib"
    LIBS += "$$PWD/external/libs/Win32/glfw3.lib"
    DESTDIR = ./WIN32
}

debug {
    DESTDIR = $$DESTDIR/debug/
} else {
    DESTDIR = $$DESTDIR/release/
}

HEADERS = \
   $$PWD/GeneratedFiles/ui_glcapsviewer.h \
   $$PWD/glcapsviewer_autogen/include/ui_glCapsViewer.h \
   $$PWD/capsGroup.h \
   $$PWD/glCapsViewer.h \
   $$PWD/glCapsViewerCore.h \
   $$PWD/glCapsViewerHttp.h \
   $$PWD/internalFormatInfo.h \
   $$PWD/internalFormatTarget.h \
   $$PWD/resource.h \
   $$PWD/settings.h \
   $$PWD/settingsDialog.h \
   $$PWD/submitDialog.h \
   $$PWD/treeproxyfilter.h

SOURCES = \
   $$PWD/capsGroup.cpp \
   $$PWD/glcapsviewer.cpp \
   $$PWD/glcapsviewerCore.cpp \
   $$PWD/glCapsViewerHttp.cpp \
   $$PWD/internalFormatInfo.cpp \
   $$PWD/internalFormatTarget.cpp \
   $$PWD/main.cpp \
   $$PWD/qrc_glCapsViewer.cpp \
   $$PWD/settings.cpp \
   $$PWD/settingsDialog.cpp \
   $$PWD/submitDialog.cpp \
   $$PWD/treeproxyfilter.cpp

FORMS += ./glcapsviewer.ui
RESOURCES += glCapsViewer.qrc

INCLUDEPATH = \
    $$PWD/. \
    $$PWD/GeneratedFiles \
    $$PWD/glcapsviewer_autogen/include
INCLUDEPATH += "./external"

DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/release
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
win32:RC_FILE = glCapsViewer.rc

