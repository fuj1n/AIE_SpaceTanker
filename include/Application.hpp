#include "IDrawable.hpp"
#include "AIE.h"
#include "GameObjects.hpp"
#include "bass.h"
#include "WorldData.hpp"
#include "ScoreTable.hpp"
#include <cctype>
#include <windows.h>
#include <vector>
#include <string>
#include <math.h>
#include <cmath>
#include <time.h>
#include <assert.h>
#include <crtdbg.h>
#include <iostream>
#include <cstring>
#include <sstream>

#pragma once

static const char* GAME_NAME = "Space Tanker";
static const char* VERSION = "v0.6";
static const bool gameRendersThroughPause = true;
static const float startCountdownMax = 5.f;
static bool debugMode = false;
static HANDLE processHandle;
static HWND windowHandle;

namespace {
	enum States {
		SPLASH,
		MAIN_MENU,
		TRACKPICK,
		TUTORIAL,
		LOADING,
		GAME,
		UPGRADE,
		PAUSE,
		GAME_OVER,
		HSCORES
	}; States currentState = SPLASH;
}

class PlayerUpgrades {
public:
	int availableCoins;

	//Upgrade values
	int speed, maxHealth, bulletSpeed, damageResistance, maxRange, fireRate, sprintDuration, sprintCooldownSpeed;
	int speedPrice, healthPrice, bspeedPrice, damResPrice, maxRangePrice, fireRatePrice, sprintDurPrice, sprintCdnPrice;

	int healthAdded;

	PlayerUpgrades() {
		availableCoins = 0;

		speed = maxHealth = bulletSpeed = damageResistance = maxRange = sprintDuration = sprintCooldownSpeed = fireRate = 1;

		healthAdded = 0;
		speedPrice = 15;
		healthPrice = 30;
		bspeedPrice = 20;
		damResPrice = 50;
		maxRangePrice = 20;
		fireRatePrice = 100;
		sprintDurPrice = 75;
		sprintCdnPrice = 80;
	}

	int calculateMaxHealth(int upgradeLevel = -1) {
		if(upgradeLevel == -1) {
			upgradeLevel = maxHealth;
		}

		return (int)Math::roundf(100 + (50 * (upgradeLevel - 1) * 0.5f));
	}

	long calculateDamageResistance(int upgradeLevel = -1) {
		if(upgradeLevel == -1) {
			upgradeLevel = damageResistance;
		}

		float calc = (upgradeLevel - 1) * 0.4f;
		calc = (float)((long)calc * 10) + Math::getBobber(calc);

		return (long)calc * 2;
	}
};

class Application {
private:
	int screenWidth, screenHeight;
	double tickLimit;
	bool isFullscreen;

	int fps, tps;

	int cameraX, cameraY;

	unsigned long long score;
	int scoreboard_selectedScore;
	bool earnedHighScore;

	char enteredName[4];
	int nameIndex;

	ScoreTable* scoreTable;

	ITrackable* aiTrackTarget;
	PlayerUpgrades* playerUpgrades;

	GameObjects* gameObjects;

	std::vector<IDrawable*> planets;
	std::vector<IDrawable*> drawables;

	bool altTrack;

	int update();
	void draw();
	void drawUpgradeStats(int index, float x, float y);
public:
	static Application* instance;

	Application(int screenWidth, int screenHeight, bool isFullscreen);
	~Application();

	int run();

	void addDrawable(IDrawable* drawable);
	void removeDrawable(IDrawable* drawable);
	void clearDrawables();
	void addPlanet(IDrawable* drawable);
	void removePlanet(IDrawable* drawable);
	void clearPlanets();
	void positionCamera(int x, int y);
	double getTickLimit();
	GameObjects* getGameObjects();
	int getScreenWidth();
	int getScreenHeight();
	ITrackable* getTrackTarget();

	void setState(int state);

	unsigned long long getScore();
	void setScore(unsigned long long score);

	void endGame();
	bool checkHighScore();

	//GUI variables(purposely public)
	int sprintCooldown;
	int maxSprintCooldown;
	int playerHealth;

private:
	int healthUpSpawn, healthUpFrequency, gameTicks;
	float proceduralDifficulty;

	int startCountdown;

	void init();
	void generatePlanets();
	void preloadGame();
	void initGame();
	int updateGame();
	void applyDifficulty();
};

