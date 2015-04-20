
INCLUDEPATH += "$$(KINECTSDK10_DIR)inc"
INCLUDEPATH += "$$(KINECT_TOOLKIT_DIR)inc"

KINECT_VERSION = 180
KINECT_ARCHTECTURE = x86
KINECT_WORDSIZE = 32
contains(QMAKE_HOST.arch, x86_64){
    KINECT_ARCHTECTURE = amd64
    KINECT_WORDSIZE = 64
}

LIBS += -L"$$(KINECTSDK10_DIR)lib/$${KINECT_ARCHTECTURE}"
LIBS += -L"$$(KINECT_TOOLKIT_DIR)Lib/$${KINECT_ARCHTECTURE}"
LIBS += -lKinect10

for(libname, KINECT_LIB): LIBS += -l$${libname}$${KINECT_VERSION}_$${KINECT_WORDSIZE}
!build_pass: for(libname, KINECT_LIB) {
    system("$$QMAKE_COPY \"$$system_path($$(KINECT_TOOLKIT_DIR)bin/$${libname}$${KINECT_VERSION}_$${KINECT_WORDSIZE}.dll)\" \"$$WORKING_DIR\"")
}

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/view/tnuicolorcamera.cpp \
    $$PWD/view/tnuihandarea.cpp \
    $$PWD/view/tnuitrackeritem.cpp \
    $$PWD/kinectglobal.cpp \
    $$PWD/tnuicolorstreameffect.cpp \
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
    $$PWD/view/tnuihandarea.h \
    $$PWD/view/tnuitrackeritem.h \
    $$PWD/kinectglobal.h \
    $$PWD/tnuicolorstreameffect.h \
    $$PWD/tnuicolorstream.h \
    $$PWD/tnuidepthstream.h \
    $$PWD/tnuiimagestream.h \
    $$PWD/tnuisensor.h \
    $$PWD/tnuisensormanager.h \
    $$PWD/tnuiskeletonstream.h \
    $$PWD/tnuistream.h \
    $$PWD/tnuitracker.h \
    $$PWD/windowsutil.h

contains(KINECT_LIB, KinectBackgroundRemoval){
    DEFINES += KINECT_USE_BACKGROUNDREMOVAL
    SOURCES += $$PWD/tnuibackgroundremovaleffect.cpp
    HEADERS += $$PWD/tnuibackgroundremovaleffect.h
}

contains(KINECT_LIB, KinectInteraction){
    DEFINES += KINECT_USE_INTERACTION
    SOURCES += $$PWD/tnuiinteractionstream.cpp
    HEADERS += $$PWD/tnuiinteractionstream.h
}
