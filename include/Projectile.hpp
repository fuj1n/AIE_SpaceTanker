#include "IDrawable.hpp"
#include "AIE.h"
#include "Application.hpp"
#include "BaseClass.hpp"

#pragma once

class Projectile : public ICollidable, BaseClass {
private:
	float width, height;
	int xDir, yDir, pixelsTravellable;
	boolean isSpecial;
	float x, y, rotation, currentRotation, speed, scale;

	SPRITE texture;
public:
	Projectile(SPRITE sprite, int x, int y, int xDir, int yDir, float rotation, float speed, float stayTime, SColour color = SColour(0xFFFFFFFF), int length = 20, bool special = false, IParent* parent = 0);
	SPRITE getTexture();
	void update();
	void destroySprites();
	unsigned int getCX();
	unsigned int getCY();
	unsigned int getWidth();
	unsigned int getHeight();
	bool isCollideTester();
	void onCollide(ICollidable* col);
	void onTesterMessage(ICollidable* col);
	std::string getColliderName();
};