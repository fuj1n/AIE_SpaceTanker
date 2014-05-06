#include "IDrawable.h"
#include "Application.h"
#include "AIE.h"
#include "Projectile.h"
#include "BaseClass.h"

#pragma once

class Player : public ITrackable, public ICollidable, BaseClass {
private:
	int width, height;
	float x, y, rotation, currentRotation, speed, scale;
	int health;

	int shootCooldown, maxSprintTime;
	float sprintCooldown;

	SPRITE texture;

	PlayerUpgrades* upgrades = new PlayerUpgrades();
public:
	Player(SPRITE sprite);
	SPRITE getTexture();
	unsigned int getCX();
	unsigned int getCY();
	unsigned int getTX();
	unsigned int getTY();
	void update();
	void destroySprites();
	unsigned int getWidth();
	unsigned int getHeight();
	bool isCollideTester();
	void onCollide(ICollidable* col);
	void onTesterMessage(ICollidable* col);
	std::string getColliderName();
	void onAction(int act);
	PlayerUpgrades* getUpgrades();
};