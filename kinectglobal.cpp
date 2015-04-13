#include "kinectglobal.h"

#include "view/tnuicolorcamera.h"
#include "view/tnuitrackeritem.h"
#include "view/tnuihandarea.h"

#include "tnuisensormanager.h"

void initializeKinect(){
    qmlRegisterType<TNuiColorCamera>("Kinect", 1, 0, "TNuiColorCamera");
    qmlRegisterType<TNuiTrackerItem>("Kinect", 1, 0, "TNuiTrackerItem");
    qmlRegisterType<TNuiHandArea>("Kinect", 1, 0, "TNuiHandArea");

    SensorManager = TNuiSensorManager::instance();
}
