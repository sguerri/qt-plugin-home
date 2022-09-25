QT -= gui
QT += widgets

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++17

VERSION = 0.0.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PHome.cpp

HEADERS += \
    PHome.h

TRANSLATIONS += \
    i18n/pluginHome_fr_FR.ts \
    i18n/pluginHome_en_US.ts

DISTFILES += \
    HomePage.cpp \
    HomePageFile.cpp \
    HomePageProject.cpp \
    HomePage.h \
    HomePageFile.h \
    HomePageProject.h \
    HomePageSlugify.h \
    HomePage.ui \
    HomePageFile.ui \
    HomePageProject.ui \
    LICENSE \
    README.md \
    TODO.md

RESOURCES += \
    pluginHome.qrc

FORMS += \

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

