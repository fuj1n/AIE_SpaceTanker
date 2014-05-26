#include "GameObjects.hpp"
#include <string>

#pragma once

class IParent {
public:
	virtual void onAction(int act) {}
};

class IMessageRecepient {
	virtual int onMessage(std::string message) = 0;
};

class IDrawable : public IParent {
public:
	IParent* parent;

	virtual SPRITE getTexture() = 0;
	virtual void update() = 0;
	virtual void destroySprites() = 0;
};

class ICollidable : public IDrawable {
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

class ITrackable {
public:
	virtual unsigned int getTX() = 0;
	virtual unsigned int getTY() = 0;
};