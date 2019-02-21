#pragma once

// this is a simple offset follow camera, it takes the controlling players
#include "Camera.h"

class OffsetFollow: public Camera
{
public:
    OffsetFollow(); // use
    OffsetFollow(float screenRatio);
    ~OffsetFollow() {}

public:
    // we are not going to use the usual update
    void Update(ObjectModel* ControllingObject);

public:
};
