
#pragma once
#undef countof
#include "glm/glm.hpp"
#undef countof
#include <vector>
#include <map>
#include "MyFiles.h"
#include <GL/glew.h>


////////////////////// *** TYPEDEFS *** //////////////////////
typedef unsigned char byte;
// This stores a skin name
typedef char tMd2Skin[64];

////////////////////// *** DEFINES *** //////////////////////
// These are the needed defines for the max values when loading .MD2 files
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

// This stores the speed of the animation between each key frame
#define kAnimationSpeed			5.0f

// Header structure for MD2 files
typedef struct {
    int ident;          /* magic number: "IDP2" */
    int version;        /* version: must be 8 */
    int skinwidth;      /* texture width */
    int skinheight;     /* texture height */
    int framesize;      /* size in bytes of a frame */
    int numSkins;       /* number of skins */
    int numVertices;    /* number of vertices per frame */
    int numUVs;         /* number of texture coordinates */
    int numTris;        /* number of triangles */
    int numGlcmds;      /* number of opengl commands */
    int numFrames;      /* number of frames */
    int offsetSkins;    /* offset skin data */
    int offsetUVs;      /* offset texture coordinate data */
    int offsetTris;     /* offset triangle data */
    int offsetFrames;   /* offset frame data */
    int offsetGlcmds;   /* offset OpenGL command data */
    int offsetEnd;
} MD2Header;


typedef struct {
    unsigned short vertex[3];
    unsigned short uv[3];
} MD2Triangle;


typedef struct {
    GLuint vbo;  // vertex buffer
    int numTriangles;
    std::vector<float> vertexBuffer;
    size_t material_id;
} MD2Mesh;


// Struct that defines an animation.
typedef struct {
    int startFrame;
    int numFrames;
    string name;
} MD2Animation;


// A containing structure for all things related to MD2's,
// it is created in the ObjectModel class when an MD2 is loaded
// to provide the MD2 handling with info on the meshes and animation.
// Be aware: if you use it, your MD2 destructor or Model Manager
// needs to take care of its disposal.
typedef struct {
    std::vector<MD2Mesh> MD2meshes;
    std::vector<MD2Animation> animations; // Vector of animations if applicable.
    std::map<std::string, MD2Animation> NamedAnimation;
} MD2DataContainer;
