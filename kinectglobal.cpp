#include "kinectglobal.h"

#include "tnuicolorcamera.h"
#include "tnuitrackeritem.h"
#include "tnuimousearea.h"
#include "tnuisensormanager.h"

void initializeKinect(){
    qmlRegisterType<TNuiColorCamera>("Kinect", 1, 0, "TNuiColorCamera");
    qmlRegisterType<TNuiTrackerItem>("Kinect", 1, 0, "TNuiTrackerItem");
    qmlRegisterType<TNuiMouseArea>("Kinect", 1, 0, "TNuiMouseArea");

    SensorManager = TNuiSensorManager::instance();
}
