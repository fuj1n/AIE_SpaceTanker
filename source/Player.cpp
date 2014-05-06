#include "Player.h"
#include "Utils.cpp"

Player::Player(SPRITE sprite) {
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
	maxSprintTime = (int)(5 * getApplication()->getTickLimit());
	SetSpriteScale(texture, w, h);
	MoveSprite(texture, x, y);
	getApplication()->maxSprintCooldown = maxSprintTime;
	getApplication()->sprintCooldown = (int)sprintCooldown;
	upgrades->availableCoins = 5000;
}

SPRITE Player::getTexture() {
	return texture;
}

unsigned int Player::getCX() {
	return (unsigned int)x - width / 2;
}

unsigned int Player::getCY() {
	return (unsigned int)y - width / 2;
}

unsigned int Player::getTX() {
	return (unsigned int)x;
}

unsigned int Player::getTY() {
	return (unsigned int)y;
}

void Player::update() {
	if(health <= 0) {
		health = -1337;
		getApplication()->endGame();
	}

	if(upgrades->healthAdded > 0) {
		health += upgrades->healthAdded;
		upgrades->healthAdded = 0;
		if(health > upgrades->calculateMaxHealth()) {
			health = upgrades->calculateMaxHealth();
		} else if(health < 0) {
			health = 0;
		}
	}

	static bool isSCooldown;

	//Decrement the values for timed "things"
	if(shootCooldown > 0) {
		shootCooldown--;
	}
	if(sprintCooldown > 0) {
		sprintCooldown -= upgrades->sprintCooldownSpeed < 5 ? upgrades->sprintCooldownSpeed * 0.2f : 1.f;
	} else {
		isSCooldown = false;
	}

	static bool lastSpeed;
	float speed = this->speed;

	if(sprintCooldown <= maxSprintTime && !isSCooldown) {
		speed = this->speed * (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT) ? 2 : 1);
		sprintCooldown += IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT) ? 5 - upgrades->sprintDuration + 1 : 0;
		if(!lastSpeed && (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT))) {
			lastSpeed = true;
			BASS_ChannelPlay(getApplication()->getGameObjects()->speedUpSound, true);
		} else if(lastSpeed && (!IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT))) {
			lastSpeed = false;
			isSCooldown = true;
		}
	} else {
		lastSpeed = false;
		isSCooldown = true;
	}

	speed += (upgrades->speed * 0.3f);

	GameUtils::movePlayer(x, y, rotation, speed, IsKeyDown('w'), IsKeyDown('s'), IsKeyDown('a'), IsKeyDown('d'), width, height);

	if(IsKeyDown(VK_SPACE) && shootCooldown <= 0) {
		int projXDir = -1337, projYDir = -1337, projX = (int)x, projY = (int)y;

		GameUtils::getProjectilePropertyModifiers(rotation, projXDir, projYDir, projX, projY);

		if(projXDir != -1337 && projYDir != -1337) {
			float speed, range;
			speed = 4.5f * (upgrades->bulletSpeed * 0.5f);
			range = 1.f * (upgrades->maxRange * 0.25f);
			Projectile* projectile = new Projectile(getApplication()->getGameObjects()->laserBeamSprite, projX, projY, projXDir, projYDir, rotation, speed, range, SColour(0xFFFF00FF), 25, false, this);
			getApplication()->addDrawable(projectile);
			BASS_ChannelPlay(getApplication()->getGameObjects()->laserFireSound, false);
			shootCooldown = (int)((0.2f *(5 - upgrades->fireRate + 1)) * getApplication()->getTickLimit());
		}
	}

	GameUtils::currRotation(currentRotation, rotation);

	RotateSprite(texture, currentRotation);
	MoveSprite(texture, x, y);
	getApplication()->positionCamera((int)x - getApplication()->getScreenWidth() / 2, (int)y - getApplication()->getScreenHeight() / 2);
	getApplication()->sprintCooldown = (int)sprintCooldown;
	getApplication()->playerHealth = health;
}

void Player::destroySprites() {
	DestroySprite(texture);
}

unsigned int Player::getWidth() {
	return width;
}

unsigned int Player::getHeight() {
	return height;
}

bool Player::isCollideTester() {
	return true;
}

void Player::onCollide(ICollidable* col) {
	std::string colliderName = col->getColliderName();

	int damRes = upgrades->calculateDamageResistance();
	damRes = Random::random(0, damRes);
	if(colliderName == "bullet" && !(col->parent == this)) {
		health -= (10 - damRes) > 0 ? (10 - damRes) : 0;
		col->onTesterMessage(this);
	} else if(colliderName == "enemy") {
		health -= (25 - damRes) > 0 ? (25 - damRes) : 0;
		col->onTesterMessage(this);
	} else if(colliderName == "powerup::health") {
		health += 35;
		col->onTesterMessage(this);
		BASS_ChannelPlay(getApplication()->getGameObjects()->healthUpSound, false);
	} else if(colliderName == "powerup::coins") {
		upgrades->availableCoins += 10;
		col->onTesterMessage(this);
		BASS_ChannelPlay(getApplication()->getGameObjects()->coinPickupSound, true);
	}

	int maxHealth = upgrades->calculateMaxHealth();
	if(health < 0) {
		health = 0;
	} else if(health > maxHealth) {
		health = maxHealth;
	}
}

void Player::onTesterMessage(ICollidable* col) {}

void Player::onAction(int act) {
	switch(act) {
	case 0:
		unsigned long long score = getApplication()->getScore();
		getApplication()->setScore(score + 15);
		break;
	}
}

std::string Player::getColliderName() {
	return "player";
}

PlayerUpgrades* Player::getUpgrades() {
	return upgrades;
}