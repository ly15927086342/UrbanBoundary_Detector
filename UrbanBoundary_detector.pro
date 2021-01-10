#-------------------------------------------------
#
# Project created by QtCreator 2019-06-12T17:14:53
#
#-------------------------------------------------

QT += core gui

#CONFIG += c++11

#multithreads
QT += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = UrbanBoundary_detector
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bufferdilationmodule.cpp \
    datamodule.cpp \
    fractalmodule.cpp \
    qcustomplot.cpp \
    findcontours.cpp \
    cubic_hermite_interp.cpp \
    agglomerativeclustering.cpp

HEADERS  += mainwindow.h \
    all.h \
    bufferdilationmodule.h \
    curvature.h \
    datamodule.h \
    fractalmodule.h \
    point.h \
    Eigen3/Eigen/Dense \
    qcustomplot.h \
    findcontours.h \
    cubic_hermite_interp.h \
    agglomerativeclustering.h

FORMS    += mainwindow.ui

RESOURCES += \
    logo.qrc

win32: LIBS += -L$$PWD/gdal_2.1.1/lib/ -lgdal_i

INCLUDEPATH += $$PWD/gdal_2.1.1/include
DEPENDPATH += $$PWD/gdal_2.1.1/include

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/gdal_2.1.1/lib/gdal_i.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/gdal_2.1.1/lib/libgdal_i.a

DISTFILES += \
    HAC.py

INCLUDEPATH += -I F:/Python/Python37_86/include
LIBS += -LF:/Python/Python37_86/libs -lpython37

#INCLUDEPATH += -I F:/ArcGIS10.2/include
#LIBS += -LF:/ArcGIS10.2/libs/ -lpython27

#INCLUDEPATH += -I F:/Python/Python27/include
#LIBS += -LF:/Python/Python27/libs -lpython27
#DEPENDPATH += $$PWD/F:/Python/Python37/include

#win32:!win32-g++: PRE_TARGETDEPS += $$PWD/F:/Python/Python37/libs/python37.lib
#else:win32-g++: PRE_TARGETDEPS += $$PWD/F:/Python/Python37/libs/libpython37.a
