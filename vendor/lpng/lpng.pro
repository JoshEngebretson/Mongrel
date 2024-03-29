QT -= core gui

TARGET = lpng
TEMPLATE = lib
CONFIG += staticlib

HEADERS += png.h \
        pngconf.h \
        pngpriv.h

SOURCES += png.c \
        pngerror.c \
        pngget.c \
        pngmem.c \
        pngpread.c \
        pngread.c \
        pngrio.c \
        pngrtran.c \
        pngrutil.c \
        pngset.c \
        pngtest.c \
        pngtrans.c \
        pngwio.c \
        pngwrite.c \
        pngwtran.c \
        pngwutil.c
