QT       += qml quick quickcontrols2 sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 staticlib

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	../../server/command.cpp \
	cartmodel.cpp \
    commoditymodel.cpp \
    lib/client.cpp \
    lib/commodity.cpp \
	lib/order.cpp \
    main.cpp \
    lib/user.cpp \
	ordermodel.cpp

HEADERS += \
	../../server/command.h \
	cartmodel.h \
    commoditymodel.h \
    lib/client.h \
    lib/commodity.h \
	lib/order.h \
    lib/user.h \
	ordermodel.h

DISTFILES += \
    main.qml \
    main.qml


FORMS +=

RESOURCES += \
    rsc.qrc

QML_IMPORT_PATH =


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    main.qml


FORMS +=

RESOURCES +=

QML_IMPORT_PATH =


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
