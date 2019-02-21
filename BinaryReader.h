#pragma once

#include <fstream>
#undef countof
#include "glm/glm.hpp"


class BinaryReader
{
public:
    BinaryReader();
    ~BinaryReader();

public:
    bool Load(const char* filename);
    void Close();
    unsigned char ReadByte();
    unsigned char* ReadBytes(int numBytes);
    void* ReadAny(int size);
    int ReadInt();
    float ReadFloat();
    short ReadShort();
    glm::vec3 ReadVec3();
    glm::vec3 ReadCompressedVec3();
    void Seek(int position);

protected:
    FILE *fileHandle;
    unsigned char *data;
    int readPosition;
};

