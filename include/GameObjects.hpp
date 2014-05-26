#include "bass.h"
#include "AIE.h"

#pragma once

typedef unsigned int SPRITE;

typedef enum RotationDirections {
	ROT_NORTH = 0,
	ROT_NORTHWEST = 45,
	ROT_WEST = 90,
	ROT_SOUTHWEST = 135,
	ROT_SOUTH = 180,
	ROT_SOUTHEAST = 225,
	ROT_EAST = 270,
	ROT_NORTHEAST = 315
}RotationDirections;

const int numExplosions = 10;

class GameObjects {
public:
	//Sprites
	SPRITE splashTexture;
	SPRITE logoTexture;
	SPRITE instructionTexture;
	SPRITE menuButtons[4];
	SPRITE stars;

	SPRITE guiFrameSprite;
	SPRITE guiTankerCustomisationPrefabSprite;
	SPRITE guiFullyUpgradedPrefabSprite;

	SPRITE laserBeamSprite;
	SPRITE healthPowerUpSprite;
	SPRITE playerSprite;
	SPRITE coinsSprite;

	SPRITE healthRemainSprite;
	SPRITE coinsOwnedSprite;

	SPRITE enemySprite;

	SPRITE explosionSprites[numExplosions];

	//Sounds
	HSTREAM menuSound;
	HSTREAM backgroundLoop;
	HSTREAM laserFireSound;
	HSTREAM speedUpSound;
	HSTREAM healthUpSound;
	HSTREAM explosionSound;
	HSTREAM coinPickupSound;

	struct PlayerUpgrades {
		SPRITE speed, maxHealth, bulletSpeed, damageResistance, maxRange, sprintDuration, sprintCooldownSpeed, fireRate;

		void declareSprites() {
			speed = CreateSprite("./images/upgrades/speed.png", 16, 16, false);
			maxHealth = CreateSprite("./images/upgrades/health.png", 16, 16, false);
			bulletSpeed = CreateSprite("./images/upgrades/bulletSpeed.png", 16, 16, false);
			damageResistance = CreateSprite("./images/upgrades/armor.png", 16, 16, false);
			maxRange = CreateSprite("./images/upgrades/range.png", 16, 16, false);
			sprintDuration = CreateSprite("./images/upgrades/sprintDuration.png", 16, 16, false);
			sprintCooldownSpeed = CreateSprite("./images/upgrades/sprintCooldownSpeed.png", 16, 16, false);
			fireRate = CreateSprite("./images/upgrades/fireRate.png", 16, 16, false);
		}
	} playerUpgrades;

	~GameObjects() {
		//delete &splashTexture, &logoTexture, &instructionTexture, &stars, &laserBeamSprite, &laserPowerUpSprite, &healthPowerUpSprite, &playerSprite, &enemySprite;
		//delete[4] &menuButtons;
		//delete[numExplosions] &explosionSprites;

		BASS_StreamFree(menuSound);
		BASS_StreamFree(backgroundLoop);
		BASS_StreamFree(laserFireSound);
		BASS_StreamFree(speedUpSound);
		BASS_StreamFree(healthUpSound);
		BASS_StreamFree(explosionSound);
		BASS_StreamFree(coinPickupSound);
	}
};