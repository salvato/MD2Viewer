// simple Quad display for images on screen
// uses an ortho projection and is best suited to fbo or texture display
#include "Quad.h"


Quad::Quad() {
}


Quad::~Quad() {
};


// simple process like a font
bool
Quad::Update() {
    return true;
}


bool
Quad::Draw() {
    GLfloat RectVertices[] =
    {// data has to be: Position and then Normalized texture coords.
         -0.5f,  0.5f,  0.0f,// Position 0
          0.0f,  0.0f,       // TexCoord 0

         -0.5f, -0.5f,  0.0f,// Position 1
          0.0f,  1.0f,       // TexCoord 1

          0.5f, -0.5f,  0.0f,// Position 2
          1.0f,  1.0f,       // TexCoord 2

          0.5f,  0.5f,  0.0f,// Position 3
          1.0f,  0.0f        // TexCoord 3
    };
    (void)RectVertices;
    return true;
}
