
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
    $$PWD/windowsutil.cpp \
    $$PWD/tnuistream.cpp \
    $$PWD/tnuisensor.cpp \
    $$PWD/view/tnuicolorcamera.cpp \
    $$PWD/tnuisensormanager.cpp \
    $$PWD/tnuiskeletonstream.cpp \
    $$PWD/tnuiimagestream.cpp \
    $$PWD/tnuitracker.cpp \
    $$PWD/view/tnuimousearea.cpp \
    $$PWD/kinectglobal.cpp \
    $$PWD/view/tnuitrackeritem.cpp \
    $$PWD/tnuibackgroundremovedcolorstream.cpp

HEADERS += \
    $$PWD/windowsutil.h \
    $$PWD/stdafx.h \
    $$PWD/tnuistream.h \
    $$PWD/tnuisensor.h \
    $$PWD/view/tnuicolorcamera.h \
    $$PWD/tnuisensormanager.h \
    $$PWD/tnuiskeletonstream.h \
    $$PWD/tnuiimagestream.h \
    $$PWD/tnuitracker.h \
    $$PWD/view/tnuimousearea.h \
    $$PWD/kinectglobal.h \
    $$PWD/view/tnuitrackeritem.h \
    $$PWD/tnuibackgroundremovedcolorstream.h
