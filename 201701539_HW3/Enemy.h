#pragma once
#include "GameObject.h"
class Enemy :
    public GameObject
{
public:
    Enemy(wstring pPath, POINT pPos);
    ~Enemy();

    void moveUp();
    void moveDown();

    D2D1_RECT_F getDrawArea();
};

