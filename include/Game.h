#pragma comment(lib, "winmm") 

#include "bass.h"
#include <string>

//Sprites
unsigned int splashTexture;
unsigned int instructionTexture;
unsigned int menuButtons[4];
unsigned int stars;

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

	virtual unsigned int getTexture() = 0;
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