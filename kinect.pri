
INCLUDEPATH += "$$(KINECTSDK10_DIR)inc"
INCLUDEPATH += "$$(KINECT_TOOLKIT_DIR)inc"

contains(QMAKE_HOST.arch, x86_64){
    LIBS += -L"$$(KINECTSDK10_DIR)lib/amd64"
    LIBS += -L"$$(KINECT_TOOLKIT_DIR)Lib/amd64"

    LIBS += -lKinectBackgroundRemoval180_64
}else{
    LIBS += -L"$$(KINECTSDK10_DIR)lib/x86"
    LIBS += -L"$$(KINECT_TOOLKIT_DIR)Lib/x86"

    LIBS += -lKinectBackgroundRemoval180_32
}
LIBS += -lKinect10

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/tnuisensorselector.cpp \
    $$PWD/tbackgroundremover.cpp \
    $$PWD/windowsutil.cpp \
    $$PWD/tvideo.cpp \
    $$PWD/tbackgroundremovedcamera.cpp

HEADERS += \
    $$PWD/tnuisensorselector.h \
    $$PWD/tbackgroundremover.h \
    $$PWD/windowsutil.h \
    $$PWD/stdafx.h \
    $$PWD/tvideo.h \
    $$PWD/tbackgroundremovedcamera.h
