QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../eco/eco/lib/cartcommodity.cpp \
    ../eco/eco/lib/commodity.cpp \
    ../eco/eco/lib/order.cpp \
    ../eco/eco/lib/user.cpp \
    command.cpp \
    log.cpp \
    main.cpp \
    server.cpp \
    thread.cpp

HEADERS += \
	../eco/eco/lib/cartcommodity.h \
	../eco/eco/lib/commodity.h \
	../eco/eco/lib/order.h \
	../eco/eco/lib/user.h \
	command.h \
	log.h \
	server.h \
	thread.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
