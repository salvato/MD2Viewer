//****************************************************
// A base class, containing only information needed to
// position/rotate and create a model.
//****************************************************

#include "ObjectModel.h"
#include "Graphics.h"
#include <bullet/btBulletDynamicsCommon.h>


ObjectModel::ObjectModel() {
    glGenVertexArrays(1, &vertexArrayID);
}


ObjectModel::~ObjectModel() {
    glDeleteVertexArrays(1, &vertexArrayID);// G.S.
}


// Set up from an already established set of vertices
bool
ObjectModel::LoadModel(GLvoid* a_Vertices) {
    this->Vertices = a_Vertices;
    return true;
}


// Make the Model matrix by loading
// the rotations/positions/scale matrices
void
ObjectModel::SetModelMatrix() {
//set the matrices we use to I
    mTranslationMatrix	= glm::mat4(1.0f);
    mRotationMatrix		= glm::mat4(1.0f);
    mScaleMatrix		= glm::mat4(1.0f);
// set scale matrix (build a scale matrix created from 3 scalars)
    mScaleMatrix = glm::scale(mScaleMatrix, Scales); // nice easy function
// set Rotation Matrix
    MakeRotationMatrix(); // this is a bit complex so go do them
// set translation
    mTranslationMatrix	= glm::translate(mTranslationMatrix, WorldPosition); //use the glm translate
// now make the Model for use in render
    Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
}


void
ObjectModel::MakeTranslationMatrix() {
    mTranslationMatrix = glm::mat4(1.0f);
    mTranslationMatrix[3][0] = WorldPosition.x;
    mTranslationMatrix[3][1] = WorldPosition.y;
    mTranslationMatrix[3][2] = WorldPosition.z;
}


// Do the rotation matrices
void
ObjectModel::MakeRotationMatrix() {
    // set rotationx
    RotationMatrixX		= glm::mat4(1.0f);
    RotationMatrixX[1][1] = cosf(Rotations.x);
    RotationMatrixX[2][1] =-sinf(Rotations.x);
    RotationMatrixX[1][2] = sinf(Rotations.x);
    RotationMatrixX[2][2] = cosf(Rotations.x);
    //set rotationy
    RotationMatrixY		= glm::mat4(1.0f);
    RotationMatrixY[0][0] = cosf(Rotations.y);
    RotationMatrixY[2][0] = sinf(Rotations.y);
    RotationMatrixY[0][2] =-sinf(Rotations.y);
    RotationMatrixY[2][2] = cosf(Rotations.y);
    //set rotationz
    RotationMatrixZ		= glm::mat4(1.0f);
    RotationMatrixZ[0][0] = cosf(Rotations.z);
    RotationMatrixZ[1][0] =-sinf(Rotations.z);
    RotationMatrixZ[0][1] = sinf(Rotations.z);
    RotationMatrixZ[1][1] = cosf(Rotations.z);
// Now the combined rotation
    mRotationMatrix = RotationMatrixX*RotationMatrixY*RotationMatrixZ;
}


// Simply create the model matrix, assumes
// the translations and others have been set
void
ObjectModel::MakeModelMatrix() {
    Model = mTranslationMatrix * mRotationMatrix * mScaleMatrix;
}


// Return the position info from worldposition
glm::vec3
ObjectModel::GetPositon() {
    return WorldPosition;
}


// Set the word position
void
ObjectModel::SetPosition(glm::vec3* a_Pos) {
    WorldPosition.x = 	a_Pos->x;
    WorldPosition.y = 	a_Pos->y;
    WorldPosition.z = 	a_Pos->z;
}


// Set the world position
void
ObjectModel::SetPosition(glm::vec3 a_Pos) {
    WorldPosition = a_Pos;
}


glm::vec3
ObjectModel::GetRotations() {
    return glm::vec3(Rotations.x, Rotations.y, Rotations.z);
}


void
ObjectModel::SetRotations(glm::vec3* a_Rots) {
    Rotations.x = a_Rots->x;
    Rotations.y = a_Rots->y;
    Rotations.z = a_Rots->z;
}


void
ObjectModel::SetRotations(glm::vec3 a_Rots)	 {
    Rotations = a_Rots;
}


void
ObjectModel::SetXRotation(float a_x) {
    Rotations.x = a_x;
}


void
ObjectModel::SetYRotation(float a_y) {
    Rotations.y = a_y;
}


void
ObjectModel::SetZRotation(float a_z) {
    Rotations.z = a_z;
}


void
ObjectModel::StoreGraphicClass(Graphics* Graphics) {
    TheGraphics = Graphics;
}


btCollisionShape*
ObjectModel::CreateMyShape(ShapeTypes shape) {
    btVector3 vBox = btVector3((bmax[0]-bmin[0])/2,
                               (bmax[1]-bmin[1])/2,
                               (bmax[2]-bmin[2])/2);
    btCollisionShape* pShape= nullptr;
    switch(shape) {
        case BOX: {
            pShape = new btBoxShape(vBox);
            break;
        }
        case CAPSULE: {
            pShape = new btCapsuleShape(vBox.x(), vBox.y());
            break;
        }
        case SPHERE: {
            pShape = new btSphereShape(vBox.y() * 2);
            break;
        }
        case CYLINDER: {
            pShape = new btCylinderShape(vBox);
            break;
        }
    }
    return pShape;
}


// These are collision response systems,
// ObjectModel should not handle the logic
void
ObjectModel::HandleCollision(const ObjectModel* whohitme) {
    (void)whohitme;
    // really this should be over ridden
//     printf("Base Class reports hit\n");
}


void
ObjectModel::HandleSeparation(const ObjectModel* whohitme) {
    (void)whohitme;
    // really this should be over ridden
//     printf("Base Class reports seperation\n");
}


// A contact object may also include an object reported as a collision.
// It is up to the logic to decide if that's relevent
void
ObjectModel::HandleContact(const ObjectModel *whohitme) {
    (void)whohitme;
    // really this should be over ridden
//     printf("Base Class reports contact\n");
}
