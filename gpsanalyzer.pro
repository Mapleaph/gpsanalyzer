#-------------------------------------------------
#
# Project created by QtCreator 2016-08-10T16:25:35
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = gpsanalyzer

SOURCES += main.cpp \
    dialog.cpp \
    gpsdataretreiver.cpp

HEADERS  += \
    dialog.h \
    gpsdataretreiver.h \
    nmea.h \

RESOURCES += \
    res.qrc

RC_FILE = res.rc
