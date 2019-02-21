#include "MD2Model.h"
#include "Game.h"
#include <algorithm>
#include "Rays.h"


/************************************************************
Main consructor for an MD2 style mo
***********************************************************/
MD2Model::MD2Model(MyFiles* FH, const char *FN, ModelManager* MM) {
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    mTranslationMatrix	= glm::mat4(1.0f);
    mRotationMatrix		= glm::mat4(1.0f);
    mScaleMatrix		= glm::mat4(1.0f);
    Model	            = glm::mat4(1.0f);
    glm::vec3 Pos = glm::vec3(0.0f, 1.0f, 0.4f);
    SetPosition(Pos);
    this->MyPhysObj = nullptr;
    pAnimation = new MD2Anim();
    MM->LoadMD2(this, FN, FH);
    TheModelManager = MM; // our draw will need this

    Amb[0] = Amb[1] = 1.0f;
    Amb[2] = 1.0f;
    Amb[3] = 1.0f;
    Scales = glm::vec3(0.2f);
    SetModelMatrix();
    pAnimation->SetSequence(0, 0, MD2Anim::HOLD);
    pAnimation->SetSequence("stand", this);
}


MD2Model::MD2Model() {
	mTranslationMatrix	= glm::mat4(1.0f);
	mRotationMatrix		= glm::mat4(1.0f);
	mScaleMatrix		= glm::mat4(1.0f);
    Model	            = glm::mat4(1.0f); // give the model I as an initialisation
	glm::vec3 Pos = glm::vec3(0.0f, 1.0f, 0.4f);
	this->SetPosition(Pos);
	Scales = glm::vec3(1.0f, 1.0f, 1.0f);
	SetModelMatrix();
	Colour = glm::vec4(Rand(1.0f), Rand(1.0f), Rand(1.0f), 1.0f);
    pAnimation = new MD2Anim();
    pAnimation->SetSequence("stand", this);
    this->MyPhysObj = nullptr;
    glGenVertexArrays(1, &vertexArrayID);
}


MD2Model::~MD2Model() {
    delete pAnimation;
    glDeleteVertexArrays(1, &vertexArrayID);// G.S.
}


// Not all models actually load their skin
bool
MD2Model::LoadSkin(const char* FN, ModelManager* MM) {
	 texture1 =  MM->TexManager.LoadandCreateTexture(FN);			
     return texture1 != GLuint(-1);
}


bool
MD2Model::Update() {
// Set up model view
    Rotations.x = DEG2RAD(DegreeRotations.x);
	Rotations.y = DEG2RAD(DegreeRotations.y);
	Rotations.z = DEG2RAD(DegreeRotations.z);
	mScaleMatrix = glm::mat4(1.0f);
	mScaleMatrix = glm::scale(mScaleMatrix, Scales);
	MakeRotationMatrix();
	MakeTranslationMatrix();
	MakeModelMatrix(); 
	glUseProgram(this->programObject);	
    GLint AmbID = glGetUniformLocation(programObject, "Ambient");
	glUniform4fv(AmbID, 1, &Amb[0]);
	return true;
}


bool
MD2Model::Draw() {
    glBindVertexArray(vertexArrayID);
    glUseProgram(programObject);
	
    if(glGetError() != GL_NO_ERROR) {
		printf("Oh bugger MD2Model Draw error\n");
		return false;
	}

    if(MyPhysObj != nullptr) {
		btScalar transform[16]; // physics uses a 16 float matrix syste		
		btTransform Trans;
// Now get transform from Bullet which has been converted to an OpenGL matrix format
        MyPhysObj->GetTransform(transform);
        WorldPosition.x = transform[12];
        WorldPosition.y = transform[13];
        WorldPosition.z = transform[14]; // don't need the w.
// Work out forward and Right
		btQuaternion orient = this->MyPhysObj->GetRigidBody()->getOrientation();	
		Forward = btVector3(1, 0, 0);
		Forward = quatRotate(orient, Forward);
		Right = btVector3(0, 0, 1);
		Right = quatRotate(orient, Right);
// Transfer the OpenGL (rotation and position) to Model matrix and mult by scale for a full Model Matrix
        for(int i=0; i<16; i++) // we now transfer our physics matrix
            Model[i/4][i%4] = transform[i]; // to our game matix for place and view
		mScaleMatrix = glm::mat4(1.0f);
		mScaleMatrix = glm::scale(mScaleMatrix, Scales); // nice easy function
		glm::mat4 T = Model*mScaleMatrix;
		Model = T;
	}

// do a frustum check (Not Working)
//    if(TheGame->GetCamera()->TheFrustum.TestSphere(this->WorldPosition, 1.0f) == false) {
//        printf("I got Culled\n");
//        return false;
//    }
	
// test for an occulsion
	RayReturn RayValues;
    if(this->WhatAmI == ObjectModel::e_GUARD) {
		Rays Rays;
		RayValues.WasItStatic = false;
		ObjectModel* Player = TheGame->MyObjects[1];
		btVector3 Position  = btVector3(Player->WorldPosition.x, Player->WorldPosition.y, Player->WorldPosition.z);
		btVector3 PPPosition  = btVector3(WorldPosition.x, WorldPosition.y, WorldPosition.z);
		btVector3 Direction = PPPosition - Position;
		Direction.normalize();
		Position = Position + (Direction*3); // move it to the edge of the capsule, if the ray starts inside it can be random
		bool test = Rays.Raycast(TheGame->DynamicPhysicsWorld,
                                 Position,
                                 PPPosition,
                                 RayValues
                    );
// We'll always get a true, because we are going to at least hit the model,
// but we can test for a static, or a specific type.
        if(test && RayValues.WasItStatic) {// if there is a wall between our camera and it
            TheGame->m_pPhysicsDrawer->drawLine(Position,
                                                RayValues.WhereWeHit,
                                                btVector3(1.0f, 0.0f, 0.0f)
                                       ); // this is the contact ray
			return false;
		}
        this->TheGame->m_pPhysicsDrawer->drawLine(Position, RayValues.WhereWeHit, btVector3(1.0f, 1.0f, 1.0f)); // this is the contact ray
	}
	glm::mat4* Projection = TheGame->GetCamera()->GetProjection(); 
    glm::mat4* View       = TheGame->GetCamera()->GetView();
// make the MVP and MV
    glm::mat4 MVP = *Projection * *View * Model; // Remember order seems backwards
    glm::mat4 MV  =  *View * Model;	// for the light shader
// find the handles for our common Uniforms	
    GLint MatrixID = glGetUniformLocation(programObject, "MVP");
    samplerLoc     = glGetUniformLocation(programObject, "s_texture");
    GLint AmbID	   = glGetUniformLocation(programObject, "Ambient");
// set up the common attributes
    positionLoc     = glGetAttribLocation(programObject, "a_position");
    texCoordLoc     = glGetAttribLocation(programObject, "a_texCoord");
    NextpositionLoc = glGetAttribLocation(programObject, "a_NextPosition");
    GLint IsLitFlag = glGetUniformLocation(programObject, "Lit");
    glUniform1f(IsLitFlag, 0.0f); // send the unlit value 1.0 for lit, 0 for unlit

    if(glGetError() != GL_NO_ERROR) {
		printf("Oh bugger MD2Model Draw error\n");
		return false;
	}
		
// there are some attribs and uniforms we only have with a light shader	
// if we are using a light shader we also need to set up other values we can try to get
// them, if they are not there it will come back as -1
    GLint MatrixID2 = glGetUniformLocation(programObject, "MV");
	NormalLoc = glGetAttribLocation(programObject, "a_normal");
    NextNormalLoc = glGetAttribLocation(programObject, "a_NextNormal");
// we need to give info on where the light source is.	
	glm::vec3 LightPos = glm::vec3(1110.0, 1110.0, 1110.0); // hard coded here // light is to the right
	GLint WhereIsLight = glGetUniformLocation(programObject, "LightPos");	
	glUniform3fv(WhereIsLight, 1, &LightPos[0]);
	GLint WhereIsTime = glGetUniformLocation(programObject, "u_Time");	
    glUniform1f(WhereIsTime,
                ((pAnimation->TimeStep-pAnimation->Time)/pAnimation->TimeStep));
    // make sure it never goes out of range

// send our known uniform data to the shader.
// Send our transformation to the currently bound shader, in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MV[0][0]);	
	glUniform4fv(AmbID, 1, &this->Amb[0]);
// ok we're ready to draw something. 
    GLuint addr = TheMD2Data ->MD2meshes[ulong(pAnimation->CurrentFrame)].vbo;
	glBindBuffer(GL_ARRAY_BUFFER, addr); // this is now the buffer we work with
    GLuint addr2 = TheMD2Data->MD2meshes[ulong(pAnimation->NextFrame)].vbo;
    if (glGetError() != GL_NO_ERROR) {
		printf("Oh bugger MD2Model Draw error\n");
		return false;
	}
    GLsizei stride = (3+2)*sizeof(GLfloat); // we have 3 ver, and 2 tc
//load the vertex data info
    glVertexAttribPointer(GLuint(positionLoc),
                          3,	   // there are 3 values xyz
                          GL_FLOAT,// they a float
                          GL_FALSE,// don't need to be normalised
                          stride,  // how many floats to the next one
                          nullptr  // where do they start as an index); // use 3 values, but add stride each time to get to the next
    );
    if(glGetError() != GL_NO_ERROR) {
		printf("Oh bugger MD2Model Draw error\n");
		return false;
	}

// Load the normal coordinate
    glVertexAttribPointer(GLuint(NormalLoc),
                          3, // there are 2 values
                          GL_FLOAT, //they are floats
                          GL_FALSE, // we don't need them normalised
                          stride,  // whats the stride to the next ones
                          reinterpret_cast<void*>(sizeof(float) * 3)
    );

// Load the texture coordinate
    glVertexAttribPointer(GLuint(texCoordLoc),
                          2, // there are 2 values
                          GL_FLOAT, //they are floats
                          GL_FALSE, // we don't need them normalised
                          stride,  // whats the stride to the next ones
                          reinterpret_cast<void*>(sizeof(float)*3)  //(GLfloat*)this->Vertices + 9 // where is the 1st one
    );
// bind the correct texture for this frame
    if(TheMD2Data->MD2meshes[ulong(pAnimation->CurrentFrame)].material_id < materials.size()) {
        std::string diffuse_texname = materials[TheMD2Data->MD2meshes[ulong(pAnimation->CurrentFrame)].material_id].diffuse_texname;
        if(TheModelManager->TexManager.Totaltextures.find(diffuse_texname) !=
           TheModelManager->TexManager.Totaltextures.end())
		{
            glBindTexture(GL_TEXTURE_2D, TheModelManager->TexManager.Totaltextures[diffuse_texname]);
		}
        else {// if we have no textures use texture1, but really to get here there has been an error
			glBindTexture(GL_TEXTURE_2D, texture1);
        }
	}
    else {// if we have no textures use texture 1
        glBindTexture(GL_TEXTURE_2D, texture1);
    }
    glEnableVertexAttribArray(GLuint(NormalLoc));
    glEnableVertexAttribArray(GLuint(positionLoc));
    glEnableVertexAttribArray(GLuint(texCoordLoc));
// set the other buffer up
    glBindBuffer(GL_ARRAY_BUFFER, addr2);// this is now the buffer we work with
//load the vertex data info
    glVertexAttribPointer(GLuint(NextpositionLoc),
                          3,	   // there are 3 values xyz
                          GL_FLOAT,// they a float
                          GL_FALSE,// don't need to be normalised
                          stride,  // how many floats to the next one
                          nullptr  // where do they start as an index); // use 3 values, but add stride each time to get to the next
    );
    glEnableVertexAttribArray(GLuint(NextpositionLoc));

	glUniform1i(this->samplerLoc, 0);
    int numberTri = TheMD2Data->MD2meshes[uint(pAnimation->CurrentFrame)].numTriangles * 3;
	glDrawArrays(GL_TRIANGLES, 0, numberTri);

    if(glGetError() != GL_NO_ERROR) {
		printf("Oh bugger MD2Model Draw error\n");
		return false;
	}
	
    glDisableVertexAttribArray(GLuint(positionLoc));
    glDisableVertexAttribArray(GLuint(texCoordLoc));
//	glDisableVertexAttribArray(NormalLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}


// renders to the height map in our frame buffer
bool
MD2Model::DrawShadow() {
    glBindVertexArray(vertexArrayID);
    glUseProgram(TheGraphics->ShadowShader);
// set up the framebuffer
// our view point is from the camera
    glm::mat4 View = glm::lookAt(this->TheGraphics->LightPos,
                                 this->TheGraphics->LightTarget,
                                 glm::vec3 (0,1,0)// pointing up( 0,-1,0 will be upside-down));
                     );
// use the model from the last version that was made.
    glm::mat4	LightProjection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 600.0f);
    glm::mat4 LightMVP    = LightProjection * View * Model;  // Remember order seems backwards
    GLint MatrixID =	glGetUniformLocation(TheGraphics->ShadowShader, "u_lightMVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &LightMVP[0][0]);
// set up the common attributes
    positionLoc = glGetAttribLocation(TheGraphics->ShadowShader, "a_position");
// ok we're ready to draw something.
    GLuint addr = TheMD2Data->MD2meshes[ulong(pAnimation->CurrentFrame)].vbo;
    glBindBuffer(GL_ARRAY_BUFFER, addr);  // this is now the buffer we work with
// proceed
    GLsizei stride = (3 + 2)*sizeof(GLfloat);  // we have 3 ver, and 2 tc
    //load the vertex data info
    glVertexAttribPointer(GLuint(positionLoc),
                          3,			// there are 3 values xyz
                          GL_FLOAT,	// they a float
                          GL_FALSE,	// don't need to be normalised
                          stride,		// how many floats to the next one
                          nullptr  // where do they start as an index); // use 3 values, but add stride each time to get to the next
    );

    glEnableVertexAttribArray(GLuint(positionLoc));

    int numberTri = TheMD2Data->MD2meshes[ulong(pAnimation->CurrentFrame)].numTriangles * 3;
    glDrawArrays(GL_TRIANGLES, 0, numberTri);
    if(glGetError() != GL_NO_ERROR) {
        printf("Oh bugger MD2Model Shadow Draw error\n");
        return false;
    }

    glDisableVertexAttribArray(GLuint(positionLoc));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}


