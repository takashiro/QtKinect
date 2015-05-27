#include "kinectglobal.h"

#include "view/tnuicolorcamera.h"
#include "view/tnuiskeleton.h"
#include "view/tnuihandarea.h"

#include "tnuisensormanager.h"

void initializeKinect(){
    qmlRegisterType<TNuiColorCamera>("Kinect", 1, 0, "TNuiColorCamera");
    qmlRegisterType<TNuiSkeleton>("Kinect", 1, 0, "TNuiSkeleton");
    qmlRegisterType<TNuiHandArea>("Kinect", 1, 0, "TNuiHandArea");

    SensorManager = TNuiSensorManager::instance();
}
