#include "Enemy.hpp"
#include "EnemySpawner.hpp"

EnemySpawner::EnemySpawner(int x, int y, int safezone, int negspawnrange, int posspawnrange, bool revX, SPRITE tex) {
	this->x = x;
	this->y = y;
	this->safediameter = safezone;
	this->negspawndiameter = negspawnrange;
	this->posspawndiameter = posspawnrange;
	this->texture = DuplicateSprite(tex);
	MoveSprite(texture, (float)x, (float)y);
	this->revX = revX;

	this->spawnticks = 0;
	this->spawninterval = (int)(0.3 * getApplication()->getTickLimit());
}

SPRITE EnemySpawner::getTexture() {
	return texture;
}

void EnemySpawner::update() {
	bool shouldSpawn = spawnticks >= spawninterval && getApplication()->getEnemyCount() < 100 && Random::random(0, getApplication()->getSpawnEase()) == 0;
	float distance = Math::dist((float)x, (float)y, (float)getApplication()->getTrackTarget()->getTX(), (float)getApplication()->getTrackTarget()->getTY());

	if(distance > safediameter && shouldSpawn) {
		spawnticks = 0;
		int xPos = Random::random(revX ? -posspawndiameter / 2 : negspawndiameter / 2, posspawndiameter / 2);
		int yPos = Random::random(revX ? -posspawndiameter / 2 : negspawndiameter / 2, posspawndiameter / 2);
		int followRange = Random::random(480, 640);

		getApplication()->addDrawable(new Enemy(getApplication()->getGameObjects()->enemySprite, getApplication()->getGameObjects()->explosionSprites, xPos + x, yPos + y, ROT_EAST, 1.5f, 1.f, 1.5f, followRange));
	} else if(spawnticks < spawninterval) {
		spawnticks++;
	} else {
		spawnticks = 0;
	}
}

void EnemySpawner::destroySprites() {
	DestroySprite(texture);
}