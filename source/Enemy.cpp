#include "Enemy.hpp"

#include "Utils.cpp"
#include "Powerup.hpp"

Enemy::Enemy(SPRITE sprite, SPRITE explosionSprites[], int x, int y, float rotation, float speed, float scale, float explosionScale, int followRange) {
	width = height = 64;

	float width = (float)this->width * scale, height = (float)this->height * scale;

	texture = DuplicateSprite(sprite);

	for(int i = 0; i < numExplosions; i++) {
		explosionTextures[i] = DuplicateSprite(explosionSprites[i]);
	}

	this->x = (float)x;
	this->y = (float)y;
	this->rotation = rotation;
	this->currentRotation = this->rotation;
	this->speed = speed;
	this->scale = (float)scale;
	this->dropCoins = false;
	this->explTex = 0;
	this->explTicks = 0;

	SetSpriteScale(texture, width, height);
	MoveSprite(texture, this->x, this->y);
	for(int i = 0; i < numExplosions; i++) {
		float width = (float)this->width * explosionScale, height = (float)this->height * explosionScale;
		SetSpriteScale(explosionTextures[i], width, height);
		MoveSprite(explosionTextures[i], this->x, this->y);
	}

	this->followRange = followRange;

	isAlive = true;
	isDead = false;
}

void Enemy::update() {
	if(!isAlive && isDead) {
		if(dropCoins && Random::random(0, 52) != 0) {
			getApplication()->addDrawable(new Powerup("coins", getApplication()->getGameObjects()->coinsSprite, (int)x - getWidth() / 2, (int)y - getHeight() / 2, (int)(30 * getApplication()->getTickLimit())));
		}
		getApplication()->removeDrawable(this);
		return;
	} else if(!isAlive) {
		if(explTicks % (int)(getApplication()->getTickLimit() / 5) == 0) {
			explTex++;
		}
		explTicks++;

		if(explTex == numExplosions - 1) {
			isDead = true;
			explTex--;
		}

		return;
	}

	int xSide = 0, ySide = 0;

	//Facing the player
	//if(getApplication()->getTrackTarget() != 0 && (getApplication()->getTrackTarget()->getTX() - x < followRange && getApplication()->getTrackTarget()->getTX() - x > -followRange) && (getApplication()->getTrackTarget()->getTY() - y < followRange && getApplication()->getTrackTarget()->getTY() - y > -followRange)) {
	xSide = getApplication()->getTrackTarget()->getTX() < x ? -1 : getApplication()->getTrackTarget()->getTX() > x ? 1 : 0;
	ySide = getApplication()->getTrackTarget()->getTY() < y ? -1 : getApplication()->getTrackTarget()->getTY() > y ? 1 : 0;

	GameUtils::rotate(rotation, xSide, ySide);
	//}

	GameUtils::currRotation(currentRotation, rotation);

	//Movement
	switch(xSide) {
	case -1:
		x -= 1 * speed;
		break;
	case 0:
		break;
	case 1:
		x += 1 * speed;
		break;
	}

	switch(ySide) {
	case -1:
		y -= 1 * speed;
		break;
	case 0:
		break;
	case 1:
		y += 1 * speed;
		break;
	}

	RotateSprite(texture, currentRotation);
	MoveSprite(texture, x, y);
	for(int i = 0; i < numExplosions; i++) {
		RotateSprite(explosionTextures[i], currentRotation);
		MoveSprite(explosionTextures[i], x, y);
	}
}

SPRITE Enemy::getTexture() {
	if(!isAlive) {
		return explosionTextures[explTex];
	} else {
		return texture;
	}
}

void Enemy::destroySprites() {
	DestroySprite(texture);
	for(int i = 0; i < numExplosions; i++) {
		DestroySprite(explosionTextures[i]);
	}
}

unsigned int Enemy::getCX() {
	return (int)(x - width / 2);
}

unsigned int Enemy::getCY() {
	return (int)(y - height / 2);
}

unsigned int Enemy::getWidth() {
	return width;
}

unsigned int Enemy::getHeight() {
	return height;
}

bool Enemy::isCollideTester() {
	return true;
}

void Enemy::onCollide(ICollidable* col) {
	if(col->getColliderName() == "bullet" && !(col->parent == this)) {
		if(isAlive) {
			col->onTesterMessage(this);
			BASS_ChannelPlay(getApplication()->getGameObjects()->explosionSound, true);

			isAlive = false;
			isDead = false;
			dropCoins = true;
		}
	}
}

void Enemy::onTesterMessage(ICollidable* col) {
	col;
	if(isAlive) {
		BASS_ChannelPlay(getApplication()->getGameObjects()->explosionSound, true);

		isAlive = false;
		isDead = false;
	}
}

std::string Enemy::getColliderName() {
	return std::string("enemy") + (!isAlive ? std::string("::dead") : std::string(""));
}