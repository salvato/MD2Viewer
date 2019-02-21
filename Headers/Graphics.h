#pragma once

#include <GL/glew.h>   // OpenGL Extension Wrangler Library (GLEW) is a cross-platform open-source
                       // C/C++ extension loading library. http://glew.sourceforge.net/

#include <GLFW/glfw3.h>// GLFW is a free, Open Source, multi-platform library for OpenGL,
                       // OpenGL ES and Vulkan application development. https://www.glfw.org
                       // It provides a simple, platform-independent API for creating windows,
                       // contexts and surfaces, reading input, handling events, etc.

#include <glm/glm.hpp> // OpenGL Mathematics (glm.g-truc.net)

class ObjectModel;
class ShaderManager;


class Graphics 
{
public:
	Graphics();
	~Graphics();
	
public:
    void init_ogl();
    GLuint CreateShadowBuffer(int width, int height);

public:
    ShaderManager* OurShaderManager;

    typedef struct Target_State {
        int32_t width;
        int32_t height;
        GLFWwindow* nativewindow;
    } Target_State;
    Target_State state;
	Target_State* p_state = &state;
		
    glm::vec3 LightPos;
    glm::vec3 LightTarget;
    GLuint	ShadowFB;	 // our framebuffer for shadows
    GLuint	ShadowShader;// our standard shadow shader
    GLuint	ShadowTexture;//
};
