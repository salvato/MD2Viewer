#pragma once

// Parts of this file, specifically the routines, LoadandConvert and CalcNormal as well as the data structures used by them
// have been converted and modifed from Soyo Fujita's viewer example code in TinyOBJLoader which was distributed under the MIT
// license. In accordance with that license I acknowledge the original copyright and ownership of those routines and reprint
// the copyright notice here:

// The MIT License (MIT)

// Copyright (c) 2012-2017 Syoyo Fujita and many contributors.

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.



#include	<map>
#include	<string>
#include	<vector>
#include <GL/glew.h>
#undef countof
#include	"glm/glm.hpp"
#include	"glm/gtc/matrix_transform.hpp"
#undef countof
#include	"ObjectModel.h"
#include	"MyFiles.h"
#include	"ShaderManager.h"
#include	"tiny_obj_loader.h"
#include	"TextureManager.h"
#include	"BinaryReader.h"
#include	"MD2.h"
#include    <bullet/btBulletDynamicsCommon.h>


class ModelManager
{
public:
    ModelManager();
    ~ModelManager();

public:
    static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
        float v10[3];
        v10[0] = v1[0] - v0[0];
        v10[1] = v1[1] - v0[1];
        v10[2] = v1[2] - v0[2];

        float v20[3];
        v20[0] = v2[0] - v0[0];
        v20[1] = v2[1] - v0[1];
        v20[2] = v2[2] - v0[2];

        N[0] = v20[1] * v10[2] - v20[2] * v10[1];
        N[1] = v20[2] * v10[0] - v20[0] * v10[2];
        N[2] = v20[0] * v10[1] - v20[1] * v10[0];

        float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
        if(len2 > 0.0f) {
            float len = sqrtf(len2);
            N[0] /= len;
            N[1] /= len;
        }
    }
    // simple calls to load different model types;
    bool LoadMD2(ObjectModel* model, const char* Filename, MyFiles* Handler);
    bool LoadOBJModel(ObjectModel* MyModel,
                      MyFiles* FH,
                      const char* filename,
                      std::vector<btVector3>* CollisionMesh = nullptr
            ); //note default null value
    // the workhorse loader	for obj
    bool LoadandConvert(std::vector<ObjectModel::DrawObject>* drawObjects,		/*passed from the object*/
                        std::vector<tinyobj::material_t>& materials, /* passed from and to the object*/
                        std::map<std::string,GLuint>& textures, /* Located here to keep track of textures*/
                        const char* filename,
                        MyFiles* FH,
                        std::vector<btVector3>* CollisionMesh = nullptr
            ); //only if a call supplies the mesh do we collect and store it.

    void GetBoundingBoxes(ObjectModel* OM);

    // Texture manager will keep hold of the textures
    TextureManager TexManager;
    // Manager will also keep a record of the Models's already loaded and their associated VBO, it won't care about CPU vertices
    std::map<std::string, ObjectModel::DrawObject> ExistingShapes;
    std::map<std::string, MD2DataContainer*> AllMD2Meshes;
    float bmin[3];
    float bmax[3]; // this will provide bounding boxes for 3 axis, they are not always needed, so the calling routine can decide it it wants to use them after a load complete

private:
}; // class



