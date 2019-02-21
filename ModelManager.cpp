// Model Manager
// Primary function is to load models and their mat/textures and store them in OAB's or in ObjModel data
// Most OBJ's will go to the OAB and use TinyObjLoader to do their conversion and storage.
// Contains the maps and texture lists to prevent double loads of texture files and OAB's

// MD2Models can optionally choose to use their global normal values, though they are repeated for each vector in a triangle
// ensure that MD2NORMS is defined as you want it in MD2Model.h



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


#include "ModelManager.h"
#include "MD2Model.h" // we need to know if MD2NORMS are wanted

#define PRINT_ANIM


#ifdef MD2NORMS
// if defined include the standard MD2 normal indexed by a byte value in the data.
// if you plan to do any lighitng on your MD2, you will need to include this, so it is defaulted to defined
// M2DModel.cpp sets up the correct attributes
static float MD2Norms[256][3] = {
#include "anorms.h"
};
#endif // MD2norms


ModelManager::ModelManager()  {
//constructor set ups here
};


ModelManager::~ModelManager() {
// make sure the vectors are closed and data systems shut down
}


bool
ModelManager::LoadOBJModel(ObjectModel* MyModel, MyFiles* FH, const char* filename, std::vector<btVector3>* Mesh) {
    return LoadandConvert(&MyModel->gdrawObjects,
                          MyModel->materials,
                          this->TexManager.Totaltextures,
                          filename,
                          FH,
                          Mesh
           );
}



//Load and convert an OBJ file to a vbo, store draw objects ObjModel base
//Maintains textures in TextureManager
bool
ModelManager::LoadandConvert(std::vector<ObjectModel::DrawObject>* drawObjects,
                             std::vector<tinyobj::material_t>& materials,
                             std::map<std::string,GLuint>& textures,
                             const char* filename,
                             MyFiles* FH,
                             std::vector<btVector3>* Mesh//=0
              )
{
// create a few local working variables
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::string err;
// Do the actual load
    bool ret = tinyobj::LoadObj(&attrib,
                                &shapes,
                                &materials,
                                &err, filename,
                                "Resources/Material/"
                        );
// check for errors
    if (!err.empty()) {
        char *cstr = &err[0u];// printf needs chars
        printf("oh Bugger can't load this %s, file returns error %s \n", filename, cstr);
        return false; // we failed no sense in continuing.
    }
//it may have loaded but not parsed correctly
    if (!ret) {
        printf("The %s.obj seems to be corrupt or incorrectly formated\n", filename);
        return false;
    }
// output the use data for reference
    printf("Attrib data for %s is :-\n", filename);
    printf("# vertices  : %d\n", int(attrib.vertices.size()) / 3);
    printf("# normals   : %d\n", int(attrib.normals.size()) / 3);
    printf("# texcoords : %d\n", int(attrib.texcoords.size()) / 2);
    printf("Other data  :-%s\n", " - - -");
    printf("# materials : %d\n", int(materials.size()));
    for (uint i=0; i<shapes.size(); i++)  {
        printf("# Indices in shapes[%d]  : %d\n",
               i,
               int(shapes[i].mesh.indices.size())
        );
    }
 // Append `default` material so there is something at 0
    materials.push_back(tinyobj::material_t());
    const char* base_dir = "Resources/Textures/";
    // Parse the materials and Load and create textures
    {
        for(size_t m=0; m<materials.size(); m++) {
            tinyobj::material_t* mp = &materials[m];
            if(mp->diffuse_texname.length() > 0) {
                char *cstr10 = &mp->diffuse_texname[0u]; // embarrasing side effect of the decision to use char.....
                printf("# Texture name    : %s \n", cstr10);
                printf("# Size of Map     : %lud \n", textures.size());
                // Only load the texture if it is not already loaded
                if(textures.find(mp->diffuse_texname) == textures.end()) {
                    GLuint texture_id;
                    int w=0, h=0;
                    std::string texture_filename = mp->diffuse_texname;
                    texture_filename = base_dir + mp->diffuse_texname;
                    char *cstr = &texture_filename[0u]; // embarrasing side effect of the decision to use char.....
                    char* image = FH->Load(cstr, &w, &h);
                    if (w*h == 0) {
                        printf("Unable to load texture:%s \n", cstr);
                        exit(EXIT_FAILURE);
                    }
                    printf("Texture info for %s :-\n", cstr);
                    printf("# width   : %d\n", w);
                    printf("# Height  : %d\n", h);
                    printf("# comp    : %d\n", FH->comp);
                    glGenTextures(1, &texture_id);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    if (FH->comp == 4) {	// check if we had an alpha texture or a plane (strictly speaking any value not 3 is wrong)
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
                    }
                    else {
                        if (FH->comp != 3) printf("Made a default non alpha texture, comp was :%d\n", FH->comp);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                    }
                    if(glGetError() != GL_NO_ERROR) {
                        printf("Oh bugger:- Model texture creation error, but attempting to carry on.\n");
                    }
                    glGenerateMipmap(GL_TEXTURE_2D); // uncomment if you want to use mipmapping....
                                                     // of course you do in any game with depth.
                    glBindTexture(GL_TEXTURE_2D, 0);
                    free(image); // release the cpu memory once its been put in the GPU
                    textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                }
                else
                    printf("Texture %s already present not reloaded \n", cstr10);
            }
        }
    }
// Note this version produces ONLY vertex arrays, and does so by using indices
// to find the vertices.
// A considerable rendering improvment can be had by creating
// VBs of the unique vertices and indices...hint
//		 int howmanyShapes = shapes.size();
// fix/update the min and max, held as local class variables,
// for possible download, not all obj's need them so no need to auto load them
    bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::min();
    bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();
    {
        for(size_t s=0; s<shapes.size(); s++) {
            ObjectModel::DrawObject o;
            std::vector<float> vb;  // pos(3float), normal(3float), color(3float)
            for(size_t f=0; f<shapes[s].mesh.indices.size()/3; f++) {
                tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];
                int current_material_id = shapes[s].mesh.material_ids[f];
// some error checking
                if((current_material_id < 0) ||
                   (current_material_id >= static_cast<int>(materials.size())))
                {
                  // Invaid material ID. Use default material.
                    current_material_id = int(materials.size() - 1); // Default material is added to the last item in `materials`.
                }
                if(current_material_id >= int(materials.size())) {
                    std::cerr << "Invalid material index: " << current_material_id << std::endl;
                }
                //
                float diffuse[3];
                for (size_t i=0; i<3; i++) {
                    diffuse[i] = materials[ulong(current_material_id)].diffuse[i];
                }
                float tc[3][2];
                if (attrib.texcoords.size() > 0) {
                    assert(attrib.texcoords.size() > ulong(2 * idx0.texcoord_index + 1));
                    assert(attrib.texcoords.size() > ulong(2 * idx1.texcoord_index + 1));
                    assert(attrib.texcoords.size() > ulong(2 * idx2.texcoord_index + 1));
// The division by materials[current_material_id].diffuse_texopt.scale[]
// solves the problem of textures being stretched on the whole surface.
// If we want to repeat a texture we need to give the uv coordinates
// a value > 1. In example a value of 2 means the texture is going to be repeated twice.
// The scale value ranges from 0.0 to 1.0 so dividing by a number < 0
// we're actually increasing the uv values and our textures will be repeated accordingly.
// - Thanks to Emilio Laiso for this useful fix
                    tc[0][0] = attrib.texcoords[ulong(2 * idx0.texcoord_index)] /
                               materials[ulong(current_material_id)].diffuse_texopt.scale[0];
                    tc[0][1] = 1.0f - attrib.texcoords[ulong(2 * idx0.texcoord_index + 1)] /
                               materials[ulong(current_material_id)].diffuse_texopt.scale[1];
                    tc[1][0] = attrib.texcoords[ulong(2 * idx1.texcoord_index)] /
                               materials[ulong(current_material_id)].diffuse_texopt.scale[0];
                    tc[1][1] = 1.0f - attrib.texcoords[ulong(2 * idx1.texcoord_index + 1)] /
                               materials[ulong(current_material_id)].diffuse_texopt.scale[1];
                    tc[2][0] = attrib.texcoords[ulong(2 * idx2.texcoord_index)] /
                               materials[ulong(current_material_id)].diffuse_texopt.scale[0];
                    tc[2][1] = 1.0f - attrib.texcoords[ulong(2 *idx2.texcoord_index + 1)] /
                               materials[ulong(current_material_id)].diffuse_texopt.scale[1];
                }
                else {
                    tc[0][0] = 0.0f;
                    tc[0][1] = 0.0f;
                    tc[1][0] = 0.0f;
                    tc[1][1] = 0.0f;
                    tc[2][0] = 0.0f;
                    tc[2][1] = 0.0f;
                }

                float v[3][3];
                for(ulong k=0; k<3; k++) {
                    ulong f0 = ulong(idx0.vertex_index);
                    ulong f1 = ulong(idx1.vertex_index);
                    ulong f2 = ulong(idx2.vertex_index);
                    v[0][k] = attrib.vertices[3 * f0 + k]; // the triangle is here.... x
                    v[1][k] = attrib.vertices[3 * f1 + k];//y
                    v[2][k] = attrib.vertices[3 * f2 + k];//z
                    bmin[k] = std::min(v[0][k], bmin[k]);
                    bmin[k] = std::min(v[1][k], bmin[k]);
                    bmin[k] = std::min(v[2][k], bmin[k]);
                    bmax[k] = std::max(v[0][k], bmax[k]);
                    bmax[k] = std::max(v[1][k], bmax[k]);
                    bmax[k] = std::max(v[2][k], bmax[k]);
                }
                float n[3][3];
                if(attrib.normals.size() > 0) {
                    int f0 = idx0.normal_index;
                    int f1 = idx1.normal_index;
                    int f2 = idx2.normal_index;
                    assert(f0 >= 0);
                    assert(f1 >= 0);
                    assert(f2 >= 0);
                    for(int k=0; k<3; k++) {
                        n[0][k] = attrib.normals[ulong(3 * f0 + k)];
                        n[1][k] = attrib.normals[ulong(3 * f1 + k)];
                        n[2][k] = attrib.normals[ulong(3 * f2 + k)];
                    }
                }
                else {
                  // compute geometric normal
                    CalcNormal(n[0], v[0], v[1], v[2]);
                    n[1][0] = n[0][0];
                    n[1][1] = n[0][1];
                    n[1][2] = n[0][2];
                    n[2][0] = n[0][0];
                    n[2][1] = n[0][1];
                    n[2][2] = n[0][2];
                }
                for(int k=0; k<3; k++) {
                    vb.push_back(v[k][0]);
                    vb.push_back(v[k][1]);
                    vb.push_back(v[k][2]);
#ifdef BULLET // only relevent if we have Bullet in place
                    if(Mesh != nullptr) {// if we are storint to a mesh, lets collect the vertex and send to the supplied mesh vector.
                        btVector3 vert = btVector3( v[k][0], v[k][1], v[k][2]);
                        Mesh->push_back(vert);
                    }
#endif // BULLET
                // uncomment if you want to see the data in console out
                //	printf("v1 v2 v3 =%f  %f  %f\n", v[k][0], v[k][1], v[k][2]);
                    vb.push_back(n[k][0]);
                    vb.push_back(n[k][1]);
                    vb.push_back(n[k][2]);
                    // Combine normal and diffuse to get color.
                    float normal_factor = 0.2f;
                    float diffuse_factor = 1 - normal_factor;
                    float c[3] = {
                        n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
                        n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
                        n[k][2] * normal_factor + diffuse[2] * diffuse_factor
                    };
                    float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                    if(len2 > 0.0f) {
                        float len = sqrtf(len2);
                        c[0] /= len;
                        c[1] /= len;
                        c[2] /= len;
                    }
                    vb.push_back(c[0] * 0.5f + 0.5f);
                    vb.push_back(c[1] * 0.5f + 0.5f);
                    vb.push_back(c[2] * 0.5f + 0.5f);
                    vb.push_back(tc[k][0]);
                    vb.push_back(tc[k][1]);
                }
            }
            o.vb = 0;
            o.numTriangles = 0;
            // OpenGL viewer does not support texturing with per-face material.
            if(shapes[s].mesh.material_ids.size() > 0 &&
               shapes[s].mesh.material_ids.size() > s)
            {// Base case
                o.material_id = ulong(shapes[s].mesh.material_ids[s]);
            }
            else {
                o.material_id = materials.size() - 1; // = ID for default material.
            }
// check here if we already have a vbo for this mesh in which case
// copy the shape details and do not create another
            if(ExistingShapes.find(shapes[s].name) == ExistingShapes.end()) {
                if(vb.size() > 0) {
                    glGenBuffers(1, &o.vb);
                    glBindBuffer(GL_ARRAY_BUFFER, o.vb);
                    glBufferData(GL_ARRAY_BUFFER,
                                 long(vb.size()*sizeof(float)),
                                 &vb.at(0),
                                 GL_STATIC_DRAW
                    );
                    //not really the number of triangles but the number of components parts.
                    o.numTriangles = int(vb.size() / (3 + 3 + 3 + 2));
                    printf("shape[%d] # of triangles = %d\n",
                           static_cast<int>(s),
                           o.numTriangles/3);
                    GLsizei size;
                    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
                    printf("VB is reported as %d \n", size);
                    ExistingShapes.insert(std::make_pair(shapes[s].name, o));
                    printf("Stored values are %zu, %d, vbID=  %d,  size of map is %lu \n",
                           o.material_id, o.numTriangles, o.vb, ExistingShapes.size());
                    drawObjects->push_back(o);
                }
            }
            else {
                o.material_id = ExistingShapes[shapes[s].name].material_id;
                o.numTriangles = ExistingShapes[shapes[s].name].numTriangles;
                o.vb = ExistingShapes[shapes[s].name].vb;
                printf("duplicated values are %zu, %d, %d  size of map is %lu \n",
                       o.material_id,
                       o.numTriangles,
                       o.vb,
                       ExistingShapes.size()
                );
                drawObjects->push_back(o);
            }
        }
    }
    printf("bmin = %f, %f, %f\n", double(bmin[0]), double(bmin[1]), double(bmin[2]));
    printf("bmax = %f, %f, %f\n", double(bmax[0]), double(bmax[1]), double(bmax[2]));
    glBindTexture(GL_TEXTURE_2D,0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}


// give the object the bmin and bmax values, they are not always used so this is an optional load
// for a model
void
ModelManager::GetBoundingBoxes(ObjectModel* OM) {
    for (int i=0; i<3; i++) {
        OM->bmin[i] = bmin[i];
        OM->bmax[i] = bmax[i];
    }
}


/**************************************************************************************
The MD2 loader works on the same basic principles of the OBJ loader
keeps track of its textures and uses the same basic materails as the OBJ
also has animation data though and uses binary loads.Some MD2's are supplied with no skin
it is up to the instancing to provide a nice texture.
****************************************************************************************/
bool
ModelManager::LoadMD2(ObjectModel* model, const char* Filename, MyFiles* FH) {
    printf("Attempting to load: %s\n", Filename);
// check if we already loaded this, if so, use it, and 	don't load again
    if(!(AllMD2Meshes.find(Filename) == AllMD2Meshes.end())) {// ok we have this one
        printf("But we alreay got this one: %s\n", Filename);
        model->TheMD2Data = AllMD2Meshes[Filename];
        return true;
    }
    MD2DataContainer* WhereIsMesh = new MD2DataContainer;
    BinaryReader reader;
    const char* base_dir = "Resources/Models/";
    if(!reader.Load(Filename)) {
        std::cerr << "Unable to load MD2: \"" << Filename << "\".\n";
        exit(EXIT_FAILURE);
    }
    MD2Header *header; // Read the header into our struct.
    header = reinterpret_cast<MD2Header*>(reader.ReadAny(sizeof(MD2Header)));
    if(header->ident != 844121161 || header->version != 8) {
        printf("Oh Bugger-MD2 Magic Number is not as expected in %s: \n",Filename);
        exit(EXIT_FAILURE);
    }
    reader.Seek(header->offsetSkins);
    base_dir = "Resources/Textures/";
    if(header->numSkins == 0) {
        printf("***!!!! Take note, this model has no defined skins\n    You should load your own texture!\n");
    }
    for(int i=0; i<header->numSkins; i++) {
        tinyobj::material_t material;

        char tmp[64];
// load in the 64 bytes of the texture file here
        for(int i=0; i<64; i++) {
            tmp[i] = char(reader.ReadByte());
        }

// cast it to a string
        material.diffuse_texname = static_cast<string>(tmp);

// older files often use pcx, but stbi can't load them so check if that is the case.
        string testFor  = ".pcx";
        string replace  = ".tga"; // they are usually replaced with tga but alter as you see fit?
        string replace2 = ".png"; // you can provide your own preffered back up conversion for PCX

        size_t pos = material.diffuse_texname.find(testFor);
        if(pos) {
            printf("WARNING!!! MD2 wants to load an unsupported PCX file,\n");
            printf("%s attempting to load %s version instead\n ", tmp, &replace[0u]);
            material.diffuse_texname.replace(pos, pos+4, replace);
        }
        // create and store a texture......
        // Only load the texture if it is not already loaded
        if(TexManager.Totaltextures.find(material.diffuse_texname) ==
           TexManager.Totaltextures.end())
        { // we gotta make a new texture
            GLuint texture_id;
            int w=0, h=0;
            std::string texture_filename = base_dir + material.diffuse_texname;
            char *cstr = &texture_filename[0u]; // embarrasing side effect of the decision to use char.....
            char* image = FH->Load(cstr, &w, &h);
            if(w*h == 0) {
                if(!pos) { // this was an as provided non PCX texture so either its not there or its
                    printf("Unable to load this texture %s it may not be present or supported\n", cstr);
                    exit(EXIT_FAILURE);
                }
// ok so we replace
                printf("Unable to load texture:%s will try a last attempt at a %s \n", cstr, &replace2[0u]);
// have one more go at a png
                material.diffuse_texname.replace(pos, pos + 4, replace2);
                std::string texture_filename = base_dir + material.diffuse_texname;
                cstr = &texture_filename[0u];
                image = FH->Load(cstr, &w, &h);
                if(w*h == 0) {
                    printf("Sorry, I tried TNG and PNG and failed!! I'll give up now if thats ok! \n");
                    exit(EXIT_FAILURE);
                }
            }
            if(pos) printf("Substitute texture type %s loaded ok\n", cstr);
            printf("Texture info for %s :-\n", cstr);
            printf("# width   : %d\n", w);
            printf("# Height  : %d\n", h);
            printf("# comp    : %d\n", FH->comp);
            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            if(FH->comp == 4) {// check if we had an alpha texture or a plane (strictly speaking any value not 3 is wrong)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            else {
                if (FH->comp != 3) printf("Made a default non alpha texture, comp was :%d\n", FH->comp);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
            if (glGetError() != GL_NO_ERROR) {
                printf("Oh bugger:- Model texture creation error, but attempting to carry on.\n");
            }
            glBindTexture(GL_TEXTURE_2D, 0);
            free(image); // release the cpu memory once its been put in the GPU
// we may have loaded a tga or a png, but if we wanted pcx,
// we want to prevent mutiple loads so whatever value
// we ended up loading we should use the orignal md2 name in our map
// so that the orginal test for validity will work, and we don't accidentaly
// reload over and over again.
// what ever the format we loaded, we will use the name it was called
// in the MD2 to reference it.
            material.diffuse_texname = static_cast<string>(tmp);
            this->TexManager.Totaltextures.insert(std::make_pair(material.diffuse_texname,
                                                                 texture_id));
            model->materials.push_back(material);
        }
        else
            printf("MD2 Texture already present \n");
    }
    glm::vec2* uvs = new glm::vec2[ulong(header->numUVs)];
    reader.Seek(header->offsetUVs);
    for (int i=0; i<header->numUVs; i++) {
        uvs[i].x = float(reader.ReadShort()) / float(header->skinwidth);
        uvs[i].y = float(reader.ReadShort()) / float(header->skinheight);
    }
// create index buffers
    reader.Seek(header->offsetTris);
    MD2Triangle* tris = new MD2Triangle[ulong(header->numTris)];
    for(int i=0; i<header->numTris; i++) {
        tris[i] = *reinterpret_cast<MD2Triangle*>(reader.ReadAny(sizeof(MD2Triangle)));
    }
    reader.Seek(header->offsetFrames);
// we're pointing at the frame
// iterate through the frames
    std::string lastFrameName = "";
    int frameNumber = 0;
    (void)frameNumber;
// fix / update the min and max, held as local class variables, for possible download,
// not all obj's need them so no need to auto load them
    bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::min();
    bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();
    for(int i=0; i<header->numFrames; i++) {
// get the scale, translate, name and verts for this frame
        glm::vec3 scale = reader.ReadVec3(); // get the scale
        glm::vec3 translate = reader.ReadVec3();  // and the translate
        char name[16]; // then the name 16butes
        for(int j=0; j<16; j++)
            name[j] = char(reader.ReadByte());
        std::string frameName = name;
// Go through and extract the frame numbers and erase them from the name
        for(unsigned int j=0; j<frameName.length(); j++) {
            // If the current index is a number and it's one
            // of the last 2 characters of the name
            if(isdigit(frameName[j]) && j >= frameName.length() - 2) {
                // Use a C function to convert the character to an integer.
                // Notice we use the address to pass in the current character and on
                frameNumber = atoi(&frameName[j]);
                // Erase the frame number from the name so we extract the animation name
                frameName.erase(j, 16 - j);
                break;
            }
        }
        if(frameName != lastFrameName) {
#ifdef PRINT_ANIM
            printf("%s Animation name: %s\n", Filename, frameName.c_str());
#endif
// we changed frames, set up a new one
            MD2Animation anim;
            anim.startFrame = i;
            anim.numFrames = 1;
            anim.name = frameName;
            WhereIsMesh->animations.push_back(anim);
        }
        else
            WhereIsMesh->animations[WhereIsMesh->animations.size() - 1].numFrames++;
        // make an array for our vertices
        glm::vec3* vertices = new glm::vec3[ulong(header->numVertices)];
        int	 NormalIndex = 0;
        (void)NormalIndex;
// fill up our vertex list and do the translate on them
        for(int j=0; j<header->numVertices; j++) {
            vertices[j] = reader.ReadCompressedVec3();
            NormalIndex = reader.ReadByte(); // normal index can be discarded or used
        }
// make a mesh
        MD2Mesh mesh;
// initialise it, we're running through a biggish loop, declare them outside of the loop to gain a bit of speed (unless optimisatoon does this for you)
        mesh.vbo = 0;
        mesh.numTriangles = 0;
        mesh.material_id = 0;
        mesh.vertexBuffer.clear(); // never hurts to be careful
        int index;
        glm::vec3 pos;
        glm::vec2 uv;

// run through the triangles and make a vertex list
        for(int tr=0; tr<header->numTris; tr++) {
            for(int p=0; p<3; p++) {
                index = tris[tr].vertex[p]; // got to the triangle, and the index for each of the 3 points
                pos = (vertices[index]*scale)+translate;
                if(i < 2) {
                    bmin[0] = std::min(pos.x, bmin[0]);
                    bmin[1] = std::min(pos.y, bmin[1]);
                    bmin[2] = std::min(pos.z, bmin[2]);
                    bmax[0] = std::max(pos.x, bmax[0]);
                    bmax[1] = std::max(pos.y, bmax[1]);
                    bmax[2] = std::max(pos.z, bmax[2]);
                }
                mesh.vertexBuffer.push_back(pos.x);
                mesh.vertexBuffer.push_back(pos.z);
                mesh.vertexBuffer.push_back(pos.y);
                index = tris[tr].uv[p];
                uv = uvs[index];
                mesh.vertexBuffer.push_back(uv.x);
                mesh.vertexBuffer.push_back(uv.y);

#ifdef MD2NORMS
                // we need to send it 3 times :(
                mesh.vertexBuffer.push_back(MD2Norms[NormalIndex][0]);
                mesh.vertexBuffer.push_back(MD2Norms[NormalIndex][1]);
                mesh.vertexBuffer.push_back(MD2Norms[NormalIndex][2]);
#endif // MD@NORMS
            }//for(int p=0; p<3; p++)
        }//for(int tr=0; tr<header->numTris; tr++)

        // Done with vertices delete it !
        delete[] vertices;
        if(mesh.vertexBuffer.size() > 0) {
    #ifdef MD2NORMS
            mesh.numTriangles = int(mesh.vertexBuffer.size() / ((3+3+2)*3));
    #else
            mesh.numTriangles = int(mesh.vertexBuffer.size() / ((3+2)*3));
    #endif // MD2NORMS
            glGenBuffers(1, &mesh.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         long(mesh.vertexBuffer.size()*sizeof(float)),
                         &mesh.vertexBuffer[0],
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            WhereIsMesh->MD2meshes.push_back(mesh); // the the model know all about the frame info
        }
        lastFrameName = frameName;
    }
// set up the name map so we can easily get acces to our object
    for(uint i=0; i<WhereIsMesh->animations.size(); i++) {
        WhereIsMesh->NamedAnimation.insert(std::make_pair(WhereIsMesh->animations[i].name,
                                                          WhereIsMesh->animations[i]));
    }
    reader.Close();
    AllMD2Meshes.insert(std::make_pair(Filename, WhereIsMesh));
    model->TheMD2Data = WhereIsMesh;
    delete[] uvs;
    delete[] tris;
    return true;
}


