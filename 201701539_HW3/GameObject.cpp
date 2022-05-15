#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject(wstring pPath, POINT pPos) {
	resourcePath = pPath;
	pos = pPos;
}

GameObject::~GameObject() {
	SAFE_RELEASE(objectBitmap);
}

D2D1_RECT_F GameObject::getDrawArea() {
	return D2D1::RectF(pos.x, pos.y - (this->getBitmap()->GetSize().height) / 2,
		pos.x + (this->getBitmap()->GetSize().width), (pos.y) + (this->getBitmap()->GetSize().height) / 2);
}