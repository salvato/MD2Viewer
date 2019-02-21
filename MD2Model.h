#pragma once

#include "ObjectModel.h"
#include "ModelManager.h"
#include "MD2.h"
#include "MD2Anim.h"


class MD2Model : public ObjectModel
{
public:
    MD2Model();
    MD2Model(MyFiles* FH, const char* FN, ModelManager* MM);
    ~MD2Model();

public:
    bool LoadSkin(const char *FN, ModelManager* MM);
    bool Update(); // we supply an update
    bool Draw(); // and a draw
    bool DrawShadow(); // and an extra feature for md2's

    MD2Anim* pAnimation;
    int MDLIndex; //
    ModelManager* TheModelManager;	 // for locating the textures and materials
    float Amb[4];// = { Rand(1.0f), Rand(1.0f), Rand(1.0f), 1.0f };
    glm::vec3 Speed;

protected:
    GLuint vertexArrayID;
    GLint NextpositionLoc;
    GLint NextNormalLoc;
};
