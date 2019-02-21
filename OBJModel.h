#pragma once

#include "Graphics.h"
#include "ObjectModel.h"
#include "ModelManager.h"


// This is the general model class
// for an OBJ loaded using TinyOBJLoader
class OBJModel : public ObjectModel
{
public:
	OBJModel();
    OBJModel(MyFiles* FH, const char* FN, ModelManager* MM);
    virtual ~OBJModel();

public:
    bool Update(); // we supply an update
	bool Draw(); // and a draw 
	bool DrawShadow(); // we can gather shadows
	void SetAmbient(glm::vec4);
	
public:
    ModelManager* TheModelManager;	// for locating the textures and materials
	float Amb[4];					// not always used but can be useful
	glm::vec3 Speed;
    GLuint vertexArrayID;
};

