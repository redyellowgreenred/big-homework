QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    character.cpp \
    main.cpp \
    mainwindow.cpp \
    mygraphicsview.cpp \
    player.cpp \
    prop.cpp \
    propfactory.cpp

HEADERS += \
    character.h \
    mainwindow.h \
    mygraphicsview.h \
    player.h \
    prop.h \
    propfactory.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    src.qrc

# Deployment rules (optional)
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target
