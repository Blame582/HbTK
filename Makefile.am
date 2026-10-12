AM_CPPFLAGS = -I$(srcdir)/include $(HBTK_CFLAGS)
AM_CXXFLAGS = -Wall -Wextra -fPIC

lib_LIBRARIES = libhbtk.a
libhbtk_a_SOURCES = \
    src/Application.cpp \
    src/Button.cpp \
    src/Colors.cpp \
    src/Dialog.cpp \
    src/Entry.cpp \
    src/Event.cpp \
    src/EventDispatcher.cpp \
    src/FileTree.cpp \
    src/Frame.cpp \
    src/Label.cpp \
    src/MainWindow.cpp \
    src/Menu.cpp \
    src/MenuBar.cpp \
    src/Module.cpp \
    src/OpenFileDialog.cpp \
    src/Painter.cpp \
    src/Splitter.cpp \
    src/Style.cpp \
    src/StyleEngine.cpp \
    src/TerminalWidget.cpp \
    src/TextEdit.cpp \
    src/Widget.cpp \
    src/WidgetContainer.cpp

hbtkincludedir = $(includedir)/hbtk
hbtkinclude_HEADERS = \
    include/Application.hpp \
    include/Button.hpp \
    include/Colors.hpp \
    include/Dialog.hpp \
    include/Entry.hpp \
    include/Event.hpp \
    include/EventDispatcher.hpp \
    include/FileTree.hpp \
    include/Frame.hpp \
    include/Label.hpp \
    include/MainWindow.hpp \
    include/Menu.hpp \
    include/MenuBar.hpp \
    include/Module.hpp \
    include/OpenFileDialog.hpp \
    include/Painter.hpp \
    include/Splitter.hpp \
    include/Style.hpp \
    include/StyleEngine.hpp \
    include/TerminalWidget.hpp \
    include/TextEdit.hpp \
    include/Widget.hpp \
    include/WidgetContainer.hpp

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hbtk.pc

EXTRA_DIST = README docs/INSTALL