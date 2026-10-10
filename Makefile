CXX       = g++
AR        = ar
ARFLAGS   = rcs

PREFIX     ?= /usr
INCLUDEDIR = $(PREFIX)/include/hbtk
LIBDIR     = $(PREFIX)/lib

CPPFLAGS = -Iinclude -I/usr/include/freetype2
CXXFLAGS = -O2 -Wall -Wextra -fPIC

TARGET = libhbtk.a

SOURCES = \
	src/Application.cpp \
	src/Button.cpp \
	src/Colors.cpp \
	src/Dialog.cpp \
	src/Entry.cpp \
	src/Event.cpp \
	src/EventDispatcher.cpp \
	src/Frame.cpp \
	src/FileTree.cpp \
	src/Label.cpp \
	src/MainWindow.cpp \
	src/Menu.cpp \
	src/MenuBar.cpp \
	src/Module.cpp \
	src/Painter.cpp \
	src/Splitter.cpp \
	src/Style.cpp \
	src/StyleEngine.cpp \
	src/TerminalWidget.cpp \
	src/Widget.cpp \
	src/WidgetContainer.cpp

OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

install: $(TARGET)
	install -d $(DESTDIR)$(INCLUDEDIR)
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 include/*.hpp $(DESTDIR)$(INCLUDEDIR)/
	install -m 644 $(TARGET) $(DESTDIR)$(LIBDIR)/

uninstall:
	rm -rf $(DESTDIR)$(INCLUDEDIR)
	rm -f $(DESTDIR)$(LIBDIR)/$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)
