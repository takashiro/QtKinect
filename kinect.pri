
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
    $$PWD/view/tnuicolorcamera.cpp \
    $$PWD/view/tnuimousearea.cpp \
    $$PWD/view/tnuitrackeritem.cpp \
    $$PWD/kinectglobal.cpp \
    $$PWD/tnuibackgroundremovedcolorstream.cpp \
    $$PWD/tnuicolorstream.cpp \
    $$PWD/tnuidepthstream.cpp \
    $$PWD/tnuiimagestream.cpp \
    $$PWD/tnuisensor.cpp \
    $$PWD/tnuisensormanager.cpp \
    $$PWD/tnuiskeletonstream.cpp \
    $$PWD/tnuistream.cpp \
    $$PWD/tnuitracker.cpp \
    $$PWD/windowsutil.cpp

HEADERS += \
    $$PWD/view/tnuicolorcamera.h \
    $$PWD/view/tnuimousearea.h \
    $$PWD/view/tnuitrackeritem.h \
    $$PWD/kinectglobal.h \
    $$PWD/tnuibackgroundremovedcolorstream.h \
    $$PWD/tnuicolorstream.h \
    $$PWD/tnuidepthstream.h \
    $$PWD/tnuiimagestream.h \
    $$PWD/tnuisensor.h \
    $$PWD/tnuisensormanager.h \
    $$PWD/tnuiskeletonstream.h \
    $$PWD/tnuistream.h \
    $$PWD/tnuitracker.h \
    $$PWD/windowsutil.h
