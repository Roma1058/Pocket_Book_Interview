QT += quick quickcontrols2

CONFIG += c++17

TEMPLATE = app

QMAKE_CXXFLAGS += -Wall -Wextra -O2
QMAKE_CXXFLAGS += -std=c++17

SOURCES += src/main.cpp \
           src/FileListModel.cpp \
           src/CompressionWorker.cpp

HEADERS += include/FileListModel.h \
           include/CompressionWorker.h

RESOURCES += qml.qrc

DESTDIR = $$PWD/../build

INCLUDEPATH += ../ImgCompressLib/include
LIBS += -L$$PWD/../build/lib -lImgCompressLib
QMAKE_LFLAGS += -Wl,-rpath,$$PWD/../build/lib

TARGET = ImgCompressUI

unix {
    LIBS += -lm
}
