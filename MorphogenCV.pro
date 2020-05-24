QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configparser.cpp \
    generator.cpp \
    imageiterationplot.cpp \
    imageoperations.cpp \
    main.cpp \
    mainwidget.cpp \
    qcustomplot.cpp

HEADERS += \
    configparser.h \
    generator.h \
    imageiterationplot.h \
    imageoperations.h \
    mainwidget.h \
    parameter.h \
    parameterwidget.h \
    qcustomplot.h

# Change path according to your system
unix:INCLUDEPATH += /usr/include/opencv4
win32:INCLUDEPATH += C:/opencv-dynamic-qt-dynamic-build/install/include

# Change path according to your system
unix:LIBS += -lopencv_core -lopencv_imgproc -lopencv_photo -lopencv_highgui -lopencv_videoio
win32:LIBS += -LC:/opencv-dynamic-qt-dynamic-build/install/x64/mingw/lib -lopencv_core430.dll -lopencv_imgproc430.dll -lopencv_photo430.dll -lopencv_highgui430.dll -lopencv_videoio430.dll

QMAKE_CXXFLAGS_RELEASE += -O3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
