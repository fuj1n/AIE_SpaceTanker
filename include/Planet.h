#include "IDrawable.h"
#include "bass.h"
#include "AIE.h"
#include "BaseClass.h"

#pragma once

class Planet : public IDrawable, BaseClass {
private:
	SPRITE texture;
public:
	Planet(const char* textureName, int x, int y, int bounds);
	SPRITE getTexture();
	void update();
	void destroySprites();
};