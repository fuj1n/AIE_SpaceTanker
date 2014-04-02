#include "Player.h"
#include "Utils.cpp"

Player::Player(SPRITE sprite){
	scale = 2;
	width = height = 64;
	float w = 64 * scale, h = 64 * scale;
	x = WORLD_WIDTH / 2;
	y = WORLD_HEIGHT / 2;
	rotation = ROT_EAST;
	currentRotation = rotation;
	speed = 1.5F;
	health = 100;
	texture = DuplicateSprite(sprite);
	sprintCooldown = 0;
	maxSprintTime = (int)(5 * Application::instance->getTickLimit());
	SetSpriteScale(texture, w, h);
	MoveSprite(texture, x, y);
}

SPRITE Player::getTexture(){
	return texture;
}

unsigned int Player::getCX(){
	return (unsigned int)x - width / 2;
}

unsigned int Player::getCY(){
	return (unsigned int)y - width / 2;
}

unsigned int Player::getTX(){
	return (unsigned int)x;
}

unsigned int Player::getTY(){
	return (unsigned int)y;
}

void Player::update(){
	static bool isSCooldown;

	//Decrement the values for timed "things"
	if(shootCooldown > 0){
		shootCooldown--;
	}
	if(betterAmmoCooldown > 0){
		if(betterAmmoCooldown == 1){
			BASS_ChannelPlay(Application::instance->getGameObjects()->powerDownSound, false);
		}
		betterAmmoCooldown--;
	}
	if(sprintCooldown > 0){
		static bool cooldownTick;
		if(cooldownTick){
			sprintCooldown--;
		}
		cooldownTick = !cooldownTick;
	}else{
		isSCooldown = false;
	}

	static bool lastSpeed;
	float speed = this->speed;

	if(sprintCooldown <= maxSprintTime && !isSCooldown){
		speed = this->speed * (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT) ? 2 : 1);
		sprintCooldown += IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT) ? 1 : 0;
		if(!lastSpeed && (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT))){
			lastSpeed = true;
			BASS_ChannelPlay(Application::instance->getGameObjects()->speedUpSound, false);
		}else if(lastSpeed && (!IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT))){
			lastSpeed = false;
			isSCooldown = true;
		}
	}else{
		lastSpeed = false;
		isSCooldown = true;
	}
	
	GameUtils::movePlayer(x, y, rotation, speed, IsKeyDown('w'), IsKeyDown('s'), IsKeyDown('a'), IsKeyDown('d'), width, height);

	if(IsKeyDown(VK_SPACE) && shootCooldown <= 0){
		int projXDir = -1337, projYDir = -1337, projX = (int)x, projY = (int)y;
		
		GameUtils::getProjectilePropertyModifiers(rotation, projXDir, projYDir, projX, projY);
	
		if(projXDir != -1337 && projYDir != -1337){
			Projectile* projectile = new Projectile(Application::instance->getGameObjects()->laserBeamSprite, projX, projY, projXDir, projYDir, rotation, betterAmmoCooldown > 0 ? 5.5f : 4.5f, betterAmmoCooldown > 0 ? 1.5f : 1, betterAmmoCooldown > 0 ? SColour(0x00FFFFFF) : SColour(0xFFFF00FF), betterAmmoCooldown > 0 ? 30 : 20, betterAmmoCooldown > 0, this);
			Application::instance->addDrawable(projectile);
			BASS_ChannelPlay(Application::instance->getGameObjects()->laserFireSound, false);
			shootCooldown = (int)(0.5 * Application::instance->getTickLimit());
		}
	}

	GameUtils::currRotation(currentRotation, rotation);

	RotateSprite(texture, currentRotation);
	MoveSprite(texture, x, y);
	Application::instance->positionCamera((int)x - Application::instance->getScreenWidth() / 2, (int)y - Application::instance->getScreenHeight() / 2);
}

void Player::destroySprites(){
	DestroySprite(texture);
}

unsigned int Player::getWidth(){
	return width;	
}

unsigned int Player::getHeight(){
	return height;
}

bool Player::isCollideTester(){
	return true;
}

void Player::onCollide(ICollidable* col){
	std::string colliderName = col->getColliderName();

	if(colliderName == "bullet" && !(col->parent == this)){
		health -= 10;
		col->onTesterMessage(this);
	}else if(colliderName == "enemy"){
		health -= 15;
		col->onTesterMessage(this);
	}else if(colliderName == "powerup::health"){
		health += 35;
		col->onTesterMessage(this);
		BASS_ChannelPlay(Application::instance->getGameObjects()->healthUpSound, false);
	}else if(colliderName == "powerup::laser"){
		betterAmmoCooldown = 900;
		col->onTesterMessage(this);
		BASS_ChannelPlay(Application::instance->getGameObjects()->powerUpSound, false);
	}

	if(health < 0){
		health = 0;
	}else if(health > 100){
		health = 100;
	}
}

void Player::onTesterMessage(ICollidable* col){}

std::string Player::getColliderName(){
	return "player";
}