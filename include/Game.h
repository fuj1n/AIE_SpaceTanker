#pragma comment(lib, "winmm") 

#include "bass.h"

//Sprites
unsigned int splashTexture;
unsigned int instructionTexture;
unsigned int menuButtons[4];
unsigned int stars;

//Sounds
HSTREAM backgroundLoop;
HSTREAM laserFireSound;
HSTREAM speedUpSound;

class IDrawable{
public: 
	virtual unsigned int getTexture() = 0;
	virtual void update() = 0;
	virtual bool isCollidable() = 0;
};

class ICollidable{
public:
	virtual unsigned int getX() = 0;
	virtual unsigned int getY() = 0;
	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;
	virtual bool isCollideTester() = 0;
	virtual void onCollide(ICollidable*) = 0;
	virtual void onTesterMessage(ICollidable*) = 0;

	virtual char* getColliderName() = 0;
};

class ITrackable{
	virtual unsigned int getX() = 0;
	virtual unsigned int getY() = 0;
};