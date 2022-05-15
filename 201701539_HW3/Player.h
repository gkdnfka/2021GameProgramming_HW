#pragma once
#include "GameObject.h"
class Player : public GameObject
{
public:
	Player(wstring pPath, POINT pPos);
	~Player();
	
	void moveUp();
	void moveDown();
};

