#include "Graphics.h"
#include "ObjectModel.h"
#include "ShaderManager.h"

#include <stdio.h>


Graphics::Graphics()
    : OurShaderManager(nullptr)
{
    OurShaderManager = new ShaderManager();
    init_ogl();
}


Graphics::~Graphics() {
    if(OurShaderManager)
        delete OurShaderManager;
}


// Set up the OpenGL
void
Graphics::init_ogl() {
    if(!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        getchar();
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const GLFWvidmode* pMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    state.width  = pMode->width;
    state.height = pMode->height;
    state.nativewindow = glfwCreateWindow(state.width,
                                          state.height,
                                          "Tutorial",
                                          nullptr,
                                          nullptr
                         );
    if(state.nativewindow == nullptr) {
        printf("Failed to open GLFW window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(state.nativewindow);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // glewInit() will generate a GL_INVALID_ENUM error if used with a core profile context.
    // It calls glGetString(GL_EXTENSIONS) to get the extension list, but this will generate
    // GL_INVALID_ENUM with the core profile, where glGetStringi() must be used instead.
    while(true) {
        if(glGetError() == GL_NO_ERROR)
            break;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(state.nativewindow, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //>>glfwSetInputMode(state.nativewindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(state.nativewindow, state.width/2, state.height);

    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Setup the viewport
    glViewport(0, 0, state.width, state.height);
}


// Used when creating dynamic shadows: it
// create and return a frame buffer object
GLuint
Graphics::CreateShadowBuffer(int width, int height) {
    GLuint	FBO, RenderBuffer;
    glActiveTexture(GL_TEXTURE1);  // Use an unused texture
// Make a texture the size of the screen
    glGenTextures(1, &ShadowTexture);
    glBindTexture(GL_TEXTURE_2D, ShadowTexture); // it is bound to texture 1

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 nullptr);// was RGBA, probably does not need the A might save time/space
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// Once the texture is set up, we can turn to the Framebuffer
    glGenFramebuffers(1, &FBO); // Allocate a FrameBuffer
    glGenRenderbuffers(1, &RenderBuffer); // Allocate a render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer); // And bind it
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH_COMPONENT16,
                          width,
                          height); // Give some parameters for it
    glBindTexture(GL_TEXTURE_2D, ShadowTexture);// Bind the texture
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);     // and the frame buffer

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                              GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER,
                              RenderBuffer); // They are now attached
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           ShadowTexture,
                           0); // and so is the texture that will be created

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("oh bugger framebuffer not properly set up");
        exit(EXIT_FAILURE);
    }
    glActiveTexture(GL_TEXTURE0);// Careful to not damage this
    return FBO;
}



