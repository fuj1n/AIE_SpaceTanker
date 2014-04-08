#include "Application.h"

#include "Enemy.h"
#include "Planet.h"
#include "Player.h"
#include "Powerup.h"
#include "Utils.cpp"

Application* Application::instance;

Application::Application(int screenWidth, int screenHeight, bool isFullscreen){
	instance = this;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->isFullscreen = isFullscreen;

	this->tickLimit = 60.0;

	this->fps = 0;
	this->tps = 0;
}

Application::~Application(){
	delete &screenWidth;
	delete &screenHeight;
	delete &isFullscreen;

	clearDrawables();
	clearPlanets();
}

int Application::run(){
	srand(timeGetTime());

	Initialise(screenWidth, screenHeight, isFullscreen, (std::string("Space Tanker ") + std::string(VERSION)).c_str());

	//BASS init stuffs
	BASS_Init(-1, 44100, 0, 0, 0); 

	SetBackgroundColour(SColour(0x000));

	init();

	int quitStatus = -1;

	long lastTime = timeGetTime();
	double msPerTick = 1000.0 / tickLimit;

	long lastTimer = timeGetTime();
	double delta = 0;

	int framerate = 0, tickrate = 0;

    do 
    {
		long now = timeGetTime();
		delta += (now - lastTime) / msPerTick;
		lastTime = now;
		boolean shouldRender = true;
		while(delta >= 1){
			tickrate += 1;
			quitStatus = update();
			delta -= 1;
			shouldRender = true;
		}
		Sleep(2);
		if(shouldRender){
			framerate++;
			draw();
		}

		if(timeGetTime() - lastTimer >= 1000){
			std::cout << "FPS: " << framerate << " TPS: " << tickrate << std::endl;

			fps = framerate;
			tps = tickrate;

			lastTimer += 1000;
			framerate = 0;
			tickrate = 0;
		}
    } while ( quitStatus == -1 && FrameworkUpdate() == false );

	BASS_Free();

	Shutdown();

    return quitStatus == -1 ? 0 : quitStatus;
}

void Application::setState(int state){
	currentState = (States)state;
}

int Application::update(){
	drawables.shrink_to_fit();
	planets.shrink_to_fit();
	return updateGame();
}

void Application::draw(){
	DrawIO::update();

	switch(currentState){
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
	case TUTORIAL:
		positionCamera(0, 0);
		DrawSprite(gameObjects->instructionTexture);
		DrawSprite(gameObjects->menuButtons[3]);
		break;
	case LOADING:
		positionCamera(0, 0);
		DrawString("Loading...", screenWidth / 2 - 30, screenHeight / 2 - 10);
		break;
	case GAME:{ case PAUSE:
		if(!planets.empty()){
			for(unsigned int i = 0; i < planets.size(); i++){
				IDrawable* d = planets.at(i);

				DrawSprite(d->getTexture());
			}
		}

		DrawSprite(gameObjects->stars);

		if(!drawables.empty()){
			for(unsigned int i = 0; i < drawables.size(); i++){
				IDrawable* d = drawables.at(i);

				DrawSprite(d->getTexture());
			}
		}

		if(maxSprintCooldown > 0 && sprintCooldown > 0){
			float percent = (float)sprintCooldown / (float)maxSprintCooldown;
			//I don't trust AIE DrawLine, seems semi-broken
			DrawIO::drawLine(cameraX + 20.f, cameraY + screenHeight - 25.f, percent * (float)(screenWidth - 40), 20.f, 0.f, SColour(0xFFFF00FF));
		}

		int startCountdown = (int)roundf(this->startCountdown / (float)tickLimit);

		if(startCountdown > 0){
			char* cdownValue = new char[10];
			_itoa_s(startCountdown, cdownValue, 10, 10);
			//AIE DrawString coloring broken and not sizeable
			DrawIO::drawString(cdownValue, (float)cameraX + screenWidth / 2 - 16, (float)cameraY + screenHeight / 2 - 16, 32.f, 32.f, 0.f, SColour(0x0000FFFF));
		}

		DrawIO::drawString(std::string("Health: ") + std::to_string(playerHealth), (float)cameraX + 26, (float)cameraY + screenHeight - 51, 32.f, 32.f, 0.f);
		MoveSprite(getGameObjects()->healthRemainSprite, (float)cameraX + 15, (float)cameraY + screenHeight - 43);
		DrawSprite(getGameObjects()->healthRemainSprite);

		if(currentState == PAUSE){
			DrawIO::drawString("Paused", (float)cameraX + screenWidth / 2.f - 96.f / 2.f, (float)cameraY + screenHeight / 2.f - 16.f, 32.f, 32.f, 0.f);
		}

		break;
	}
	case GAME_OVER:
		positionCamera(0, 0);
		DrawIO::drawString("Game Over", (float)screenWidth / 2 - (9 * 8), (float)screenHeight / 2 - 16, 32.f, 32.f, 0.f);
		break;
	}

	//Since this is temporary anyway, no need to decrease my performance further by using a DrawIO function
	//DrawIO::drawString(std::string("FPS: " + std::to_string(fps) + std::string(" TPS: ") + std::to_string(tps)), cameraX + 10, cameraY + 10, 32, 32, 0);
	DrawString(std::string(std::string("FPS: ") + std::to_string(fps) + std::string(" TPS: ") + std::to_string(tps)).c_str(), cameraX + 10, cameraY + 10); 

	MoveCamera((float)cameraX, (float)cameraY);

	ClearScreen();
}

void Application::positionCamera(int x, int y){
	cameraX = x;
	cameraY = y;
}

double Application::getTickLimit(){
	return tickLimit;
}

GameObjects* Application::getGameObjects(){
	return gameObjects;
}

int Application::getScreenWidth(){
	return screenWidth;
}

int Application::getScreenHeight(){
	return screenHeight;
}

ITrackable* Application::getTrackTarget(){
	return aiTrackTarget;
}

void Application::addDrawable(IDrawable* drawable){
	drawables.push_back(drawable);
}

void Application::removeDrawable(IDrawable* drawable){
	drawable->destroySprites();
	for(unsigned int i = 0; i < drawables.size(); i++){
		if(drawables.at(i) == drawable){
			drawables.erase(drawables.begin() + i);
			drawables.shrink_to_fit();
			i -= 1;
		}
	}
	delete drawable;
}

void Application::clearDrawables(){
	for(unsigned int i = 0; i < drawables.size(); i++){
		drawables.shrink_to_fit();
		drawables.at(i)->destroySprites();
		drawables.erase(drawables.begin() + i);
		i -= 1;
	}
	drawables.clear();
}

void Application::addPlanet(IDrawable* drawable){
	planets.push_back(drawable);
}

void Application::removePlanet(IDrawable* drawable){
	drawable->destroySprites();
	for(unsigned int i = 0; i < planets.size(); i++){
		if(planets.at(i) == drawable){
			planets.erase(planets.begin() + i);
			planets.shrink_to_fit();
			i -= 1;
		}
	}
	delete drawable;
}

void Application::clearPlanets(){
	for(unsigned int i = 0; i < planets.size(); i++){
		planets.shrink_to_fit();
		planets.at(i)->destroySprites();
		planets.erase(planets.begin() + i);
		i -= 1;
	}
	planets.clear();
}

void Application::endGame(){
	BASS_ChannelStop(gameObjects->backgroundLoop);
	clearDrawables();
	clearPlanets();
}

void Application::init(){
	this->startCountdown = (int)(startCountdownMax * tickLimit);
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
	getGameObjects()->stars = CreateSprite("./images/stars.png", WORLD_WIDTH, WORLD_HEIGHT, false);
	MoveSprite(getGameObjects()->menuButtons[0], (float)screenWidth / 2, (float)screenHeight / 2 - 65);
	MoveSprite(getGameObjects()->menuButtons[1], (float)screenWidth / 2, (float)screenHeight / 2);
	MoveSprite(getGameObjects()->menuButtons[2], (float)screenWidth / 2, (float)screenHeight / 2 + 65);
	MoveSprite(getGameObjects()->menuButtons[3], (float)screenWidth / 2, (float)screenHeight / 2 - 10);

	//Call all the sound initialisation here
	getGameObjects()->backgroundLoop = BASS_StreamCreateFile(false, "./sounds/ambience/ambient1.wav", 0, 0, BASS_SAMPLE_LOOP);
	getGameObjects()->laserFireSound = BASS_StreamCreateFile(false, "./sounds/fire_laser.wav", 0, 0, 0);
	getGameObjects()->speedUpSound = BASS_StreamCreateFile(false, "./sounds/speedup.wav", 0, 0, 0);
	BASS_ChannelSetAttribute(getGameObjects()->speedUpSound, BASS_ATTRIB_VOL, 0.50F);
	getGameObjects()->powerUpSound = BASS_StreamCreateFile(false, "./sounds/powerup.wav", 0, 0, 0);
	getGameObjects()->powerDownSound = BASS_StreamCreateFile(false, "./sounds/powerdown.wav", 0, 0, 0);
	getGameObjects()->healthUpSound = BASS_StreamCreateFile(false, "./sounds/pickup.wav", 0, 0, 0);
	getGameObjects()->explosionSound = BASS_StreamCreateFile(false, "./sounds/explode.wav", 0, 0, 0);
}

void Application::generatePlanets(){
	const int planetsToGenerate = 13;
	bool usedPlanets[13];
	for(int i = 0; i < 13; i++){
		usedPlanets[i] = false;
	}

	for(int i = 0; i < planetsToGenerate; i++){
		while(true){
			int texture = Random::random(0, 12);

			if(!usedPlanets[texture]){
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

void Application::preloadGame(){
	//Create all the desired game sprites here
	getGameObjects()->laserBeamSprite = CreateSprite("./images/beam.png", 10, 4, true);
	getGameObjects()->laserPowerUpSprite = CreateSprite("./images/powerkit.png", 128, 128, false);
	getGameObjects()->healthPowerUpSprite = CreateSprite("./images/healthkit.png", 128, 128, false);
	getGameObjects()->playerSprite = CreateSprite("./images/tanker.png", 64, 64, true);
	getGameObjects()->enemySprite = CreateSprite("./images/enemy.png", 64, 64, true);
	getGameObjects()->healthRemainSprite = DuplicateSprite(getGameObjects()->healthPowerUpSprite);
	float w = 16.f, h = 16.f;
	SetSpriteScale(getGameObjects()->healthRemainSprite, w, h);
	
	for(int i = 0; i < numExplosions; i++){
		getGameObjects()->explosionSprites[i] = CreateSprite((std::string("./images/explosion/") + std::to_string(i) + std::string(".png")).c_str(), 128, 128, true);
	}

	initGame();
	currentState = GAME;
}

void Application::initGame(){
	powerUpFrequency = (int)(30 * tickLimit);
	healthUpFrequency = (int)(20 * tickLimit);
	powerUpSpawn = 0;
	healthUpSpawn = healthUpFrequency / 4;
	proceduralDifficulty = 1.F;
	gameTicks = 0;

	//Call all the IDrawable initialisation here
	Player* pl = new Player(getGameObjects()->playerSprite);
	aiTrackTarget = pl;
	addDrawable(pl);

	generatePlanets();

	BASS_ChannelSetAttribute(getGameObjects()->backgroundLoop, BASS_ATTRIB_VOL, 0.15F);
	BASS_ChannelPlay(getGameObjects()->backgroundLoop, false);
}

int Application::updateGame(){
	static int quitTickDown;
	if(IsKeyDown(KEY_ESC)){
		if(quitTickDown >= (2 * 60)){
			return 0;
		}else{
			quitTickDown++;
		}
	}else{
		quitTickDown = 0;
	}

	int mouseX, mouseY;
	GetMouseLocation(mouseX, mouseY);

	switch(currentState){
	case SPLASH:{
		static unsigned int ticks;

		if(ticks >= 240){
			currentState = MAIN_MENU;
		}else{
			ticks++;
		}

		break;
	}
	case MAIN_MENU:
		if(GetMouseButtonDown(0)){
			if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 - 65 && mouseY < screenHeight / 2 + 30 - 65){
				currentState = TUTORIAL;
			}else if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 && mouseY < screenHeight / 2 + 30){
				currentState = HSCORES;
			}else if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 + 65 && mouseY < screenHeight / 2 + 30 + 65){
				return 0;
			}
		}

		break;
	case TUTORIAL:
		if(GetMouseButtonDown(0)){
			if(mouseX > screenWidth / 2 - 100 && mouseX < screenWidth / 2 + 100 && mouseY > screenHeight / 2 - 30 - 10 && mouseY < screenHeight / 2 + 30 - 10){
				currentState = LOADING;
			}
		}
		break;
	case LOADING:
		preloadGame();
	case GAME:{
		if(startCountdown > 0){
			startCountdown--;
		}
		int enemyCount = 0;
			//applyDifficulty();
			if(powerUpSpawn <= 0){

				int powerUpX = Random::random(64, WORLD_WIDTH - 64);
				int powerUpY = Random::random(64, WORLD_HEIGHT - 64);
				int powerUpDespawn = (int)(Random::random(50, 300) * tickLimit);

				addDrawable(new Powerup("laser", getGameObjects()->laserPowerUpSprite, powerUpX, powerUpY, powerUpDespawn));
				powerUpSpawn = powerUpFrequency;
			}else{
				powerUpSpawn--;
			}

			if(healthUpSpawn <= 0){
				healthUpSpawn = healthUpFrequency;

				int healthUpX = Random::random(64, WORLD_WIDTH - 64);
				int healthUpY = Random::random(64, WORLD_HEIGHT - 64);
				int healthUpDespawn = (int)(Random::random(100, 450) * tickLimit);

				addDrawable(new Powerup("health", getGameObjects()->healthPowerUpSprite, healthUpX, healthUpY, healthUpDespawn));
			}else{
				healthUpSpawn--;
			}

			drawables.shrink_to_fit();
			if(!drawables.empty()){
				for(unsigned int i = 0; i < drawables.size(); i++){
					IDrawable* d = drawables.at(i);
					ICollidable* col0 = dynamic_cast<ICollidable*>(d);

					if(col0 != 0 && col0->isCollideTester()){
						for(unsigned int i1 = 0; i1 < drawables.size(); i1++){
							ICollidable* col1 = dynamic_cast<ICollidable*>(drawables.at(i1));
							if(col1 != 0){
								if(col0 != col1 && (Collision::rect_intersects(col0->getCX(), col0->getCY(), col0->getWidth(), col0->getHeight(), col1->getCX(), col1->getCY(), col1->getWidth(), col1->getHeight()))){
									col0->onCollide(col1);
								}
							}
						}
					}

					//Count how many enemies there are using the main update loop
					if(dynamic_cast<Enemy*>(d) != 0){
						enemyCount++;
					}

					d->update();
				}
			}

			//enemy spawning limited to 100 enemies at a time
			if(enemyCount < 100){
				if(Random::random(0, 250) == 0){
					int xPos = Random::random(0, WORLD_WIDTH);
					int yPos = Random::random(0, WORLD_HEIGHT);
					int followRange = Random::random(480, 640);
	
					addDrawable(new Enemy(getGameObjects()->enemySprite, getGameObjects()->explosionSprites, xPos, yPos, ROT_EAST, 1.5f, 1.f, 1.5f, followRange));
				}
			}

			if(quitTickDown == 1){
				currentState = PAUSE;
				BASS_Pause();
			}

			gameTicks++;

			break;
		}
	case PAUSE:
		if(quitTickDown == 1){
			currentState = GAME;
			BASS_Start();
		}
	}
	
	return -1;
}

void Application::applyDifficulty(){
	if(((int)gameTicks % (int)(15 * tickLimit)) == 0){
		proceduralDifficulty += 0.1F;

		powerUpFrequency *= (int)proceduralDifficulty;
		healthUpFrequency *= (int)(proceduralDifficulty / 2);
	}
}