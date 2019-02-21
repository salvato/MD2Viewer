#pragma once

#include <stdlib.h>
#include <stdio.h>

#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" // only using glm for vec4 colour value can use any lib with vec4
#undef countof
#include "MyFiles.h" // we will need to load graphics;
#include <GL/glew.h>

// we are using buffers so need to set a limit on the number of chars we can set up before
// we do a render and flush the buffer, this will decide the size of that buffer (maxnumber * size of info buffer)
#define MAX_TEXT 1000
// making a tile so its basically 6 verts (could use an index version???)
#define VERTS_PER_CHAR 6


class Font3D
{
public:
	Font3D();
	~Font3D();
	
public:
    void Init(int FBWidth, int FBHeight, const char *FontFile); // set it up
	void PrintText(const char* text, int x, int y); // write to buffer
	void RenderText();
    inline void SetFontSize(int s){ Font_Size = s; } // the square size of the tiles (num pixels)
    inline void SetScale(float s){ Scale = s; }
    inline void SetColour(glm::vec4 c) 	{
		CurrentColour = c;
		glUseProgram(PO);
		ColourLoc = glGetUniformLocation(PO, "u_Colour");
		glUniform4fv(ColourLoc, 1, &this->CurrentColour[0]);
		glUseProgram(0);
	}
	
public:
    GLuint	PO;
	glm::vec4 CurrentColour;	
	
private:	
// Handles for the shader
	GLuint	PosLoc;
	GLuint	SamplerLoc;

	GLuint  texCoord;
    GLuint  HalfSizeID;// to let us use clip space as screen
    GLint	ColourLoc; // so we can change colour
// We need to create a buffer of data in this format
    struct VertexInfo {
		float x;	// screen position 
		float y;	//   "		"
		float u;   // uv tex coords
		float v;
	};
	
	VertexInfo*	VertexTextBuffer;

	int	Font_Size;
	float U_Size; // will depend on size of texture but is basically (1.0 / number of chars)
	float V_Size; // this provides a step value to get to the tile in the texture.
	int	CharsToWrite; // when we do our Render we need to know.
	int ScreenWidth;
	int	ScreenHeight; // limits of our draws
	char* TexBuffer; // we're not goint to actually create a texutre, but keep a buffer.
	GLuint TexBuffHandle;
	GLuint TextureID;
    GLuint vertexArrayID;
    float Scale;
};
