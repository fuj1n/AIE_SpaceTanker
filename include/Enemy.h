#include "IDrawable.h"
#include "BaseClass.h"

#pragma once

class Enemy : public ICollidable, BaseClass{
private: 
	int width, height;
	float x, y, rotation, currentRotation, speed, scale;
	SPRITE texture;
	SPRITE explosionTextures[numExplosions];

	int followRange;

	bool isAlive, isDead;

	int explTicks;
	int explTex;

public:
	Enemy(SPRITE sprite, SPRITE explosionSprites[], int x, int y, float rotation, float speed, float scale = 1, float explosionScale = 1.5f, int followRange = 240);
	void update();
	SPRITE getTexture();
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
