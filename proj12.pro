#-------------------------------------------------
# Project created by QtCreator 2022-01-01T20:06:08
#-------------------------------------------------

QT       += core gui sql widgets printsupport

CONFIG += c++11

# Define to emit warnings for deprecated Qt features
DEFINES += QT_DEPRECATED_WARNINGS

# Sources files
SOURCES += \
    connection.cpp \
    main.cpp \
    inviteclass.cpp \

# Header files
HEADERS += \
    connection.h \
    inviteclass.h

# Form files
FORMS += \
    inviteclass.ui

# Resource files
RESOURCES += \
    image.qrc

# Add include directories for POCO headers

# Add POCO libraries
LIBS += -LC:/Users/Lenovo/Documents/poco-1.13.3-all/lib \


# Deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
