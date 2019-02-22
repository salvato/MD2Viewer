#include "SkyBox.h"
#include "Game.h"


SkyBox::SkyBox(MyFiles* FH)
    : CubeModel()
{
    Scales = glm::vec3(1000.0f, 1000.0f, 1000.0f);
    setupCubeMap(FH);
}


SkyBox::~SkyBox() {
}
	

bool
SkyBox::Init() {
    SetModelMatrix();
    glBindVertexArray(vertexArrayID);
    positionLoc = glGetAttribLocation(programObject, "vertex");
    PVM       = glGetUniformLocation(programObject, "PVM");
    AmbID     = glGetUniformLocation(programObject, "ambient");
    if((positionLoc == -1) ||
       (AmbID     == -1) ||
       (PVM       == -1))
    {
        printf("Unable to get Shader Variables locations\n");
        exit(EXIT_FAILURE);
    }
    glBindVertexArray(0);

    if (glGetError() != GL_NO_ERROR) {
        printf("setupCubeMap() Error !\n");
        exit(EXIT_FAILURE);
	}

    return true;
}


bool
SkyBox::Update() {
// Set up model view
    DegreeRotations.x += TheGame->DeltaTimePassed*0.1f;
    Rotations.x = DEG2RAD(DegreeRotations.x);
    DegreeRotations.y -= TheGame->DeltaTimePassed*0.1f;
    Rotations.y = DEG2RAD(DegreeRotations.y);
    DegreeRotations.z += TheGame->DeltaTimePassed*0.1f;
    Rotations.z = DEG2RAD(DegreeRotations.z);

    MakeRotationMatrix();
    MakeTranslationMatrix();
    MakeModelMatrix();

    // get these from the camera
    glm::mat4* View       = TheGame->GetCamera()->GetView();
    glm::mat4* Projection = TheGame->GetCamera()->GetProjection();
    // make the MVP
    glm::mat4 MVP   = *Projection * *View * Model; // Remember order seems backwards

    glUseProgram(programObject);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(PVM, 1, GL_FALSE, &MVP[0][0]);

    float Amb[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glUniform4fv(AmbID, 1, &Amb[0]);

    glUseProgram(0);

    return true;
}


bool
SkyBox::Draw() {
    // Use the program object
    glUseProgram(programObject);
    glDepthFunc(GL_LEQUAL);

    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(GLuint(positionLoc),
                          3,// 3 values
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GLfloat)*5,// stride
                          static_cast<void*>(nullptr)
    );
    glEnableVertexAttribArray(GLuint(positionLoc));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDepthFunc(GL_LESS);
    return true;
}


void
SkyBox::loadCubemap(std::vector<const GLchar*> faces, MyFiles* FH) {
    int width=0, height=0;
    char* image;

    // texture1 is defined in ObjectModel !
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture1);
    for(GLuint i=0; i<faces.size(); i++) {
        image = FH->Load(faces[i], &width, &height);
        if(width*height == 0) {
            printf("Cube texture %s could not be loaded\n", faces[i]);
            exit(EXIT_FAILURE);
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGBA,
                     width,
                     height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     image
        );
        delete image;
    }// for(GLuint i=0; i<faces.size(); i++)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


void
SkyBox::setupCubeMap(MyFiles* FH) {
    vector<const char*> faces;
    faces.push_back("Resources/Textures/vr_rt.tga");
    faces.push_back("Resources/Textures/vr_lf.tga");
    faces.push_back("Resources/Textures/vr_up.tga");
    faces.push_back("Resources/Textures/vr_dn.tga");
    faces.push_back("Resources/Textures/vr_bk.tga");
    faces.push_back("Resources/Textures/vr_ft.tga");

    loadCubemap(faces, FH);
}


void
deleteCubeMap(GLuint& texture) {
    glDeleteTextures(1, &texture);
}

