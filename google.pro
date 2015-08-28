#-------------------------------------------------
#
# Project created by QtCreator 2015-08-24T13:46:02
#
#-------------------------------------------------

TARGET = qtgeoservices_google
#TEMPLATE = lib
CONFIG += plugin
QT +=  location-private positioning-private network

PLUGIN_TYPE = geoservices
PLUGIN_CLASS_NAME = QGeoServiceProviderFactoryGoogle
load(qt_plugin)

DESTDIR = $$[QT_INSTALL_PLUGINS]/geoservices

SOURCES += \
    qgeotiledmappingmanagerenginegoogle.cpp \
    qgeotilefetchergoogle.cpp \
    qgeotiledmapreplygoogle.cpp \
    qgeoserviceproviderfactorygoogle.cpp \
    qgeotiledmapgoogle.cpp \
    qcoordtrans.cpp

HEADERS += \
    qgeotiledmappingmanagerenginegoogle.h \
    qgeotilefetchergoogle.h \
    qgeotiledmapreplygoogle.h \
    qgeoserviceproviderfactorygoogle.h \
    qgeotiledmapgoogle.h \
    qcoordtrans.h

DISTFILES += google.json

unix {
    target.path = /usr/lib
    INSTALLS += target
}
