#include "Projectile.h"

Projectile::Projectile(SPRITE sprite, int x, int y, int xDir, int yDir, float rotation, float speed, float stayTime, SColour color, int length, bool special, IParent* parent){
	scale = 1;
	width = 10 * scale;
	height = length * scale;
	this->x = (float)x;
	this->y = (float)y;
	this->xDir = xDir;
	this->yDir = yDir;
	this->rotation = rotation;
	currentRotation = rotation;
	this->speed = speed;
	isSpecial = special;
	timeUntilDeath = (int)(stayTime * getApplication()->getTickLimit());
	texture = DuplicateSprite(sprite);
	SetSpriteScale(texture, width, height);
	SetSpriteColour(texture, color);
	MoveSprite(texture, this->x, this->y);
		
	this->parent = parent;
}

SPRITE Projectile::getTexture(){
	return texture;
}

void Projectile::update(){
	timeUntilDeath--;

	x += xDir * speed;
	y += yDir * speed;

	for(int index = 0; index < 2; index++){
		if(currentRotation > rotation){
			currentRotation -= 5;
		}else if(currentRotation < rotation){
			currentRotation += 5;
		}
	}
	
	RotateSprite(texture, currentRotation);
	MoveSprite(texture, x, y);

	if(timeUntilDeath <= 0){
		getApplication()->removeDrawable(this);
		return;
	}
}

void Projectile::destroySprites(){
	DestroySprite(texture);
}

unsigned int Projectile::getCX(){
	return (int)x;
}

unsigned int Projectile::getCY(){
	return (int)y;
}

unsigned int Projectile::getWidth(){
	return (unsigned int)width;	
}

unsigned int Projectile::getHeight(){
	return (unsigned int)height;
}

bool Projectile::isCollideTester(){
	return false;
}

void Projectile::onCollide(ICollidable* col){}

void Projectile::onTesterMessage(ICollidable* col){
	if(!isSpecial){
		timeUntilDeath = 0;
	}
}

std::string Projectile::getColliderName(){
	return std::string("bullet") + (timeUntilDeath <= 0 ? std::string("::dead") : std::string(""));
}