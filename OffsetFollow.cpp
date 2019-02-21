#include "OffsetFollow.h"
#include "ObjectModel.h"


// create a 
OffsetFollow::OffsetFollow(float screenRatio)
    : Camera(screenRatio)
{
}


// OffsetFollow's are objects to so they need to be
// moved in this case by the Player's position
void
OffsetFollow::Update(ObjectModel* Controller) {
// we are taking our camera view point from a player controlled object
    glm::vec3 ControllerPos = Controller->GetPositon();
// provide some default systems but ideally use an overridden method
    btVector3 myback = Controller->Forward * -1.0f;
    glm::vec3 behind = glm::vec3(myback.x(), myback.y(), myback.z());

    CameraPos = ControllerPos + (behind*13.0f);
//    CameraPos.y += 80.0f; // raise it up
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    View = glm::lookAt(CameraPos, ControllerPos, Up);
//    glm::vec3 LightPos = glm::vec3(-10.0/2.0, 351.0/4.0, 120.0/2);
//    View = glm::lookAt(LightPos, glm::vec3(0,0,0), Up);

// sort out the frustum for culling
    this->TheFrustum.Build(this->FarPlane, Projection, View);
}

