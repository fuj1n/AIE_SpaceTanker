#include "IDrawable.hpp"
#include "bass.h"
#include "AIE.h"
#include "BaseClass.hpp"

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