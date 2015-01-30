
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
INCLUDEPATH += $$PWD/model
INCLUDEPATH += $$PWD/view

SOURCES += \
    $$PWD/tnuisensorselector.cpp \
    $$PWD/tbackgroundremover.cpp \
    $$PWD/windowsutil.cpp \
    $$PWD/tnuicolorstream.cpp \
    $$PWD/tnuistream.cpp \
    $$PWD/tnuisensor.cpp \
    $$PWD/model/tnuiimagebuffer.cpp \
    $$PWD/view/tbackgroundremovedcamera.cpp \
    $$PWD/view/timage.cpp \
    $$PWD/view/tnuicolorcamera.cpp \
    $$PWD/view/tnuiimage.cpp \
    $$PWD/tnuisensormanager.cpp

HEADERS += \
    $$PWD/tnuisensorselector.h \
    $$PWD/tbackgroundremover.h \
    $$PWD/windowsutil.h \
    $$PWD/stdafx.h \
    $$PWD/tnuicolorstream.h \
    $$PWD/tnuistream.h \
    $$PWD/tnuisensor.h \
    $$PWD/model/tnuiimagebuffer.h \
    $$PWD/view/tbackgroundremovedcamera.h \
    $$PWD/view/timage.h \
    $$PWD/view/tnuicolorcamera.h \
    $$PWD/view/tnuiimage.h \
    $$PWD/tnuisensormanager.h
