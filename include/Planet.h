#include "IDrawable.h"
#include "bass.h"
#include "AIE.h"

#pragma once

class Planet : public IDrawable{
private:
	SPRITE texture;
public:
	Planet(const char* textureName, int x, int y, int bounds);
	SPRITE getTexture();
	void update();
	void destroySprites();
};