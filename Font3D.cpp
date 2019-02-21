#include "Font3D.h"
#include <assert.h>
// Font 3D does not load shaders, let the code that uses it create the shaders
// quite a simple font system, and only prints 1 colour
// (could change colour from a uniform to an attrib)


Font3D::Font3D() {
// Don't really need to do much here the init should
// handle it but do ensure the buffer is nulled
// that way if the buffer is not actually created,
// it won't accidently remove random memory
    glGenVertexArrays(1, &vertexArrayID);
    VertexTextBuffer = nullptr;
// Set a default size for the font, it can be altered;
    SetFontSize(16);
	SetScale(1.0f);
	CurrentColour = glm::vec4(1.0); // white as default
}


Font3D::~Font3D() {
// There should be a buffer... if there is remove it
// and any textures can be freed as the buffer
// won't get created if the init didn't happen
    if(VertexTextBuffer != nullptr) {
		free(this->VertexTextBuffer);
	}
    glDeleteVertexArrays(1, &vertexArrayID);
}


// Load a font, convert to texture, create a buffer...easy, PO must be set up 1st
void
Font3D::Init(int Width, int Height, const char* FontFile) {
    ScreenHeight = Height;// This has to be the FB resolution, not the screen res.
    ScreenWidth = Width;  // Our text will not draw outside these
// load the font image, if it fails stop
	MyFiles FH;	
    int Theight, Twidth;
    TexBuffer = nullptr;
	TexBuffer = FH.Load(FontFile, &Twidth, &Theight);
	assert(TexBuffer != NULL); // we will stop if we failed to load
	U_Size = 1.0f / (Twidth / Font_Size);
	V_Size = 1.0f / (Theight / Font_Size);

    glBindVertexArray(vertexArrayID);
    glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 Twidth,
                 Theight,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 TexBuffer);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    free(TexBuffer); // it's in GPU MEM now so kill the data
// Ok it loaded	let our handle know
	glUseProgram(PO);
	glGenBuffers(1, &this->TexBuffHandle);

// We dont' load any shaders in here but we should be comfortable
// that the shader has been made so we can set up values
    HalfSizeID = GLuint(glGetUniformLocation(PO, "u_HalfSize"));
    SamplerLoc = GLuint(glGetUniformLocation(PO, "u_Sampler"));
    ColourLoc = glGetUniformLocation(PO, "u_Colour");
// The uniforms are given const values
	
    glUniform1i(GLint(SamplerLoc), 0); // use texture 0
    glUniform2f(GLint(HalfSizeID), ScreenWidth/2, ScreenHeight/2);
	glUniform4fv(ColourLoc, 1,&this->CurrentColour[0]);
	
// Get the attrib
    PosLoc   = GLuint(glGetAttribLocation(PO, "a_Position"));
    texCoord = GLuint(glGetAttribLocation(PO, "a_texCoord"));
	
// Now create a buffer
// This is where we create our draw array, destructor must free it.
    VertexTextBuffer = reinterpret_cast<VertexInfo*>
            (malloc(sizeof(VertexInfo)*(MAX_TEXT*VERTS_PER_CHAR)));
    CharsToWrite = 0;
	glUseProgram(0); // release the PO
    glBindVertexArray(0);
}


// We print text into our buffer but do not render it until a game cycle is complete
void
Font3D::PrintText(const char* text, int x, int y) {
// Shaders are not so good with ints doing clipspace calcs
// so we need to send them floats
	float fx = x ;
	float fy = y ;

	VertexInfo* tBuffer = &VertexTextBuffer[CharsToWrite*VERTS_PER_CHAR];  

// Parse the text string until 0 is found, or max chars is reached
    for(; *text != 0 && CharsToWrite < MAX_TEXT;
        CharsToWrite++, text++, fx += Font_Size*Scale)
    {
// We discount anything below the space (ascii 32)
        char character = *text - 32;
		float u = (character % Font_Size)*(U_Size);
		float v = (character / Font_Size)*(V_Size);
// We store 6 vertex values, x and y, but also the uv coords for the tile
        *tBuffer++ = VertexInfo { fx, fy, u, v };
        *tBuffer++ = VertexInfo { fx, fy + Font_Size*Scale, u, v + V_Size };
        *tBuffer++ = VertexInfo { fx + Font_Size*Scale, fy, u + U_Size, v };
        *tBuffer++ = VertexInfo { fx + Font_Size*Scale, fy + Font_Size*Scale, u + U_Size, v + V_Size };
        *tBuffer++ = VertexInfo { fx + Font_Size*Scale, fy, u + U_Size, v };
        *tBuffer++ = VertexInfo { fx, fy + Font_Size*Scale, u, v + V_Size };
	}
}

	
void
Font3D::RenderText() {
    glBindVertexArray(vertexArrayID);
// We use this shader not defined in this class
// so we can allow the game to choose.
    glUseProgram(PO);
// Bind that texture buffer
    glBindBuffer(GL_ARRAY_BUFFER, TexBuffHandle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
// Now set up the data buffer with our vertices
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(VertexInfo)*VERTS_PER_CHAR*uint(CharsToWrite),
                 VertexTextBuffer,
                 GL_STATIC_DRAW);
// Prep the attribute writes
    glVertexAttribPointer(PosLoc,
                          2,       // 2 values x and y
                          GL_FLOAT,// these are ints cast to floats
                          false,
                          sizeof(VertexInfo),
                          nullptr);
    glVertexAttribPointer(this->texCoord,
                          2, // 2 values u v
                          GL_FLOAT,
                          false,
                          sizeof(VertexInfo),
                          reinterpret_cast<const void*>(sizeof(float)*2));
// Enable them
	glEnableVertexAttribArray(PosLoc);
	glEnableVertexAttribArray(texCoord);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// Get busy with it!
    glDrawArrays(GL_TRIANGLES, 0, CharsToWrite*VERTS_PER_CHAR);
// Clean up
	CharsToWrite = 0;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}
