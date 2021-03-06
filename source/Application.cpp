#include "Application.hpp"

#include "resource1.h"
#include "Enemy.hpp"
#include "Planet.hpp"
#include "Player.hpp"
#include "Powerup.hpp"
#include "EnemySpawner.hpp"
#include "Utils.cpp"
#include <array>

Application* Application::instance;

Application::Application(int screenWidth, int screenHeight, bool isFullscreen) {
	instance = this;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->isFullscreen = isFullscreen;

	this->tickLimit = 60.0;

	this->fps = 0;
	this->tps = 0;

	earnedHighScore = false;

	eortt = 0;
	enemySpawnEase = 64;
	enemySpawnerIndex = 0;

	Input::fillDefaults();
}

Application::~Application() {
	delete &screenWidth;
	delete &screenHeight;
	delete &isFullscreen;

	clearDrawables();
	clearPlanets();
}

int Application::run() {
	srand(timeGetTime());

	Initialise(screenWidth, screenHeight, isFullscreen, (std::string("Space Tanker ") + std::string(VERSION)).c_str());

	//A little hackery to move the window
	int displayWidth, displayHeight;
	int wndWidth, wndHeight;
	WindowUtils::getScreenSize(displayWidth, displayHeight);

	windowHandle = GetActiveWindow();
	WindowUtils::getWindowSize(windowHandle, wndWidth, wndHeight);

	MoveWindow(windowHandle, displayWidth / 2 - wndWidth / 2, displayHeight / 2 - wndHeight / 2, wndWidth, wndHeight, true);

	//AIE Framework window icon GG, no clue why it uses the primary domain icons when I provide it with the HINSTANCE (it works properly for the wrapper, which does not use AIE Framework)
	HICON winICO = LoadIcon(applInstance, MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(windowHandle, WM_SETICON, ICON_SMALL, (LPARAM)winICO);
	SendMessage(windowHandle, WM_SETICON, ICON_BIG, (LPARAM)winICO);

	//BASS init stuffs
	BASS_Init(-1, 44100, 0, windowHandle, 0);

	SetBackgroundColour(SColour(0x000));

	init();

	int quitStatus = -1;

	long lastTime = timeGetTime();
	double msPerTick = 1000.0 / tickLimit;

	long lastTimer = timeGetTime();
	double delta = 0;

	int framerate = 0, tickrate = 0;

	HCURSOR gameCursor = LoadCursorFromFile("./images/GameCursor.cur");
	SPRITE sGameCursor = CreateSprite("./images/GameCursor.png", 32, 32, false);

	do {
		if(!isFullscreen) {
			SetCursor(gameCursor);
		}

		long now = timeGetTime();
		delta += (now - lastTime) / msPerTick;
		lastTime = now;
		
		// If this value is false by default, the framerate is capped to the tickrate
		boolean shouldRender = true;
		
		while(delta >= 1) {
			tickrate += 1;
			quitStatus = update();
			delta -= 1;
			shouldRender = true;
		}
		Sleep(1);
		if(shouldRender) {
			framerate++;
			draw();
		}

		if(timeGetTime() - lastTimer >= 1000) {
			std::cout << "FPS: " << framerate << " TPS: " << tickrate << std::endl;

			fps = framerate;
			tps = tickrate;

			lastTimer += 1000;
			framerate = 0;
			tickrate = 0;
		}

		if(isFullscreen) {
			int mousePosX, mousePosY;
			GetMouseLocation(mousePosX, mousePosY);

			if(mousePosX > screenWidth) {
				mousePosX = screenWidth;
			} else if(mousePosX < 0) {
				mousePosX = 0;
			}

			if(mousePosY > screenHeight) {
				mousePosY = screenHeight;
			} else if(mousePosY < 0) {
				mousePosY = 0;
			}

			MoveSprite(sGameCursor, (float)cameraX + mousePosX, (float)cameraY + mousePosY);
			DrawSprite(sGameCursor);
		}
	} while(quitStatus == -1 && FrameworkUpdate() == false);

	BASS_Free();
	Shutdown();

	return quitStatus == -1 ? 0 : quitStatus;
}

void Application::setState(int state) {
	currentState = (States)state;
}

unsigned long long Application::getScore() {
	return score;
}

void Application::setScore(unsigned long long score) {
	this->score = score;
}

int Application::update() {
	drawables.shrink_to_fit();
	planets.shrink_to_fit();
	return updateGame();
}

void Application::draw() {
	DrawIO::update();

	switch(currentState) {
	case SPLASH:
		positionCamera(0, 0);
		DrawSprite(gameObjects->splashTexture);
		break;
	case MAIN_MENU:
		positionCamera(0, 0);
		DrawSprite(gameObjects->menuButtons[0]);
		DrawSprite(gameObjects->menuButtons[1]);
		DrawSprite(gameObjects->menuButtons[2]);
		DrawSprite(gameObjects->logoTexture);
		break;
	case TRACKPICK:
		positionCamera(0, 0);
		DrawIO::fillRect(screenWidth / 2 - 350.f / 2, screenHeight / 2 - 60.f / 2 - 60 / 2 - 20, 350.f, 60.f, 0.f, SColour(0xFFFFFFFF));
		DrawIO::fillRect(screenWidth / 2 - 350.f / 2, screenHeight / 2 - 60.f / 2 + 20, 350.f, 60.f, 0.f, SColour(0xFFFFFFFF));
		DrawIO::drawString("Select ambient music track: ", screenWidth / 2 - (32.f * 27) / 4, 60.f, 32.f, 32.f, 0.f);
		DrawIO::drawString("Original 8-bit", screenWidth / 2 - 350.f / 2 + (350.f / 2 - 32.f * 15.f / 4), screenHeight / 2 - 60.f / 2 - 60 / 2 - 20 + (32.f / 4), 32.f, 32.f, 0.f);
		DrawIO::drawString("Nitro Fun - New Game", screenWidth / 2 - 350.f / 2 + (350.f / 2 - 32.f * 21.f / 4), screenHeight / 2 - 60.f / 2 + 20 + (32.f / 4), 32.f, 32.f, 0.f);
		break;
	case TUTORIAL:
		positionCamera(0, 0);
		DrawSprite(gameObjects->instructionTexture);
		DrawSprite(gameObjects->menuButtons[3]);
		break;
	case LOADING:
		positionCamera(0, 0);
		DrawString("Loading...", screenWidth / 2 - 30, screenHeight / 2 - 10);
		break;
	case GAME:{ case UPGRADE: case PAUSE:
		if(!planets.empty()) {
			for(unsigned int i = 0; i < planets.size(); i++) {
				IDrawable* d = planets.at(i);

				DrawSprite(d->getTexture());
			}
		}

		DrawSprite(gameObjects->stars);

		if(!drawables.empty()) {
			for(unsigned int i = 0; i < drawables.size(); i++) {
				IDrawable* d = drawables.at(i);

				DrawSprite(d->getTexture());
			}
		}

		if(maxSprintCooldown > 0 && sprintCooldown > 1) {
			float percent = (float)sprintCooldown / (float)maxSprintCooldown;
			DrawIO::drawLine(cameraX + 20.f, cameraY + screenHeight - 25.f, percent * (float)(screenWidth - 40), 20.f, 0.f, SColour(0xFFFF00FF));
		}

		int startCountdown = (int)Math::roundf(this->startCountdown / (float)tickLimit);
		if(currentState == GAME) {
			if(eortt > 0) {
				DrawIO::drawString("Enemy Wave", cameraX + (float)screenWidth - 190.f, cameraY + 10.f, 32.f, 32.f, 0.f);
			}

			if(startCountdown > 0) {
				char* cdownValue = new char[10];
				_itoa_s(startCountdown, cdownValue, 10, 10);
				DrawIO::drawString(cdownValue, (float)cameraX + screenWidth / 2 - 16, (float)cameraY + screenHeight / 2 - 16, 32.f, 32.f, 0.f, SColour(0x0000FFFF));
			}

			DrawIO::drawString(std::string("Health: ") + std::to_string(playerHealth), (float)cameraX + 26, (float)cameraY + screenHeight - 51, 32.f, 32.f, 0.f);
			MoveSprite(getGameObjects()->healthRemainSprite, (float)cameraX + 15, (float)cameraY + screenHeight - 43);
			DrawSprite(getGameObjects()->healthRemainSprite);

			DrawIO::drawString(std::string("Coins: ") + std::to_string(playerUpgrades->availableCoins), (float)cameraX + 226, (float)cameraY + screenHeight - 51, 32.f, 32.f, 0.f);
			MoveSprite(getGameObjects()->coinsOwnedSprite, (float)cameraX + 215, (float)cameraY + screenHeight - 43);
			DrawSprite(getGameObjects()->coinsOwnedSprite);

			DrawIO::drawString(std::string("Score: ") + std::to_string(score), (float)cameraX + 10, (float)cameraY + 10, 32.f, 32.f, 0.f);
		} else if(currentState == PAUSE) {
			DrawIO::fillRect(cameraX + 0.f, cameraY + 0.f, (float)screenWidth, (float)screenHeight, 0.f, SColour(0x00000088));
			DrawIO::drawString("Paused", (float)cameraX + screenWidth / 2.f - 96.f / 2.f, (float)cameraY + screenHeight / 2.f - 16.f, 32.f, 32.f, 0.f);
		} else if(currentState == UPGRADE) {
			//The framerate on this screen will be dramatically lower because I am using cheaty functions because of the lack of them in the framework
			DrawIO::fillRect(cameraX + 0.f, cameraY + 0.f, (float)screenWidth, (float)screenHeight, 0.f, SColour(0x00000088));

			DrawIO::drawString(std::string("Coins: ") + std::to_string(playerUpgrades->availableCoins), (float)cameraX + 26, (float)cameraY + screenHeight - 51, 32.f, 32.f, 0.f);
			MoveSprite(getGameObjects()->coinsOwnedSprite, (float)cameraX + 15, (float)cameraY + screenHeight - 43);
			DrawSprite(getGameObjects()->coinsOwnedSprite);

			//Optimisation: using a prefab with pre-built text

			MoveSprite(gameObjects->guiTankerCustomisationPrefabSprite, cameraX + screenWidth / 2 - 480.f / 2, cameraY + screenHeight / 2 - 360.f / 2);
			DrawSprite(gameObjects->guiTankerCustomisationPrefabSprite);
			//DrawIO::drawString("Tanker Customisation", cameraX + screenWidth / 2 - 8.f * 20, cameraY + screenHeight / 2 - 16.f - (360 / 3 + 30), 32.f, 32.f, 0.f);

			int upgradeIndex = 0;
			float plY = cameraY + screenHeight / 2 - 16.f - (360 / 3 + 30) + 35;
			for(int i = 1; i <= 4; i++) {
				float plX = cameraX + screenWidth / 2 - 480.f / 2 + 16;
				for(int j = 1; j <= 2; j++) {
					drawUpgradeStats(upgradeIndex, plX, plY);
					plX += 216 + 15;
					upgradeIndex++;
				}
				plY += 64 + 10;
			}
		}

		break;
	}
	case GAME_OVER: {
						positionCamera(0, 0);
						DrawIO::drawString("Game Over", (float)screenWidth / 2 - (9 * 8), 10.f, 32.f, 32.f, 0.f);
						std::string playerScore = std::string("Score: ") + std::to_string(score);

						DrawIO::drawString(playerScore, (float)screenWidth / 2 - (playerScore.length() * 8), (float)screenHeight / 2 - 70.f, 32.f, 32.f, 0.f);
						if(earnedHighScore) {
							DrawIO::drawString("New Highscore!", (float)screenWidth / 2 - (14 * 8), (float)screenHeight / 2 - 30.f, 32.f, 32.f, 0.f);
							DrawIO::drawString("Enter name below(3 characters max)", (float)screenWidth / 2 - (34 * 4), (float)screenHeight / 2, 16.f, 16.f, 0.f);
							DrawIO::drawLine((float)screenWidth / 2 - 30, (float)screenHeight / 2 + 40.f, 16.f, 1.f, 0.f);
							DrawIO::drawLine((float)screenWidth / 2 - 10, (float)screenHeight / 2 + 40.f, 16.f, 1.f, 0.f);
							DrawIO::drawLine((float)screenWidth / 2 + 10, (float)screenHeight / 2 + 40.f, 16.f, 1.f, 0.f);
							DrawIO::drawString(enteredName, (float)screenWidth / 2 - 36, (float)screenHeight / 2 + 13.f, 32.f, 32.f, 3.f);
						}

						DrawIO::fillRect((float)screenWidth / 2 - 200.f / 2, (float)screenHeight / 2 + 60.f, 200.f, 40.f, 0.f);
						DrawIO::drawString("OK", screenWidth / 2 - 200.f / 2 + (200.f / 2 - 32.f * 3.f / 4), (float)screenHeight / 2 + 55.f + (32.f / 4), 32.f, 32.f, 0.f);

						break;
	}
	case HSCORES:
		DrawIO::drawString("High Scores", (float)screenWidth / 2 - (32 * 11) / 4, 10.f, 32.f, 32.f, 0.f);
		int dY = 70;
		for(unsigned int i = 0; i < 10; i++) {
			SColour sc = (signed int)i == scoreboard_selectedScore ? SColour(0x00FF00FF) : SColour(0xFFFFFFFF);
			std::string score = scoreTable->scoreMap->getValues()->at(i);
			int dots = 17 - score.length();
			char* c_dots = new char[dots + 1];
			for(int j = 0; j < dots; j++) {
				c_dots[j] = '.';
			}
			c_dots[dots] = '\0';
			DrawIO::drawString(std::to_string(i + 1) + std::string(". ") + (i < 9 ? std::string(" ") : std::string()) + String::replaceChar(std::string(scoreTable->scoreMap->getKeys()->at(i)), ' ', '.') + std::string(c_dots) + score, 62.f, (float)dY, 32.f, 32.f, 5.f, sc);
			dY += 33;
			delete[] c_dots;
		}

		DrawIO::fillRect((float)screenWidth / 2 - 200.f / 2, (float)screenHeight - 50.f, 98.f, 40.f, 0.f);
		DrawIO::drawString("OK", screenWidth / 2 - 100.f / 2 + (100.f / 2 - 32.f * 3.f / 4) - 52.f, (float)screenHeight - 55.f + (32.f / 4), 32.f, 32.f, 0.f);

		DrawIO::fillRect((float)screenWidth / 2 - 200.f / 2 + 102.f, (float)screenHeight - 50.f, 98.f, 40.f, 0.f);
		DrawIO::drawString("Reset", screenWidth / 2 - 100.f / 2 + (100.f / 2 - 80.f * 3.f / 4) + 62.f, (float)screenHeight - 55.f + (32.f / 4), 32.f, 32.f, 0.f);
		break;
	}

	if(debugMode) {
		DrawString(std::string(std::string("FPS: ") + std::to_string(fps) + std::string(" TPS: ") + std::to_string(tps)).c_str(), cameraX + 10, cameraY + 10);
	}

	MoveCamera((float)cameraX, (float)cameraY);

	ClearScreen();
}

void Application::drawUpgradeStats(int index, float x, float y) {
	std::string upgradeName;
	int upgradeLevel;
	SPRITE upgradeIcon;
	int upgradePrice;

	switch(index) {
	case 0:
		upgradeName = "Tanker Speed";
		upgradeLevel = playerUpgrades->speed;
		upgradeIcon = getGameObjects()->playerUpgrades.speed;
		upgradePrice = playerUpgrades->speedPrice;
		break;
	case 1:
		upgradeName = "Max Health";
		upgradeLevel = playerUpgrades->maxHealth;
		upgradeIcon = getGameObjects()->playerUpgrades.maxHealth;
		upgradePrice = playerUpgrades->healthPrice;
		break;
	case 2:
		upgradeName = "Bullet Speed";
		upgradeLevel = playerUpgrades->bulletSpeed;
		upgradeIcon = getGameObjects()->playerUpgrades.bulletSpeed;
		upgradePrice = playerUpgrades->bspeedPrice;
		break;
	case 3:
		upgradeName = "Damage Resistance";
		upgradeLevel = playerUpgrades->damageResistance;
		upgradeIcon = getGameObjects()->playerUpgrades.damageResistance;
		upgradePrice = playerUpgrades->damResPrice;
		break;
	case 4:
		upgradeName = "Fire Range";
		upgradeLevel = playerUpgrades->maxRange;
		upgradeIcon = getGameObjects()->playerUpgrades.maxRange;
		upgradePrice = playerUpgrades->maxRangePrice;
		break;
	case 5:
		upgradeName = "Fire Rate";
		upgradeLevel = playerUpgrades->fireRate;
		upgradeIcon = getGameObjects()->playerUpgrades.fireRate;
		upgradePrice = playerUpgrades->fireRatePrice;
		break;
	case 6:
		upgradeName = "Sprint Duration";
		upgradeLevel = playerUpgrades->sprintDuration;
		upgradeIcon = getGameObjects()->playerUpgrades.sprintDuration;
		upgradePrice = playerUpgrades->sprintDurPrice;
		break;
	case 7:
		upgradeName = "Sprint Cooldown Speed";
		upgradeLevel = playerUpgrades->sprintCooldownSpeed;
		upgradeIcon = getGameObjects()->playerUpgrades.sprintCooldownSpeed;
		upgradePrice = playerUpgrades->sprintCdnPrice;
		break;
	default:
		upgradeName = "Error";
		upgradeLevel = 5;
		upgradeIcon = getGameObjects()->playerUpgrades.fireRate;
		upgradePrice = 42;
		break;
	}

	SColour col = upgradeLevel == 5 ? SColour(0x00FF00FF) : SColour(0xFFFFFFFF);

	upgradePrice *= upgradeLevel;

	if(col.colour != SColour(0xFFFFFFFF).colour) {
		DrawIO::drawRect(x, y, 216.f, 64.f, 0.f, 1.f, col);
		DrawIO::drawString(upgradeName, x + 2, y + 2, 16.f, 16.f, 0.f, col);
		MoveSprite(upgradeIcon, x + 216.f - 18.f, y + 2);
		SetSpriteColour(upgradeIcon, col);
		DrawSprite(upgradeIcon);
	}

	if(upgradeLevel != 5) {
		DrawIO::drawString(upgradeLevel == 5 ? std::string("Fully upgraded.") : std::string("Price: ") + std::to_string(upgradePrice), x + 2, y + 20, 16.f, 16.f, 0.f, upgradeLevel == 5 ? col : playerUpgrades->availableCoins >= upgradePrice && upgradePrice != -1 ? col : SColour(0x0000FFFF));
	} else {
		MoveSprite(getGameObjects()->guiFullyUpgradedPrefabSprite, x + 4, y + 21);
		DrawSprite(getGameObjects()->guiFullyUpgradedPrefabSprite);
	}

	float spX = x + 8;
	for(int i = 1; i <= 5; i++) {
		if(upgradeLevel >= i) {
			DrawIO::fillRect(spX, y + 38.f, 16.f, 16.f, 0.f, col);
		} else {
			//DrawIO::drawRect(spX, y + 38.f, 16.f, 16.f, 0.f, 1.f, col);
		}
		spX += 18.f;
	}
}

void Application::positionCamera(int x, int y) {
	cameraX = x;
	cameraY = y;
}

double Application::getTickLimit() {
	return tickLimit;
}

GameObjects* Application::getGameObjects() {
	return gameObjects;
}

int Application::getScreenWidth() {
	return screenWidth;
}

int Application::getScreenHeight() {
	return screenHeight;
}

ITrackable* Application::getTrackTarget() {
	return aiTrackTarget;
}

void Application::addDrawable(IDrawable* drawable) {
	drawables.push_back(drawable);
}

void Application::addDrawable_(IDrawable* drawable) {
	drawables.insert(drawables.begin(), drawable);
}

void Application::removeDrawable(IDrawable* drawable) {
	drawable->destroySprites();
	for(unsigned int i = 0; i < drawables.size(); i++) {
		if(drawables.at(i) == drawable) {
			drawables.erase(drawables.begin() + i);
			drawables.shrink_to_fit();
			i -= 1;
		}
	}
	delete drawable;
}

void Application::clearDrawables() {
	for(unsigned int i = 0; i < drawables.size(); i++) {
		drawables.shrink_to_fit();
		drawables.at(i)->destroySprites();
		drawables.erase(drawables.begin() + i);
		i -= 1;
	}
	drawables.clear();
}

void Application::addPlanet(IDrawable* drawable) {
	planets.push_back(drawable);
}

void Application::removePlanet(IDrawable* drawable) {
	drawable->destroySprites();
	for(unsigned int i = 0; i < planets.size(); i++) {
		if(planets.at(i) == drawable) {
			planets.erase(planets.begin() + i);
			planets.shrink_to_fit();
			i -= 1;
		}
	}
	delete drawable;
}

void Application::clearPlanets() {
	for(unsigned int i = 0; i < planets.size(); i++) {
		planets.shrink_to_fit();
		planets.at(i)->destroySprites();
		planets.erase(planets.begin() + i);
		i -= 1;
	}
	planets.clear();
}

void Application::endGame() {
	BASS_ChannelStop(gameObjects->backgroundLoop);
	BASS_ChannelPlay(gameObjects->menuSound, true);
	clearDrawables();
	clearPlanets();

	enteredName[0] = enteredName[1] = enteredName[2] = ' ';
	enteredName[3] = '\0';
	nameIndex = 0;

	currentState = GAME_OVER;
	scoreboard_selectedScore = -1;

	earnedHighScore = checkHighScore();
}

bool Application::checkHighScore() {
	if(scoreTable->isHighScore(score)) {
		return true;
	} else {
		return false;
	}
}

void Application::init() {
	this->score = 0;
	this->startCountdown = (int)(startCountdownMax * tickLimit);
	this->scoreTable = new ScoreTable();
	scoreTable->load();

	gameObjects = new GameObjects();

	//Call all the sprite initialisation here
	getGameObjects()->splashTexture = CreateSprite("./images/splash.png", screenWidth, screenHeight, false);
	getGameObjects()->logoTexture = CreateSprite("./images/logo.png", 500, 64, true);
	MoveSprite(getGameObjects()->logoTexture, (float)screenWidth / 2, (float)screenHeight / 2 - (float)screenHeight / 4 - 50);
	getGameObjects()->instructionTexture = CreateSprite("./images/instructions.png", screenWidth, screenHeight, false);
	getGameObjects()->menuButtons[0] = CreateSprite("./images/buttons/play.png", 200, 60, true);
	getGameObjects()->menuButtons[1] = CreateSprite("./images/buttons/hscores.png", 200, 60, true);
	getGameObjects()->menuButtons[2] = CreateSprite("./images/buttons/exit.png", 200, 60, true);
	getGameObjects()->menuButtons[3] = CreateSprite("./images/buttons/ok.png", 200, 60, true);
	MoveSprite(getGameObjects()->menuButtons[0], (float)screenWidth / 2, (float)screenHeight / 2 - 65);
	MoveSprite(getGameObjects()->menuButtons[1], (float)screenWidth / 2, (float)screenHeight / 2);
	MoveSprite(getGameObjects()->menuButtons[2], (float)screenWidth / 2, (float)screenHeight / 2 + 65);
	MoveSprite(getGameObjects()->menuButtons[3], (float)screenWidth / 2, (float)screenHeight / 2 + 8);
	getGameObjects()->guiFrameSprite = CreateSprite("./images/guiFrame.png", 480, 360, false);
	getGameObjects()->guiTankerCustomisationPrefabSprite = CreateSprite("./images/prefabs/tankerCustomisation.png", 480, 360, false);
	getGameObjects()->guiFullyUpgradedPrefabSprite = CreateSprite("./images/prefabs/strFullyUpgraded.png", 120, 13, false);
	getGameObjects()->stars = CreateSprite("./images/stars.png", WORLD_WIDTH, WORLD_HEIGHT, false);

	//Call all the sound initialisation here
	getGameObjects()->menuSound = BASS_StreamCreateFile(false, "./sounds/menu/menu_music1.ogg", 0, 0, BASS_SAMPLE_LOOP);
	getGameObjects()->laserFireSound = BASS_StreamCreateFile(false, "./sounds/fire_laser.wav", 0, 0, 0);
	getGameObjects()->speedUpSound = BASS_StreamCreateFile(false, "./sounds/speedup.wav", 0, 0, 0);
	BASS_ChannelSetAttribute(getGameObjects()->speedUpSound, BASS_ATTRIB_VOL, 0.50F);
	getGameObjects()->healthUpSound = BASS_StreamCreateFile(false, "./sounds/pickup.wav", 0, 0, 0);
	getGameObjects()->explosionSound = BASS_StreamCreateFile(false, "./sounds/explode.wav", 0, 0, 0);
	getGameObjects()->coinPickupSound = BASS_StreamCreateFile(false, "./sounds/coin.wav", 0, 0, 0);
}

void Application::generatePlanets() {
	const int planetsToGenerate = 13;
	bool usedPlanets[13];
	for(int i = 0; i < 13; i++) {
		usedPlanets[i] = false;
	}

	for(int i = 0; i < planetsToGenerate; i++) {
		while(true) {
			int texture = Random::random(0, 12);

			if(!usedPlanets[texture]) {
				usedPlanets[texture] = true;

				int planetX = Random::random(64, WORLD_WIDTH - 64);
				int planetY = Random::random(64, WORLD_HEIGHT - 64);
				int planetBounds = Random::random(16, 64);

				std::string textureStr = std::to_string(texture);

				addPlanet(new Planet((std::string("./images/planets/") + textureStr + std::string(".png")).c_str(), planetX, planetY, planetBounds));

				break;
			}
		}
	}
}

void Application::preloadGame() {
	//Create all the desired game sprites here
	getGameObjects()->laserBeamSprite = CreateSprite("./images/beam.png", 10, 4, true);
	getGameObjects()->coinsSprite = CreateSprite("./images/coins.png", 128, 128, false);
	getGameObjects()->healthPowerUpSprite = CreateSprite("./images/healthkit.png", 128, 128, false);
	getGameObjects()->playerSprite = CreateSprite("./images/tanker.png", 64, 64, true);
	getGameObjects()->enemySprite = CreateSprite("./images/enemy.png", 64, 64, true);
	getGameObjects()->enemySpawnerSprite = CreateSprite("./images/enemyspawner.png", 64, 64, false);
	getGameObjects()->healthRemainSprite = DuplicateSprite(getGameObjects()->healthPowerUpSprite);
	getGameObjects()->coinsOwnedSprite = DuplicateSprite(getGameObjects()->coinsSprite);

	if(!altTrack) {
		getGameObjects()->backgroundLoop = BASS_StreamCreateFile(false, "./sounds/ambience/ambient1.wav", 0, 0, BASS_SAMPLE_LOOP);
	} else {
		getGameObjects()->backgroundLoop = BASS_StreamCreateFile(false, "./sounds/ambience/ambient2.mp3", 0, 0, BASS_SAMPLE_LOOP);
	}

	float w = 16.f, h = 16.f;
	SetSpriteScale(getGameObjects()->healthRemainSprite, w, h);
	SetSpriteScale(getGameObjects()->coinsOwnedSprite, w, h);

	for(int i = 0; i < numExplosions; i++) {
		getGameObjects()->explosionSprites[i] = CreateSprite((std::string("./images/explosion/") + std::to_string(i) + std::string(".png")).c_str(), 128, 128, true);
	}

	getGameObjects()->playerUpgrades.declareSprites();

	initGame();
	currentState = GAME;
}

void Application::initGame() {
	healthUpFrequency = (int)(50 * tickLimit);
	healthUpSpawn = healthUpFrequency / 4;
	proceduralDifficulty = 1.F;
	gameTicks = 0;

	//Call all the IDrawable initialisation here
	Player* pl = new Player(getGameObjects()->playerSprite);
	aiTrackTarget = pl;
	playerUpgrades = pl->getUpgrades();
	addDrawable(pl);

	score = 0;
	playerUpgrades->availableCoins = 0;

	generatePlanets();

	BASS_ChannelSetAttribute(getGameObjects()->backgroundLoop, BASS_ATTRIB_VOL, 0.15F);
	BASS_ChannelPlay(getGameObjects()->backgroundLoop, false);
}

int Application::updateGame() {
	bool mouseDown = GetMouseButtonDown(0);
	static bool mouseDownLast;
	static int quitTickDown;
	if(IsKeyDown(KEY_ESC)) {
		if(quitTickDown >= (2 * 60)) {
			return 0;
		} else {
			quitTickDown++;
		}
	} else {
		quitTickDown = 0;
	}

	static int key_cTickDown;
	if(IsKeyDown('C')) {
		key_cTickDown++;
	} else {
		key_cTickDown = 0;
	}

	static int key_debugTickDown;
	if(IsKeyDown(KEY_F3)) {
		if(key_debugTickDown == 1) {
			debugMode = !debugMode;
		}

		key_debugTickDown++;
	} else {
		key_debugTickDown = 0;
	}

	int mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	switch(currentState) {
	case SPLASH:{
					BASS_ChannelPlay(getGameObjects()->menuSound, false);
					static unsigned int ticks;

					if(ticks >= 240) {
						currentState = MAIN_MENU;
					} else {
						ticks++;
					}

					break;
	}
	case MAIN_MENU:
		if(mouseDown && !mouseDownLast) {
			if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 - 65 && mouseY < screenHeight / 2 + 30 - 65) {
				currentState = TRACKPICK;
			} else if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 && mouseY < screenHeight / 2 + 30) {
				currentState = HSCORES;
			} else if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 + 65 && mouseY < screenHeight / 2 + 30 + 65) {
				return 0;
			}
		}

		break;
	case TRACKPICK:
		if(mouseDown && !mouseDownLast) {
			if(mouseX > screenWidth / 2 - 350.f / 2 && mouseX < screenWidth / 2 - 350.f / 2 + 350.f && mouseY > screenHeight / 2 - 60.f / 2 - 60 / 2 - 20 && mouseY < screenHeight / 2 - 60.f / 2 - 60 / 2 - 20 + 60.f) {
				altTrack = false;
				currentState = TUTORIAL;
			} else if(mouseX > screenWidth / 2 - 350.f / 2 && mouseX < screenWidth / 2 - 350.f / 2 + 350.f && mouseY >screenHeight / 2 - 60.f / 2 + 20 && mouseY <screenHeight / 2 - 60.f / 2 + 20 + 60.f) {
				altTrack = true;
				currentState = TUTORIAL;
			}
		}
		break;
	case TUTORIAL:
		if(mouseDown && !mouseDownLast) {
			if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 + 8 && mouseY < screenHeight / 2 + 30 + 8) {
				BASS_ChannelStop(getGameObjects()->menuSound);
				currentState = LOADING;
			}
		}
		break;
	case LOADING:
		preloadGame();
	case GAME:{
				  if(startCountdown > 0) {
					  startCountdown--;
				  }
				  int enemyCount = 0;
				  applyDifficulty();

				  if(healthUpSpawn <= 0) {
					  healthUpSpawn = healthUpFrequency;

					  int healthUpX = Random::random(64, WORLD_WIDTH - 64);
					  int healthUpY = Random::random(64, WORLD_HEIGHT - 64);
					  int healthUpDespawn = (int)(Random::random(100, 450) * tickLimit);

					  addDrawable(new Powerup("health", getGameObjects()->healthPowerUpSprite, healthUpX, healthUpY, healthUpDespawn));
				  } else {
					  if(Random::random(0, 2) != 0) {
						  healthUpSpawn--;
					  }
				  }

				  if(eortt > 0) {
					  eortt--;
				  }

				  drawables.shrink_to_fit();
				  if(!drawables.empty()) {
					  for(unsigned int i = 0; i < drawables.size(); i++) {
						  IDrawable* d = drawables.at(i);
						  ICollidable* col0 = dynamic_cast<ICollidable*>(d);

						  if(col0 != 0 && col0->isCollideTester()) {
							  for(unsigned int i1 = 0; i1 < drawables.size(); i1++) {
								  ICollidable* col1 = dynamic_cast<ICollidable*>(drawables.at(i1));
								  if(col1 != 0) {
									  if(col0 != col1 && (Collision::rect_intersects(col0->getCX(), col0->getCY(), col0->getWidth(), col0->getHeight(), col1->getCX(), col1->getCY(), col1->getWidth(), col1->getHeight()))) {
										  col0->onCollide(col1);
									  }
								  }
							  }
						  }

						  //Count how many enemies there are using the main update loop(more efficient than having a separate loop for that)
						  if(dynamic_cast<Enemy*>(d) != 0) {
							  enemyCount++;
						  }

						  d->update();
					  }
				  }

				  numEnemies = enemyCount;
				  if(startCountdown == 0) {
					  startCountdown--;

					  addDrawable_(new EnemySpawner(0, 0, 500, 5, 250, false, getGameObjects()->enemySpawnerSprite));
					  addDrawable_(new EnemySpawner(960 - 64, 960 - 64, 500, -250, -5, false, getGameObjects()->enemySpawnerSprite));
					  enemySpawnerIndex = 1;
				  }

				  if(quitTickDown == 1) {
					  currentState = PAUSE;
					  BASS_Pause();
				  } else if(key_cTickDown == 1) {
					  currentState = UPGRADE;
					  BASS_Pause();
				  }

				  gameTicks++;

				  break;
	}
	case UPGRADE:
		if(key_cTickDown == 1 || quitTickDown == 1) {
			currentState = GAME;
			BASS_Start();
		}

		if(true) {
			int upgradeIndex = 0;
			float plY = screenHeight / 2 - 16.f - (360 / 3 + 30) + 35;
			for(int i = 1; i <= 4; i++) {
				float plX = screenWidth / 2 - 480.f / 2 + 16;
				for(int j = 1; j <= 2; j++) {
					if(mouseDown && !mouseDownLast) {
						if(mouseX > plX && mouseX < plX + 216 && mouseY > plY && mouseY < plY + 64) {
							int* upgradeLevel = nullptr;
							int upgradePrice;

							switch(upgradeIndex) {
							case 0:
								upgradeLevel = &playerUpgrades->speed;
								upgradePrice = playerUpgrades->speedPrice;
								break;
							case 1:
								upgradeLevel = &playerUpgrades->maxHealth;
								upgradePrice = playerUpgrades->healthPrice;
								break;
							case 2:
								upgradeLevel = &playerUpgrades->bulletSpeed;
								upgradePrice = playerUpgrades->bspeedPrice;
								break;
							case 3:
								upgradeLevel = &playerUpgrades->damageResistance;
								upgradePrice = playerUpgrades->damResPrice;
								break;
							case 4:
								upgradeLevel = &playerUpgrades->maxRange;
								upgradePrice = playerUpgrades->maxRangePrice;
								break;
							case 5:
								upgradeLevel = &playerUpgrades->fireRate;
								upgradePrice = playerUpgrades->fireRatePrice;
								break;
							case 6:
								upgradeLevel = &playerUpgrades->sprintDuration;
								upgradePrice = playerUpgrades->sprintDurPrice;
								break;
							case 7:
								upgradeLevel = &playerUpgrades->sprintCooldownSpeed;
								upgradePrice = playerUpgrades->sprintCdnPrice;
								break;
							default:
								upgradeLevel = new int(5);
								upgradePrice = 42;
								break;
							}
							if(upgradeLevel != nullptr && *upgradeLevel < 5) {
								upgradePrice *= *upgradeLevel;
								if(upgradePrice <= playerUpgrades->availableCoins) {
									*upgradeLevel = *upgradeLevel + 1;
									playerUpgrades->availableCoins -= upgradePrice;

									if(upgradeLevel == &playerUpgrades->maxHealth) {
										playerUpgrades->healthAdded = playerUpgrades->calculateMaxHealth() - playerUpgrades->calculateMaxHealth(*upgradeLevel - 1);
									}
								}
							}
						}
					}
					plX += 216 + 15;
					upgradeIndex++;
				}
				plY += 64 + 10;
			}
		}

		break;
	case PAUSE:
		if(quitTickDown == 1) {
			currentState = GAME;
			BASS_Start();
		}

		break;
	case GAME_OVER:
		if(earnedHighScore) {
			int key = Input::getNewPressedKey(Input::typekit, '\0');
			DrawIO::fillRect((float)screenWidth / 2 - 200.f / 2, (float)screenHeight / 2 + 60.f, 200.f, 40.f, 0.f);
			if(mouseDown && !mouseDownLast) {
				if(mouseX > screenWidth / 2 - 200.f / 2 && mouseY > screenHeight / 2 + 60.f && mouseX < screenWidth / 2 - 200.f / 2 + 200.f && mouseY < screenHeight / 2 + 60.f + 40.f) {
					if(nameIndex > 0 && nameIndex <= 3) {
						scoreTable->putScore(enteredName, score);
						scoreTable->save();
						scoreboard_selectedScore = scoreTable->findScore(std::string(enteredName));
						currentState = HSCORES;
					}
				}
			}

			if(key >= 'A' && key <= 'Z' && nameIndex < 3) {
				enteredName[nameIndex] = (char)key;
				nameIndex++;
			} else if(key == KEY_BACKSPACE && nameIndex > 0) {
				enteredName[nameIndex - 1] = ' ';
				nameIndex--;
			}
		} else {
			if(mouseDown && !mouseDownLast) {
				if(mouseX > screenWidth / 2 - 200.f / 2 && mouseY > screenHeight / 2 + 60.f && mouseX < screenWidth / 2 - 200.f / 2 + 200.f && mouseY < screenHeight / 2 + 60.f + 40.f) {
					scoreboard_selectedScore = -1;
					currentState = HSCORES;
				}
			}
		}
		break;
	case HSCORES:
		if(mouseDown && !mouseDownLast) {
			if(mouseX > screenWidth / 2 - 200.f / 2 && mouseY > screenHeight - 50.f && mouseX < screenWidth / 2 - 200.f / 2 + 98.f && mouseY < screenHeight - 50.f + 40.f) {
				scoreboard_selectedScore = -1;
				earnedHighScore = false;
				currentState = MAIN_MENU;
			} else if(mouseX > screenWidth / 2 - 200.f / 2 + 102.f && mouseY > screenHeight - 50.f && mouseX < screenWidth / 2 - 200.f / 2 + 102.f + 98.f && mouseY < screenHeight - 50.f + 40.f) {
				scoreboard_selectedScore = -1;
				earnedHighScore = false;
				scoreTable->defaults();
				scoreTable->save();
			}
		}
		break;
	}

	mouseDownLast = mouseDown;

	return -1;
}

void Application::applyDifficulty() {
	if(((int)gameTicks % (int)(tickLimit) / 2) == 0) {
		if(enemySpawnEase <= 0) {
			enemySpawnEase = 32;
		}
	}
	if(((int)gameTicks % (int)(tickLimit * 2)) == 0) {
		proceduralDifficulty += 0.1F;

		healthUpFrequency += (int)(proceduralDifficulty / 2);

		enemySpawnEase--;
		if(enemySpawnEase == 10) {
			enemySpawnEase = 1;
			eortt = (int)tickLimit;

			switch(enemySpawnerIndex) {
			case 1:
				addDrawable_(new EnemySpawner(960 - 64, 0, 500, 5, 250, true, getGameObjects()->enemySpawnerSprite));
				break;
			case 2:
				addDrawable_(new EnemySpawner(0, 960 - 64, 500, -250, -5, true, getGameObjects()->enemySpawnerSprite));
				break;
			case 3:
				addDrawable_(new EnemySpawner(0, 0, 500, 5, 250, false, getGameObjects()->enemySpawnerSprite));
				break;
			case 4:
				addDrawable_(new EnemySpawner(960 - 64, 960 - 64, 500, -250, -5, false, getGameObjects()->enemySpawnerSprite));
				break;
			case 5:
				enemySpawnerIndex = 1;
			}
			enemySpawnerIndex++;
		}
	}
}

int Application::getEnemyCount() {
	return numEnemies;
}

int Application::getSpawnEase() {
	return enemySpawnEase;
}
