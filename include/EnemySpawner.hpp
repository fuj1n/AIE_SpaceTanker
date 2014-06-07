#include "IDrawable.hpp"
#include "BaseClass.hpp"

class EnemySpawner : public IDrawable, public BaseClass {
private:
	SPRITE texture;
	int x, y, safediameter, negspawndiameter, posspawndiameter;
	int spawnticks, spawninterval;
	bool revX;
public:
	EnemySpawner(int x, int y, int safezone, int negspawnrange, int posspawnrange, bool revX, SPRITE tex);
	SPRITE getTexture();
	void update();
	void destroySprites();
};