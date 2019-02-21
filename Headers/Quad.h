#pragma once
#include "ObjectModel.h"


//A simple quad model that is designed to be viewed as a map or viewscreen 
class Quad : ObjectModel
{
public:
    Quad();
    ~Quad();

public:
    bool Update();
    bool Draw();

private:
};
