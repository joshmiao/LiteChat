QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    litechat_dialog.cpp \
    litechat_interface.cpp \
    litechat_login.cpp \
    litechat_message.cpp \
    litechat_register.cpp \
    litechat_server.cpp \
    main.cpp

HEADERS += \
    json.hpp \
    litechat_dialog.h \
    litechat_interface.h \
    litechat_login.h \
    litechat_message.h \
    litechat_register.h \
    litechat_server.h \
    request_type.h

FORMS += \
    litechat_dialog.ui \
    litechat_interface.ui \
    litechat_login.ui \
    litechat_register.ui \
    litechat_server.ui

TRANSLATIONS += \
    LiteChat_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
