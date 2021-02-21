QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = camera
TEMPLATE = app


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
    main.cpp \
    mainwindow.cpp \
    show_thread.cpp

HEADERS += \
    mainwindow.h \
    show_thread.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += /home/icedot/linux/opencv-3.4.1/install/include

#LIBS += ../../lib/libopencv_core.so \
#        ../../lib/libopencv_highgui.so \
#        ../../lib/libopencv_imgproc.so \
#        ../../lib/libopencv_videoio.so \
#        ../../lib/libopencv_imgcodecs.so \
#        -lpthread

LIBS += /home/icedot/linux/opencv-3.4.1/install/lib/libopencv_core.so \
        /home/icedot/linux/opencv-3.4.1/install/lib/libopencv_highgui.so \
        /home/icedot/linux/opencv-3.4.1/install/lib/libopencv_imgproc.so \
        /home/icedot/linux/opencv-3.4.1/install/lib/libopencv_videoio.so \
        /home/icedot/linux/opencv-3.4.1/install/lib/libopencv_imgcodecs.so \
        -lpthread



