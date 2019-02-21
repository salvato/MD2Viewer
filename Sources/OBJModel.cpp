#include "OBJModel.h"
#include "Game.h"
#include "bullet/LinearMath/btQuaternion.h"

#define DEBUGWANTED 0

/***********************************************************************
Standard OBJ model class using models loaded by TinyOBJLoader
Tiny OBJLoader always gives 11 float values per face:
3 x vertices
3 x normals
3 x colour (currently not used)
2 x tex coords

All shaders using this must therefore allow for an attribute for:
PosLoc  (3x vertices)
NormalLoc (3x normals)
texCoordLoc (2x tex coords)

The 3 floats for colour are currently wasted and might find a use later,
but current shaders are going to ignore the colour value;
***********************************************************************/


/************************************************************
Main consructor for an OBJ style ship
************************************************************/
OBJModel::OBJModel(MyFiles* FH, const char* FN, ModelManager* MM)
    : OBJModel()
{
    MM->LoadOBJModel(this, FH, FN);
    TheModelManager = MM; // our draw will need this
}


// set up the Model's basic matrices
OBJModel::OBJModel()
{
    mTranslationMatrix	= glm::mat4(1.0f);
	Amb[1] = Colour.g;
	Amb[2] = Colour.b;
	Amb[3] = 1.0f;
	DegreeRotations = glm::vec3(0);
    glGenVertexArrays(1, &vertexArrayID);
    this->MyPhysObj = nullptr;
}


OBJModel::~OBJModel() {
}


void
OBJModel::SetAmbient(glm::vec4 AmbColour) {
	Amb[0] = AmbColour.r;
	Amb[1] = AmbColour.g;
	Amb[2] = AmbColour.b;
	Amb[3] = AmbColour.a;
}


// update routine mainly handles the MVP
// if using a light shader also can decide on distance
// to avoid it and choosing whether to use a light shader
// or not.

bool
OBJModel::Update() {
	glUseProgram(programObject);
#ifndef BULLET
	int* MyPhysObj = NULL; // set up a dummy version to allow conditions to work
#endif // !BULLET
	
    if (MyPhysObj == nullptr) {
// if we are using a physics movement system from here....		
		Rotations.x = glm::radians(DegreeRotations.x);
		Rotations.y = glm::radians(DegreeRotations.y);
		Rotations.z = glm::radians(DegreeRotations.z);

		mScaleMatrix = glm::mat4(1.0f);
		mScaleMatrix = glm::scale(mScaleMatrix, Scales); // nice easy function
	
		MakeRotationMatrix();
		MakeTranslationMatrix();
		MakeModelMatrix(); 
	}
    else {
		glm::vec2 worldpos2D = glm::vec2(100, 100);
		glm::vec2 speed = glm::vec2(5, 5);
		glm::vec2 newpos = worldpos2D + speed;
        (void) newpos;
		btScalar transform[16]; // physics uses a 16 float matrix syste
		this->MyPhysObj->GetTransform(transform);
		btQuaternion orient = this->MyPhysObj->GetRigidBody()->getOrientation();	
		this->WorldPosition.x = transform[12];
		this->WorldPosition.y = transform[13];
		this->WorldPosition.z = transform[14]; // don't need the ww.
	
		Forward = btVector3(0, 0, -1);
		Forward = quatRotate(orient, Forward);
	
		Right = btVector3(-1, 0, 0);
		Right = quatRotate(orient, Right);
		//	btMatrix3x3 rotationMatrix(orient);
		//	Forward = btVector3(rotationMatrix.getColumn(2)); // we will keep this its useful for all sorts
        for(int i=0; i<16; i++) // we now transfer our physics matrix
            Model[i/4][i%4] = transform[i];
		
		mScaleMatrix = glm::mat4(1.0f);
		mScaleMatrix = glm::scale(mScaleMatrix, Scales); // nice easy function
		glm::mat4 T = Model*mScaleMatrix;
		Model = T;
	}

    glm::mat4* Projection = TheGame->GetCamera()->GetProjection();
	glm::mat4* View = TheGame->GetCamera()->GetView();
	// make the MVP and MV if using light (light does not need projection)
	glm::mat4 MVP        = *Projection * *View * Model; // Remember order seems backwards
//	glm::mat4 MV		 =  *View * Model;	// for the light shader
// find the handles for our Uniforms	
	GLint MatrixID		=	glGetUniformLocation(programObject, "MVP");
	this->samplerLoc	=	glGetUniformLocation(programObject, "s_texture"); // the normal diffuse texture
	this->samplerLoc2	=	glGetUniformLocation(programObject, "u_s2dShadowMap"); // the shadow height map 
	GLint	AmbID		=	glGetUniformLocation(programObject, "Ambient"); 
	GLint LightMatrixID =	glGetUniformLocation(programObject, "u_LightsMVP");
	GLint IsLitFlag =	glGetUniformLocation(programObject, "Lit");
//find the handles for our attributes and store
	positionLoc = glGetAttribLocation(programObject, "a_position");
	texCoordLoc = glGetAttribLocation(programObject, "a_texCoord");
// set up lights MVP
	glm::mat4	LightProjection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 600.0f);
	glm::mat4	LightView  = 	glm::lookAt(this->TheGraphics->LightPos ,
								this->TheGraphics->LightTarget,
								glm::vec3(0, 1, 0)// pointing up( 0,-1,0 will be upside-down));
										);
	glm::mat4 LightMVP    = LightProjection * LightView * Model;    // Remember order seems backwards
// send the uniforms to the PO	
	glUniformMatrix4fv(LightMatrixID, 1, GL_FALSE, &LightMVP[0][0]);
	glUniform1f(IsLitFlag, 0.0f); // send the unlit value 1.0 for lit, 0 for unlit
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniform4fv(AmbID, 1, &this->Amb[0]);
// good to check here
    if (glGetError() != GL_NO_ERROR) {
		printf("Oh bugger Ship Model Draw error\n");
		return false;
	}
    glUseProgram(0);
    return true;
}


bool
OBJModel::Draw() {
    glBindVertexArray(vertexArrayID);
// Use the program object
    glUseProgram(programObject);
// TinyOBJLoader creates gdrawObjects, and sets up VB's iterrate through them
    ulong numberOfFrames = gdrawObjects.size(); // its useful in a variable in case we need to test it.
	
    for(ulong frames=0; frames<numberOfFrames; frames++) {
		DrawObject o = gdrawObjects[frames]; // get the DrawObject which has the vb
		
		glBindBuffer(GL_ARRAY_BUFFER, o.vb); // this is now the buffer we work with
		GLsizei stride = (3 + 3 + 3 + 2)*sizeof(GLfloat); // we have 3x vert, 3x normal,3xcolour,2x texcoord.
//load the vertex data info
        glVertexAttribPointer(GLuint(positionLoc),
                              3,	// there are 3 values xyz
                              GL_FLOAT, // they a float
                              GL_FALSE, // don't need to be normalised
                              stride,  // how many floats to the next one
                              nullptr  // where do they start as an index);
                              // use 3 values, but add stride each time to get to the next
        );
// Load the texture coordinate
        glVertexAttribPointer(GLuint(texCoordLoc),
                              2, // there are 2 values
                              GL_FLOAT, //they are floats
                              GL_FALSE, // we don't need them normalised
                              stride,  // whats the stride to the next ones
                              reinterpret_cast<const void*>(sizeof(float) * 9)  //(GLfloat*)this->Vertices + 9 // where is the 1st one);
        );
        glEnableVertexAttribArray(GLuint(positionLoc));
        glEnableVertexAttribArray(GLuint(texCoordLoc));
// probably don't need to do this.
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->TheGraphics->ShadowTexture);
		// bind the correct texture for this shape	
        if((o.material_id < materials.size())) {
			std::string diffuse_texname = materials[o.material_id].diffuse_texname;
            if(this->TheModelManager->TexManager.Totaltextures.find(diffuse_texname) !=
               this->TheModelManager->TexManager.Totaltextures.end())
			{
//		char *cstr = &diffuse_texname[0u]; // embarrasing side effect of the decision to use char
//		printf("binding texture %s and %d\n", cstr, this->TheModelManager->TexManager.Totaltextures[diffuse_texname]);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, this->TheModelManager->TexManager.Totaltextures[diffuse_texname]); // this->TheGraphics->ShadowTexture to see the shadow
			}
		}

        glUniform1i(this->samplerLoc,  0);   // tell samplerloc to use texture 0 which is set up
        glUniform1i(this->samplerLoc2, 1);    // tell samplerloc2 to use texture 1 which will be an FBO
		glDrawArrays(GL_TRIANGLES, 0, o.numTriangles);
#if DEBUGWANTED		
        if(glGetError() != GL_NO_ERROR) {
			printf("Oh bugger OBJ Model Draw error\n");
			return false;
		}
#endif	
	}	
// take these off the bind
    glDisableVertexAttribArray(GLuint(positionLoc));
    glDisableVertexAttribArray(GLuint(texCoordLoc));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);

    if(glGetError() != GL_NO_ERROR) {
		printf("Oh bugger ObjModel Draw error\n");
		return false;
	}
    glUseProgram(0);
    return true;
}


bool
OBJModel::DrawShadow() {
    return true;
    // Use the program object
    glUseProgram(TheGraphics->ShadowShader);
    // make the MVP, but use the lights position
    // our view point is from the camera
    glm::mat4 View = glm::lookAt(this->TheGraphics->LightPos,
                                 this->TheGraphics->LightTarget,
                                 glm::vec3(0, 1, 0)// pointing up( 0,-1,0 will be upside-down));
                     );
// use the model from the last version that was made.
    glm::mat4	LightProjection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 600.0f);
    glm::mat4	LightMVP    = LightProjection * View * Model;   // Remember order seems backwards
    GLint MatrixID =	glGetUniformLocation(TheGraphics->ShadowShader, "u_lightMVP");

    // set up the shader attributes and uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &LightMVP[0][0]);
    positionLoc = glGetAttribLocation(TheGraphics->ShadowShader, "a_position");

    // TinyOBJLoader creates gdrawObjects, and sets up VBO's, iterrate through them
    ulong numberOfFrames = gdrawObjects.size();  // its useful in a variable in case we need to test it.

    for(ulong frames=0; frames<numberOfFrames; frames++) {
        DrawObject o = gdrawObjects[frames];  // get the DrawObject which has the vb
        glBindBuffer(GL_ARRAY_BUFFER, o.vb);  // this is now the buffer we work with
        GLsizei stride = (3 + 3 + 3 + 2)*sizeof(GLfloat);  // we have 3x vert, 3x normal,3xcolour,2x texcoord.
        //load the vertex data info
        glVertexAttribPointer(GLuint(positionLoc),
                              3,	// there are 3 values xyz
                              GL_FLOAT,	 // they a float
                              GL_FALSE,	 // don't need to be normalised
                              stride, 	  // how many floats to the next one
                              nullptr  // where do they start as an index); // use 3 values, but add stride each time to get to the next
        );
        glEnableVertexAttribArray(GLuint(positionLoc));
        glDrawArrays(GL_TRIANGLES, 0, o.numTriangles);
    }
    // take these off the bind
    glDisableVertexAttribArray(GLuint(positionLoc));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (glGetError() != GL_NO_ERROR) {
        printf("Oh bugger ObjModel shadow Draw error\n");
        return false;
    }
    return true;
}



