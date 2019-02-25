#pragma once

#include "OBJModel.h"


// Simple class mainly here to allow us to control an
// OBJModel type Object and get its mesh
class Ground : public OBJModel
{
public:
    Ground();
    virtual ~Ground();

public:
    Ground(MyFiles* FH, const char* FN, ModelManager* MM);

public:
    std::vector<btVector3> CollisionMesh;
    btBvhTriangleMeshShape* mesh;
};

