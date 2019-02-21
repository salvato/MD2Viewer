#include "BinaryReader.h"

#include <iostream>
#include <cstddef>


BinaryReader::BinaryReader() {
    fileHandle = nullptr;
    data = nullptr;
}


BinaryReader::~BinaryReader() {
    if(data != nullptr) delete data; //if we failed to load it may cause issues
}


bool
BinaryReader::Load(const char* filename) {
    fileHandle = fopen(filename, "rb");
    if(!fileHandle) {
        Close();
        return false;
    }
    fseek(fileHandle, 0, SEEK_END);
    ulong fileSize = ulong(ftell(fileHandle));
    data = new unsigned char[fileSize];
    fseek(fileHandle, 0, SEEK_SET);
    fread(data, 1, fileSize, fileHandle);
    Close();
    readPosition = 0;
    return true;
}


void
BinaryReader::Close() {
    if(fileHandle != nullptr) {
        fclose(fileHandle);
        fileHandle = nullptr;
    }
}


unsigned char
BinaryReader::ReadByte() {
    unsigned char res = data[readPosition];
    readPosition++;
    return res;
}


void*
BinaryReader::ReadAny(int size) {
    void* res = &data[readPosition];
    readPosition += size;
    return res;
}


int
BinaryReader::ReadInt() {
    int res = *(reinterpret_cast<int*>(&data[readPosition]));
    readPosition += sizeof(int);
    return res;
}


float
BinaryReader::ReadFloat() {
    float res = *(reinterpret_cast<float*>(&data[readPosition]));
    readPosition += sizeof(float);
    return res;
}


short
BinaryReader::ReadShort() {
    short res = *(reinterpret_cast<short*>(&data[readPosition]));
    readPosition += sizeof(short);
    return res;
}


glm::vec3
BinaryReader::ReadVec3() {
    glm::vec3 vec;
    vec.x = ReadFloat();
    vec.y = ReadFloat();
    vec.z = ReadFloat();
    return vec;
}


glm::vec3
BinaryReader::ReadCompressedVec3() {
    glm::vec3 vec;
    vec.x = ReadByte();
    vec.y = ReadByte();
    vec.z = ReadByte();
    return vec;
}


void
BinaryReader::Seek(int position) {
    readPosition = position;
}
