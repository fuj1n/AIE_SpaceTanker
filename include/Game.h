#pragma comment(lib, "winmm") 

#include "bass.h"
#include <string>

typedef unsigned int SPRITE;

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

//Sounds
HSTREAM backgroundLoop;
HSTREAM laserFireSound;
HSTREAM speedUpSound;
HSTREAM powerUpSound;
HSTREAM powerDownSound;
HSTREAM healthUpSound;

class IParent{

};

class IDrawable : public IParent{
public: 
	IParent* parent;

	virtual SPRITE getTexture() = 0;
	virtual void update() = 0;
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
	virtual unsigned int getTX() = 0;
	virtual unsigned int getTY() = 0;
};