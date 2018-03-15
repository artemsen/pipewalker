SOURCES +=   source/main.cpp \
             source/mainwindow.cpp \
             source/pipewalker.cpp \
             source/pipewidget.cpp
TEMPLATE =   app
CONFIG +=    warn_on \
	     thread \
             qt
TARGET =     ./bin/pipewalker

QT +=        gui \
             xml
HEADERS +=   source/mainwindow.h \
             source/pipewalker.h \
             source/pipewidget.h \
             source/levels.h

RESOURCES += source/pipewalker.qrc

TRANSLATIONS = source/pipewalker_ru.ts
