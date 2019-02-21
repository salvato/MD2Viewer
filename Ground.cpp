// This is only here to create and display the environent

#include "Ground.h"
#include "MyFiles.h"


Ground::Ground() {
}


Ground::~Ground() {
}


Ground::Ground(MyFiles* FH, const char *FN, ModelManager* MM) {
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    MM->LoadOBJModel(this, FH, FN, &CollisionMesh);
	TheModelManager = MM; // our draw will need this
// Make a mesh for the terrain
    btTriangleMesh* triangleMeshTerrain = new btTriangleMesh();
// Load it up with the vertices from our load systems
    for(uint i=0; i<CollisionMesh.size(); i+=3)	{
        triangleMeshTerrain->addTriangle(CollisionMesh[i],
                                         CollisionMesh[i+1],
                                         CollisionMesh[i+2]);
	}
// store for when we make a new object
    mesh = new btBvhTriangleMeshShape(triangleMeshTerrain, true);
    CollisionMesh.clear(); // We are done with this now
}
