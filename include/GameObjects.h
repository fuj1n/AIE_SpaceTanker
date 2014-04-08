#include "bass.h"

#pragma once

typedef unsigned int SPRITE;

typedef enum RotationDirections{
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

class GameObjects{
public:
	//Sprites
	SPRITE splashTexture;
	SPRITE logoTexture;
	SPRITE instructionTexture;
	SPRITE menuButtons[4];
	SPRITE stars;

	SPRITE laserBeamSprite;
	SPRITE laserPowerUpSprite;
	SPRITE healthPowerUpSprite;
	SPRITE playerSprite;

	SPRITE healthRemainSprite;

	SPRITE enemySprite;

	SPRITE explosionSprites[numExplosions];

	//Sounds
	HSTREAM backgroundLoop;
	HSTREAM laserFireSound;
	HSTREAM speedUpSound;
	HSTREAM powerUpSound;
	HSTREAM powerDownSound;
	HSTREAM healthUpSound;
	HSTREAM explosionSound;

	~GameObjects(){
		//delete &splashTexture, &logoTexture, &instructionTexture, &stars, &laserBeamSprite, &laserPowerUpSprite, &healthPowerUpSprite, &playerSprite, &enemySprite;
		//delete[4] &menuButtons;
		//delete[numExplosions] &explosionSprites;

		BASS_StreamFree(backgroundLoop);
		BASS_StreamFree(laserFireSound);
		BASS_StreamFree(speedUpSound);
		BASS_StreamFree(powerUpSound);
		BASS_StreamFree(powerDownSound);
		BASS_StreamFree(healthUpSound);
		BASS_StreamFree(explosionSound);
	}
};