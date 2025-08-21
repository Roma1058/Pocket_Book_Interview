CONFIG -= qt

TEMPLATE = lib
DEFINES += IMGCOMPRESSLIB_LIBRARY

CONFIG += c++17
CONFIG += shared

QMAKE_CXXFLAGS += -Wall -Wextra -O2

SOURCES += src/ImgCompressLib.cpp \
           src/Impl/Block4Strategy.cpp \
           src/Impl/BitWriter.cpp \
           src/Impl/BitReader.cpp \
           src/Impl/FileHandler.cpp \
           src/Impl/BmpReader.cpp \
           src/Impl/BmpWriter.cpp

HEADERS += include/ImgCompressLib.h \
           include/RawImageData.h \
           src/Impl/Block4Strategy.h \
           src/Impl/BitWriter.h \
           src/Impl/BitReader.h \
           src/Impl/CompressionStrategy.h \
           src/Impl/FileHandler.h \
           src/Impl/BmpReader.h \
           src/Impl/BmpWriter.h

INCLUDEPATH += include \
               src \
               src/Impl


TARGET = ImgCompressLib

unix {
    target.path = /usr/lib
    LIBS += -lm
}

!isEmpty(target.path): INSTALLS += target

DESTDIR = $$PWD/../build/lib
mkpath($$DESTDIR)
