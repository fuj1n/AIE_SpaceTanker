#include "IDrawable.h"
#include "AIE.h"
#include "GameObjects.h"
#include "bass.h"
#include "WorldData.h"
#include "ScoreTable.h"
#include <cctype>
#include <windows.h>
#include <vector>
#include <string>
#include <cmath>
#include <time.h>
#include <assert.h>
#include <crtdbg.h>
#include <iostream>
#include <cstring>
#include <sstream>

#pragma once

static const char* GAME_NAME = "Space Tanker";
static const char* VERSION = "v0.4";
static const bool gameRendersThroughPause = true;
static const float startCountdownMax = 5.f;
static HANDLE processHandle;
//static HANDLE threadHandle;
static HWND windowHandle;

namespace{
	enum States{
		SPLASH,
		MAIN_MENU,
		TUTORIAL,
		LOADING,
		GAME,
		PAUSE,
		GAME_OVER,
		HSCORES
	}; States currentState = SPLASH;
}

class Application{
private:
	int screenWidth, screenHeight;
	double tickLimit;
	bool isFullscreen;

	int fps, tps;

	int cameraX, cameraY;

	unsigned long long score;

	ScoreTable* scoreTable;

	ITrackable* aiTrackTarget;

	GameObjects* gameObjects;

	std::vector<IDrawable*> planets;
	std::vector<IDrawable*> drawables;

	int update();
	void draw();
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

	//GUI variables(purposely public)
	int sprintCooldown;
	int maxSprintCooldown;
	int playerHealth;

private:
	int powerUpSpawn, healthUpSpawn, powerUpFrequency, healthUpFrequency, gameTicks;
	float proceduralDifficulty;
	
	int startCountdown;

	void init();
	void generatePlanets();
	void preloadGame();
	void initGame();
	int updateGame();
	void applyDifficulty();
};

