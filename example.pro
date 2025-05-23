QT += core gui widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aicharacter.cpp \
    character.cpp \
    healthbar.cpp \
    main.cpp \
    mainwindow.cpp \
    mygraphicsview.cpp \
    player.cpp \
    prop.cpp \
    propeffect.cpp \
    propfactory.cpp \
    selectionline.cpp

HEADERS += \
    aicharacter.h \
    character.h \
    healthbar.h \
    mainwindow.h \
    mygraphicsview.h \
    player.h \
    prop.h \
    propeffect.h \
    propfactory.h \
    selectionline.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    src.qrc

# Deployment rules (optional)
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target
