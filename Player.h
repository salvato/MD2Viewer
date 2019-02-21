#pragma once

#include "MD2Model.h"


class Player : public MD2Model
{
public:
    Player() {}
    Player(MyFiles* FH, const char* FN, ModelManager* MM);
    ~Player() {}

public:
    bool Update(); // we only need to supply the update, Draw is part of MD2
    // some helpful systems to deal with hitting things.
    void HandleCollision(const ObjectModel* WhatDidIHit);
    void HandleSeparation(const ObjectModel* WhatDidIHit);
    void HandleContact(const ObjectModel* WhatDidIHit);

protected:
    char buffer[100];
};
