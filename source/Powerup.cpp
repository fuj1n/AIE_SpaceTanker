#include "Powerup.h"

Powerup::Powerup(char* type, SPRITE sprite, int x, int y, int stayTime) {
	width = height = 64;
	powerupType = type;
	texture = DuplicateSprite(sprite);
	SetSpriteScale(texture, width, height);
	this->x = x;
	this->y = y;
	timeLeft = stayTime;
}

SPRITE Powerup::getTexture() {
	return texture;
}

void Powerup::update() {
	MoveSprite(texture, (float)x, (float)y);
	timeLeft--;
	if(timeLeft <= 0) {
		getApplication()->removeDrawable(this);
		return;
	}
}

void Powerup::destroySprites() {
	DestroySprite(texture);
}

unsigned int Powerup::getCX() {
	return x;
}

unsigned int Powerup::getCY() {
	return y;
}

unsigned int Powerup::getWidth() {
	return (unsigned int)width;
}

unsigned int Powerup::getHeight() {
	return (unsigned int)height;
}

bool Powerup::isCollideTester() {
	return false;
}

void Powerup::onCollide(ICollidable* col) {}

void Powerup::onTesterMessage(ICollidable* col) {
	timeLeft = 0;
}

std::string Powerup::getColliderName() {
	return std::string("powerup::") + std::string(powerupType);
}