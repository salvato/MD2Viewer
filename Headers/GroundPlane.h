#pragma  once

//============
// GroundPlane
//============

// A very simple recursive system to create a mesh
// for a groundplane (or any flat mesh)

#include <GL/glew.h>
#include <vector>
#include <stdio.h>
#include "ObjectModel.h"
#undef countof
// Include GLM
#include "glm/glm.hpp"

#include "Camera.h" // we need a camera

// define the number of iterations of recursion which will increase
// the overall number of verts 0 is fine for a single quad,
// proper terrain will need potentially a lot more
#define MAX_TRIES 0


class GroundPlane :public ObjectModel
{
public:
    void Init(Graphics*);
    void Update(Camera*);
    void Generate(void);
// we need these to provide the virtual updates
    bool Update(); // not going to use this
    bool Draw(); // its our render funciton

private:
    void SubTriangle(int Level, float* Vert1, float* Vert2, float* Vert3);

private:
    GLuint	GroundTexture;
    GLuint	GroundShader;
// shader values (some common)
    GLuint	MatrixID;
// specific to texture/colour based shaders
    GLuint	AmbID;
    glm::mat4 MVP;
    std::vector<float> MeshVertices;
    int TotalVerts = 0; // a useful debug check
    int TotalTriangles = 0;
    int TotalMemory = 0;
};


