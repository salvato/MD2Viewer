#pragma once

#include "CubeModel.h"

class MyFiles;

class SkyBox : public CubeModel
{
public:
    SkyBox(MyFiles* FH);
    ~SkyBox();

public:
    bool   Draw();
    bool   Update(); // we supply an update
    bool   Init();

private:
    void setupCubeMap(MyFiles* FH);
    void loadCubemap(std::vector<const GLchar*> faces, MyFiles* FH);

private:
    GLint PVM;
    GLint AmbID;
    GLint skyBoxLoc;
    GLint vertexLoc;
};
