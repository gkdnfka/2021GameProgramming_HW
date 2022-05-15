#pragma once
class GameObject
{
private:
	wstring resourcePath;
	
	ID2D1Bitmap* objectBitmap;
public:
	POINT pos;

	GameObject(wstring pPath, POINT pPos);
	virtual ~GameObject();

	virtual D2D1_RECT_F getDrawArea();

	wstring getPath() { return resourcePath; }
	void setPath(wstring pPath) { resourcePath = pPath; }

	POINT getPos() { return pos; }
	void setPos(POINT pPos) { pos = pPos; }

	ID2D1Bitmap* getBitmap() { return objectBitmap; }
	ID2D1Bitmap** getBitmapRef() { return &objectBitmap; }
};

