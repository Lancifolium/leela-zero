QT_REQ_MAJOR_VERSION = 5
QT_REQ_MINOR_VERSION = 3
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
RC_FILE  += leelagtp.rc

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
    MovLancifolium.cpp \
    GTPConfig.cpp \
    Translation.cpp

HEADERS += \
    ../autogtp/Game.h \
    ../autogtp/Worker.h \
    ../autogtp/Job.h \
    ../autogtp/Order.h \
    ../autogtp/Result.h \
    ../autogtp/Management.h \
    LeelaGTP.h \
    MovLancifolium.h \
    GTPConfig.h \
    Translation.h

DISTFILES += \
    leelagtp.rc

RESOURCES += \
    leelagtp.qrc
