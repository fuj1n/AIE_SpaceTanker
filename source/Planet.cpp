#include "Planet.hpp"

Planet::Planet(const char* textureName, int x, int y, int bounds) {
	texture = CreateSprite(textureName, bounds, bounds, false, SColour(0xFFFFFF77));

	SetSpriteBlendMode(texture, _SRC_COLOR, _DST_COLOR);

	MoveSprite(texture, (float)x, (float)y);
}

SPRITE Planet::getTexture() {
	return texture;
}

void Planet::update() {}

void Planet::destroySprites() {
	DestroySprite(texture);
}