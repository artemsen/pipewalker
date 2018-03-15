SOURCES +=   main.cpp \
             mainwindow.cpp \
             pipewalker.cpp \
             pipewidget.cpp
TEMPLATE =   app
CONFIG +=    warn_on \
	     thread \
             qt
TARGET =     ../bin/pipewalker
QT +=        gui
HEADERS +=   mainwindow.h \
             pipewalker.h \
             pipewidget.h
RESOURCES += pipewalker.qrc
