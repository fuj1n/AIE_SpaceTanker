#pragma comment(lib, "winmm") 

#include "bass.h"
#include <string>

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

//Sprites
SPRITE splashTexture;
SPRITE instructionTexture;
SPRITE menuButtons[4];
SPRITE stars;

SPRITE laserBeamSprite;
SPRITE laserPowerUpSprite;
SPRITE healthPowerUpSprite;
SPRITE playerSprite;

SPRITE enemySprite;

const int numExplosions = 10;
SPRITE explosionSprites[numExplosions];

//Sounds
HSTREAM backgroundLoop;
HSTREAM laserFireSound;
HSTREAM speedUpSound;
HSTREAM powerUpSound;
HSTREAM powerDownSound;
HSTREAM healthUpSound;
HSTREAM explosionSound;

const int WORLD_WIDTH = 960, WORLD_HEIGHT = 960;

class IParent{

};

class IDrawable : public IParent{
public: 
	IParent* parent;

	virtual SPRITE getTexture() = 0;
	virtual void update() = 0;
	virtual void destroySprites() = 0;
};

class ICollidable : public IDrawable{
public:
	virtual unsigned int getCX() = 0;
	virtual unsigned int getCY() = 0;
	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;
	virtual bool isCollideTester() = 0;
	virtual void onCollide(ICollidable*) = 0;
	virtual void onTesterMessage(ICollidable*) = 0;

	virtual std::string getColliderName() = 0;
};

class ITrackable{
public:
	virtual unsigned int getTX() = 0;
	virtual unsigned int getTY() = 0;
};