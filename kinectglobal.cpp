#include "kinectglobal.h"

#include "timage.h"
#include "tnuicolorcamera.h"
#include "tnuimouse.h"
#include "tnuimousearea.h"
#include "tnuisensormanager.h"

void initializeKinect(){
    qmlRegisterType<TImage>("Kinect", 1, 0, "TImage");
    qmlRegisterType<TNuiColorCamera>("Kinect", 1, 0, "TNuiColorCamera");
    qmlRegisterType<TNuiMouse>("Kinect", 1, 0, "TNuiMouse");
    qmlRegisterType<TNuiMouseArea>("Kinect", 1, 0, "TNuiMouseArea");

    SensorManager = TNuiSensorManager::instance();
}
