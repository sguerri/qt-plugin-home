QT += widgets

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++17

VERSION = 0.1.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PHome.cpp \
    PHomePage.cpp \
    components/PHomePageFile.cpp \
    components/PHomePageProject.cpp

HEADERS += \
    PHome.h \
    PHomePage.h \
    components/PHomePageFile.h \
    components/PHomePageProject.h \
    utils/font.h \
    utils/slugify.h

FORMS += \
    PHomePage.ui \
    components/PHomePageFile.ui \
    components/PHomePageProject.ui

TRANSLATIONS += \
    i18n/pluginHome_fr_FR.ts \
    i18n/pluginHome_en_US.ts

RESOURCES += \
    pluginHome.qrc

DISTFILES += \
    LICENSE \
    README.md \
    TODO.md

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../componentSwitchButton/release/ -lcomponentSwitchButton
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../componentSwitchButton/debug/ -lcomponentSwitchButton
else:unix: LIBS += -L$$OUT_PWD/../componentSwitchButton/ -lcomponentSwitchButton

INCLUDEPATH += $$PWD/../componentSwitchButton
DEPENDPATH += $$PWD/../componentSwitchButton

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../componentSwitchButton/release/libcomponentSwitchButton.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../componentSwitchButton/debug/libcomponentSwitchButton.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../componentSwitchButton/release/componentSwitchButton.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../componentSwitchButton/debug/componentSwitchButton.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../componentSwitchButton/libcomponentSwitchButton.a
