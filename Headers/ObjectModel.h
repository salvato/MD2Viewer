
/*
This is our most basic class of 3D model, it is set up to cope with OBJ's, MD2's or simple vertex lists
Depending on the format of the model some varaibles/features are not used but thats acceptable. any extreme
functionality should be included in the derived class 
*/

#pragma once
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof
#include "MyFiles.h"
#include "ShaderManager.h"
#include <GL/glew.h>
#include  "tiny_obj_loader.h"
#include "MD2.h"

#define BULLET
#include "PhysicsObj.h"


#define PI			3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f
#define DEG2RAD(x) (x*PI)/180.0f
#define RAD2DEG(x) x*(180.0f/PI)

class Graphics;
class Game;

inline float
Rand(float a_Range) {
    return (float(rand())/RAND_MAX)*a_Range;
}


class ObjectModel 
{
public:
    ObjectModel();
    ObjectModel(char* filename, MyFiles* FH);
    virtual ~ObjectModel();

public:
    typedef enum {
        e_GROUND = 0,
        e_PLAYER,
        e_PATROL,
        e_GUARD,
        e_SIMPLE
    } ObjectType;

    virtual bool Update() = 0; // we need to have an Update to move and create the ModelView
    virtual bool Draw() = 0; // and we need to draw from the vertices
    virtual bool DrawShadow() {return true;} // allow this to be overriden but to do nothing if not defined, so this becomes a default no operation
    // used by OBJ models, left as empty when not needed
    typedef struct {
        GLuint vb;  // vertex buffer
        int numTriangles;
        size_t material_id;
    } DrawObject;
    // passed and maybe removed once used
    std::vector<tinyobj::material_t> materials;  // material info for the model for an OBJ or MD2
    std::vector<DrawObject> gdrawObjects; 	 // this is the draw objects needed for the mode
    // used by MD2's
    MD2DataContainer* TheMD2Data;
    // used by simple attribyte array systems
    GLvoid*	 Vertices; // a pointer to our attribute array, which should contain vertices and texture coords
    // a few basic utility functions
    glm::vec3	GetPositon();
    void		SetPosition(glm::vec3*);
    void		SetPosition(glm::vec3);

    glm::vec3	GetRotations();
    void		SetRotations(glm::vec3*);
    void		SetRotations(glm::vec3);

    void		SetXRotation(float); // if you need to set indivual rotations
    void		SetYRotation(float);
    void		SetZRotation(float);

    void		SetModelMatrix(); // initialises and resets
    void	 	MakeModelMatrix(); // makes and returns the model matrix
    void		MakeRotations();
    void		MakeTranslationMatrix();
    void		StoreGraphicClass(Graphics* Graphics);

    GLuint texture1; // a handle
    bool		LoadModel(GLvoid* vertices);
    bool		LoadOBJModel(char* fname);
    bool		LoadMD2Model(char* fname);

    glm::vec4	Colour;
    glm::vec3	WorldPosition;  // where am I in the world?

    glm::vec3	Rotations;		// what are my rotations? Stored as Radians!
    glm::vec3	DegreeRotations;
    glm::vec3	Scales;

    glm::mat4 mTranslationMatrix;
    glm::mat4 mRotationMatrix;
    glm::mat4 mScaleMatrix;

    glm::mat4 RotationMatrixX;
    glm::mat4 RotationMatrixY;
    glm::mat4 RotationMatrixZ;

    void MakeRotationMatrix(); // since these get altered alot
    GLuint programObject, vertexShader, fragmentShader; // programObject used for draw. vertex and fragment used to create programObject

    glm::mat4 Model;     // the model matrix will effectivly hold all the rotation and positional data for the object
    GLint	positionLoc; // index handles to important concepts in texture
    GLint	NormalLoc;
    GLint	texCoordLoc;
    GLint	samplerLoc;
    GLint	samplerLoc2; // usually for shadow
    GLint	samplerLoc3;
    GLint	samplerLoc4;  // we can have several
    GLuint	MyLightShader;
    GLuint	MyMVHandle;

// Anything that uses a shader will need access to the Graphic class
// set up in the Game app. Make sure this is supplied
    Graphics* TheGraphics;
    Game* TheGame;
    GLuint	vbo;
    GLuint vertexArrayID;

// Bounding box info when we need to provide it, ModelManager does provide for OBJ types.
    float bmin[3];
    float bmax[3];

    virtual void HandleCollision(const ObjectModel *);
    virtual void HandleSeparation(const ObjectModel*);
    virtual void HandleContact(const ObjectModel* WhatDidIHit);
    ObjectType	WhatAmI;

    PhysicsObj* MyPhysObj;
    btVector3 Forward;
    btVector3 Right;
    btCollisionShape* MyShape;

    typedef	enum {
        BOX,
        CAPSULE,
        SPHERE,
        CYLINDER // add others if you want
    } ShapeTypes;
    btCollisionShape* CreateMyShape(ShapeTypes);
};
/*
 * OpenGL provides an object that stores all of the state needed for rendering:
 * the Vertex Array Object (VAO)
 * VAOs are created with the glGenVertexArray function.
 * You can create multiple objects with one call. The objects are GLuints.
 *      glGenVertexArrays(1, &vao);
 * creates a single VAO, which contains the vertex array state that we have to set.
 *      glBindVertexArray(vao);
 * set the active VAO.
 * The following functions change VAO state. Therefore, if no VAO is bound to the context
 * (if you call glBindVertexArray(0) or you do not bind a VAO at all),
 *  all of these functions, except as noted, will fail.
 *
 * glVertexAttribPointer(),
 * glVertexAttribIPointer()
 * glEnableVertexAttribArray()
 * glDisableVertexAttribArray().
 *
 * glBindBuffer(GL_ELEMENT_ARRAY_BUFFER) instead will not fail.

 */
