QT_REQ_MAJOR_VERSION = 5
QT_REQ_MINOR_VERSION = 11
QT_REQ_VERSION = "$$QT_REQ_MAJOR_VERSION"."$$QT_REQ_MINOR_VERSION"

lessThan(QT_MAJOR_VERSION, $$QT_REQ_MAJOR_VERSION) {
    error(Minimum supported Qt version is $$QT_REQ_VERSION!)
}
equals(QT_MAJOR_VERSION, $$QT_REQ_MAJOR_VERSION):lessThan(QT_MINOR_VERSION, $$QT_REQ_MINOR_VERSION) {
    error(Minimum supported Qt version is $$QT_REQ_VERSION!)
}


QT       += core gui widgets
TARGET   = leelagtp
CONFIG   += c++14

#QMAKE_CFLAGS_RELEASE += -g
#QMAKE_CXXFLAGS_RELEASE += -g
# Disable optimization
#QMAKE_CFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE -= -O2

TEMPLATE = app

DEFINES += LEELA_GTP
DEFINES += ANCIENT_CHINESE_RULE_ENABLED

INCLUDEPATH += ../autogtp/

SOURCES += main.cpp \
    ../autogtp/Game.cpp \
    ../autogtp/Worker.cpp \
    ../autogtp/Order.cpp \
    ../autogtp/Job.cpp \
    ../autogtp/Management.cpp \
    LeelaGTP.cpp \
    GTPConfig.cpp \
    ShowBoard.cpp \
    Translation.cpp

HEADERS += \
    ../autogtp/Game.h \
    ../autogtp/Worker.h \
    ../autogtp/Job.h \
    ../autogtp/Order.h \
    ../autogtp/Result.h \
    ../autogtp/Management.h \
    LeelaGTP.h \
    GTPConfig.h \
    ShowBoard.h \
    Translation.h

RESOURCES += \
    leelagtp.qrc
