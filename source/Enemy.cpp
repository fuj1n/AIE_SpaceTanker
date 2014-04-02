#include "Enemy.h"

#include "Utils.cpp"

Enemy::Enemy(SPRITE sprite, SPRITE explosionSprites[], int x, int y, float rotation, float speed, float scale, float explosionScale, int followRange){
	width = height = 64;

	float width = (float)this->width * scale, height = (float)this->height * scale;
	
	texture = DuplicateSprite(sprite);

	for(int i = 0; i < numExplosions; i++){
		explosionTextures[i] = DuplicateSprite(explosionSprites[i]);
	}

	this->x = (float)x;
	this->y = (float)y;
	this->rotation = rotation;
	this->currentRotation = this->rotation;
	this->speed = speed;
	this->scale = (float)scale;

	SetSpriteScale(texture, width, height);
	MoveSprite(texture, this->x, this->y);
	for(int i = 0; i < numExplosions; i++){
		float width = (float)this->width * explosionScale, height = (float)this->height * explosionScale;
		SetSpriteScale(explosionTextures[i], width, height);
		MoveSprite(explosionTextures[i], this->x, this->y);
	}

	this->followRange = followRange;

	isAlive = true;
	isDead = false;
}

void Enemy::update(){
	if(!isAlive && isDead){
		Application::instance->removeDrawable(this);
		return;
	}else if(!isAlive){
		if(explTex < 0){
			explTex = 0;
			explTicks = 0;
		}

		if(explTicks % (int)(Application::instance->getTickLimit() / 5) == 0){
			explTex++;
		}
		explTicks++;

		if(explTex == numExplosions){
			isDead = true;
			explTex--;
		}

		return;
	}

	int xSide = 0, ySide = 0;

	//Facing the player
	if(Application::instance->getTrackTarget() != 0 && (Application::instance->getTrackTarget()->getTX() - x < followRange && Application::instance->getTrackTarget()->getTX() - x > -followRange) && (Application::instance->getTrackTarget()->getTY() - y < followRange && Application::instance->getTrackTarget()->getTY() - y > -followRange)){
		xSide = Application::instance->getTrackTarget()->getTX() < x ? -1 : Application::instance->getTrackTarget()->getTX() > x ? 1 : 0;
		ySide = Application::instance->getTrackTarget()->getTY() < y ? -1 : Application::instance->getTrackTarget()->getTY() > y ? 1 : 0;
		
		GameUtils::rotate(rotation, xSide, ySide);
	}else{
		rotation = currentRotation > 180.f ? 1 : 181.f;
	}

	GameUtils::currRotation(currentRotation, rotation);

	//Movement
	switch(xSide){
	case -1:
		x -= 1 * speed;
		break;
	case 0:
		break;
	case 1:
		x += 1 * speed;
		break;
	}

	switch(ySide){
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
	for(int i = 0; i < numExplosions; i++){
		RotateSprite(explosionTextures[i], currentRotation);
		MoveSprite(explosionTextures[i], x, y);
	}
}

SPRITE Enemy::getTexture(){
	if(!isAlive){			
		return explosionTextures[explTex];
	}else{
		return texture;
	}
}

void Enemy::destroySprites(){
	DestroySprite(texture);
	for(int i = 0; i < numExplosions; i++){
		DestroySprite(explosionTextures[i]);
	}
}

unsigned int Enemy::getCX(){
	return (int)(x - width / 2);
}
	
unsigned int Enemy::getCY(){
	return (int)(y - height / 2);
}

unsigned int Enemy::getWidth(){
	return width;
}

unsigned int Enemy::getHeight(){
	return height;
}

bool Enemy::isCollideTester(){
	return true;
}
	
void Enemy::onCollide(ICollidable* col){
	if(col->getColliderName() == "bullet" && !(col->parent == this)){
		if(isAlive){
			col->onTesterMessage(this);
			BASS_ChannelPlay(Application::instance->getGameObjects()->explosionSound, true);
		}
		isAlive = false;
		isDead = false;
	}
}

void Enemy::onTesterMessage(ICollidable* col){
	if(isAlive){
		BASS_ChannelPlay(Application::instance->getGameObjects()->explosionSound, true);
	}
	isAlive = false;
	isDead = false;
}

std::string Enemy::getColliderName(){
	return std::string("enemy") + (!isAlive ? std::string("::dead") : std::string(""));
}