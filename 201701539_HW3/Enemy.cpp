#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy(wstring pPath, POINT pPos):GameObject(pPath, pPos)
{
}

Enemy::~Enemy() {

}

void Enemy::moveUp()
{
	this->pos.y -= 5;
}

void Enemy::moveDown()
{
	this->pos.y += 5;
}

D2D1_RECT_F Enemy::getDrawArea() {
	return D2D1::RectF(pos.x - (getBitmap()->GetSize().width), pos.y - (this->getBitmap()->GetSize().height) / 2,
		pos.x, (pos.y) + (getBitmap()->GetSize().height) / 2);
}
