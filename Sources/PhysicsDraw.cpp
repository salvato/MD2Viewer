#include "PhysicsDraw.h"
#include <GL/glew.h>


void
PhysicsDraw::drawLine(const btVector3 &from,
                      const btVector3 &to,
                      const btVector3 &color)
{
// draws a simple line of pixels between points but stores them for later draw
    LineValues Line;
    btVector3 scolor = btVector3(1, 0, 1);
    Line.p1 = from;
    Line.Colour1 = color;
    Line.p2 = to;
    Line.Colour2 = scolor;
    TheLines.push_back(Line);
}


void
PhysicsDraw::DoDebugDraw(){
// set up the line shader and then draw the buffer
//load the vertex data info
    if(TheLines.size() == 0) return;
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    glUseProgram(ProgramObject);
    GLuint LineBuffer;
    glGenBuffers(1, &LineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, LineBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 long(TheLines.size()*sizeof(float)),
                 &TheLines.at(0),
                 GL_STATIC_DRAW);
    positionLoc = glGetAttribLocation(ProgramObject, "a_position");
    ColourLoc = glGetAttribLocation(ProgramObject, "a_colour");
    int stride = 8*sizeof(float); // 4 floats for each btvec3
    glVertexAttribPointer(GLuint(positionLoc),
                          3,	// there are 3 values xyz
                          GL_FLOAT, // they are float
                          GL_FALSE, // don't need to be normalised
                          stride,  // how many floats to the next one(be aware btVector3 uses 4 floats)
                          nullptr);
    glVertexAttribPointer(GLuint(ColourLoc),
                          3,	   // there are 3 values rgb
                          GL_FLOAT,// they are float
                          GL_FALSE,// don't need to be normalised
                          stride,  // how many floats to the next one(be aware btVector3 uses 4 floats)
                          reinterpret_cast<void*>(4*sizeof(float)));
    glEnableVertexAttribArray(GLuint(positionLoc));
    glEnableVertexAttribArray(GLuint(ColourLoc));

    glDrawArrays(GL_LINES, 0, int(TheLines.size()*2)); // there are 2 components to send

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertexArrayID);
    TheLines.clear();
}


void
PhysicsDraw::drawContactPoint(const btVector3 &pointOnB,
                              const btVector3 &normalOnB,
                              btScalar distance,
                              int lifeTime,
                              const btVector3 &color)
{
    (void)distance;
    (void)lifeTime;
    drawLine(pointOnB, pointOnB + (normalOnB *2), color);
}


void
PhysicsDraw::ToggleDebugFlag(int flag) {
    // checks if a flag is set and enables/
    // disables it
    if (m_debugMode & flag)
        // flag is enabled, so disable it
        m_debugMode = m_debugMode & (~flag);
    else
        // flag is disabled, so enable it
        m_debugMode |= flag;
}
