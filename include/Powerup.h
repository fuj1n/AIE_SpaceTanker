#include "IDrawable.h"
#include "AIE.h"
#include "Application.h"
#include "BaseClass.h"

#pragma once

class Powerup : public ICollidable, BaseClass{
private:
	float width, height;
	char* powerupType;

	unsigned int x, y;
	SPRITE texture;
	int timeLeft;
public:
	Powerup(char* type, SPRITE sprite, int x, int y, int stayTime);
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