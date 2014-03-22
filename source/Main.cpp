/////////////////////////////////////////////////////////////////////////

#include "AIE.h"
#include "Game.h"
#include "bass.h"
#include "Utils.cpp"
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

const char* GAME_NAME = "Space Tanker";
const char* VERSION = "v0.3";

int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
const int WORLD_WIDTH = 960, WORLD_HEIGHT = 960;
int fps, tps;

double tickLimit = 60.0;

ITrackable* aiTrackTarget;

typedef enum RotationDirections{
	ROT_NORTH = 0,
	ROT_NORTHWEST = 45,
	ROT_WEST = 90,
	ROT_SOUTHWEST = 135,
	ROT_SOUTH = 180,
	ROT_SOUTHEAST = 225,
	ROT_EAST = 270,
	ROT_NORTHEAST = 315
}RotationDirections;

void addDrawable(IDrawable*);
void removeDrawable(IDrawable*);
void clearDrawables();
void addPlanet(IDrawable*);
void removePlanet(IDrawable*);
void clearPlanets();

enum States{
	SPLASH,
	MAIN_MENU,
	TUTORIAL,
	GAME,
	HSCORES
}; States currentState = SPLASH;

std::vector<IDrawable*> planets;
std::vector<IDrawable*> drawables;

class Planet : public IDrawable{
public:
	unsigned int texture;

	Planet(const char* textureName, int x, int y, int bounds){
		texture = CreateSprite(textureName, bounds, bounds, false, SColour(0xFFFFFF77));

		SetSpriteBlendMode(texture, _SRC_COLOR, _DST_COLOR);

		MoveSprite(texture, (float)x, (float)y);
	}

	unsigned int getTexture(){
		return texture;
	}
	
	void update(){}
};

class Powerup : public ICollidable{
public:
	static const int width = 64, height = 64;
	char* powerupType;

	unsigned int x, y, texture;
	int timeLeft;

	Powerup(char* type, char* textureName, int x, int y, int stayTime){
		powerupType = type;
		texture = CreateSprite(textureName, width, height, false);
		this->x = x;
		this->y = y;
		timeLeft = stayTime;
	}

	unsigned int getTexture(){
		return texture;
	}
	
	void update(){
		MoveSprite(texture, (float)x, (float)y);

		timeLeft--;
		if(timeLeft <= 0){
			removeDrawable(this);
		}
	}

	unsigned int getCX(){
		return x;
	}

	unsigned int getCY(){
		return y;
	}

	unsigned int getWidth(){
		return width;	
	}

	unsigned int getHeight(){
		return height;
	}

	bool isCollideTester(){
		return false;
	}

	void onCollide(ICollidable* col){}

	void onTesterMessage(ICollidable* col){
		timeLeft = 0;
	}

	std::string getColliderName(){
		return std::string("powerup::") + std::string(powerupType);
	}
};

class Projectile : public ICollidable{
public:
	static const int width = 10;
	int height;
	int xDir, yDir, timeUntilDeath;
	boolean isSpecial;
	float x, y, rotation, currentRotation, speed, scale;

	unsigned int texture;
	Projectile(const char* textureName, int x, int y, int xDir, int yDir, float rotation, float speed, float stayTime, SColour color = SColour(0xFFFFFFFF), int length = 20, bool special = false, IParent* parent = 0){
		scale = 1;
		height = length;
		this->x = (float)x;
		this->y = (float)y;
		this->xDir = xDir;
		this->yDir = yDir;
		this->rotation = rotation;
		currentRotation = rotation;
		this->speed = speed;
		isSpecial = special;
		timeUntilDeath = (int)(stayTime * tickLimit);
		texture = CreateSprite(textureName, (int)(width * scale), (int)(height * scale), true);
		SetSpriteColour(texture, color);
		MoveSprite(texture, this->x, this->y);

		this->parent = parent;
	}

	unsigned int getTexture(){
		return texture;
	}

	void update(){
		timeUntilDeath--;

		x += xDir * speed;
		y += yDir * speed;

		for(int index = 0; index < 2; index++){
			if(currentRotation > rotation){
				currentRotation -= 5;
			}else if(currentRotation < rotation){
				currentRotation += 5;
			}
		}

		RotateSprite(texture, currentRotation);
		MoveSprite(texture, x, y);

		if(timeUntilDeath <= 0){
			removeDrawable(this);
		}
	}

	unsigned int getCX(){
		return (int)x;
	}

	unsigned int getCY(){
		return (int)y;
	}

	unsigned int getWidth(){
		return width;	
	}

	unsigned int getHeight(){
		return height;
	}

	bool isCollideTester(){
		return false;
	}

	void onCollide(ICollidable* col){}

	void onTesterMessage(ICollidable* col){
		if(!isSpecial){
			timeUntilDeath = 0;
		}
	}

	std::string getColliderName(){
		return "bullet";
	}
};

class Player : public ITrackable, public ICollidable{
public:
	static const int width = 64;
	static const int height = 64;
	float x, y, rotation, currentRotation, speed, scale;
	int health;

	int shootCooldown, betterAmmoCooldown;

	unsigned int texture;
	Player(const char* textureName){
		scale = 2;
		x = WORLD_WIDTH / 2;
		y = WORLD_HEIGHT / 2;
		rotation = ROT_EAST;
		currentRotation = rotation;
		speed = 1.5F;
		health = 100;
		texture = CreateSprite(textureName, (int)(width * scale), (int)(height * scale), true);
		MoveSprite(texture, x, y);
	}

	unsigned int getTexture(){
		return texture;
	}

	unsigned int getCX(){
		return (unsigned int)x - width / 2;
	}

	unsigned int getCY(){
		return (unsigned int)y - width / 2;
	}

	unsigned int getTX(){
		return (unsigned int)x;
	}

	unsigned int getTY(){
		return (unsigned int)y;
	}

	void update(){
		//Decrement the values for timed "things"
		if(shootCooldown > 0){
			shootCooldown--;
		}
		if(betterAmmoCooldown > 0){
			if(betterAmmoCooldown == 1){
				BASS_ChannelPlay(powerDownSound, false);
			}
			betterAmmoCooldown--;
		}

		static bool lastSpeed;
		float speed = this->speed * (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT) ? 2 : 1);
		if(!lastSpeed && (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT))){
			lastSpeed = true;
			BASS_ChannelPlay(speedUpSound, false);
		}else if(lastSpeed && (!IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT))){
			lastSpeed = false;
		}

		if(IsKeyDown('W') && IsKeyDown('A')){
			rotation = ROT_NORTHWEST;
			x -= 1 * speed;
			y -= 1 * speed;
		}else if(IsKeyDown('W') && IsKeyDown('D')){
			rotation = ROT_NORTHEAST;
			x += 1 * speed;
			y -= 1 * speed;
		}else if(IsKeyDown('S') && IsKeyDown('A')){
			rotation = ROT_SOUTHWEST;
			x -= 1 * speed;
			y += 1 * speed;
		}else if(IsKeyDown('S') && IsKeyDown('D')){
			rotation = ROT_SOUTHEAST;
			x += 1 * speed;
			y += 1 * speed;
		}else if(IsKeyDown('W')){
			rotation = ROT_NORTH;
			y -= 1 * speed;
		}else if(IsKeyDown('S')){
			rotation = ROT_SOUTH;
			y += 1 * speed;
		}else if(IsKeyDown('A')){
			rotation = ROT_WEST;
			x -= 1 * speed;
		}else if(IsKeyDown('D')){
			rotation = ROT_EAST;
			x += 1 * speed;
		}

		if(x < width){
			x = width;
		}else if(x > WORLD_WIDTH - width / 2){
			x = WORLD_WIDTH - width / 2;
		}

		if(y < height / 2){
			y = height / 2;
		}else if(y > WORLD_HEIGHT - height / 2){
			y = WORLD_HEIGHT - height / 2;
		}

		if(IsKeyDown(VK_SPACE) && shootCooldown <= 0){
			int projXDir = -1337, projYDir = -1337, projX = (int)x, projY = (int)y;
			switch((int)rotation){
			case ROT_NORTH:
				projXDir = 0;
				projYDir = -1;
				projY -= 40;
				break;
			case ROT_NORTHEAST:
				projXDir = 1;
				projYDir = -1;
				projX += 25;
				projY -= 25;
				break;
			case ROT_EAST:
				projXDir = 1;
				projYDir = 0;
				projX += 40;
				break;
			case ROT_SOUTHEAST:
				projXDir = 1;
				projYDir = 1;
				projX += 25;
				projY += 25;
				break;
			case ROT_SOUTH:
				projXDir = 0;
				projYDir = 1;
				projY += 40;
				break;
			case ROT_SOUTHWEST:
				projXDir = -1;
				projYDir = 1;
				projX -= 25;
				projY += 25;
				break;
			case ROT_WEST:
				projXDir = -1;
				projYDir = 0;
				projX -= 40;
				break;
			case ROT_NORTHWEST:
				projXDir = -1;
				projYDir = -1;
				projX -= 25;
				projY -= 25;
				break;
			}

			if(projXDir != -1337 && projYDir != -1337){
				Projectile* projectile = new Projectile("./images/beam.png", projX, projY, projXDir, projYDir, rotation, betterAmmoCooldown > 0 ? 5.5f : 4.5f, betterAmmoCooldown > 0 ? 1.5f : 1, betterAmmoCooldown > 0 ? SColour(0x00FFFFFF) : SColour(0xFFFF00FF), betterAmmoCooldown > 0 ? 30 : 20, betterAmmoCooldown > 0, this);
				addDrawable(projectile);
				BASS_ChannelPlay(laserFireSound, false);
				shootCooldown = (int)(0.5 * tickLimit);
			}
		}

		if(rotation - currentRotation > 180){
			currentRotation += 360;
		}else if(rotation - currentRotation < -180){
			currentRotation -= 360;
		}

		//The bigger the number after the < operator, the faster the rotation occurs
		//Reasons for high speed: cosmetic use only, and the bullet will not fire relatively to this cosmetic rotation(probably)
		for(int index = 0; index < 2; index++){
			if(currentRotation > rotation){
				currentRotation -= 5;
			}else if(currentRotation < rotation){
				currentRotation += 5;
			}
		}

		RotateSprite(texture, currentRotation);
		MoveSprite(texture, x, y);
		MoveCamera(x - SCREEN_WIDTH / 2, y - SCREEN_HEIGHT / 2);
	}

	unsigned int getWidth(){
		return width;	
	}

	unsigned int getHeight(){
		return height;
	}

	bool isCollideTester(){
		return true;
	}

	void onCollide(ICollidable* col){
		std::string colliderName = col->getColliderName();

		if(colliderName == "bullet" && !(col->parent == this)){
			health -= 5;
			col->onTesterMessage(this);
		}else if(colliderName == "enemy"){
			health -= 15;
		}else if(colliderName == "powerup::health"){
			health += 35;
			col->onTesterMessage(this);
			BASS_ChannelPlay(healthUpSound, false);
		}else if(colliderName == "powerup::laser"){
			betterAmmoCooldown = 900;
			col->onTesterMessage(this);
			BASS_ChannelPlay(powerUpSound, false);
		}

		if(health < 0){
			health = 0;
		}else if(health > 100){
			health = 100;
		}
	}

	void onTesterMessage(ICollidable* col){}

	std::string getColliderName(){
		return "player";
	}
};

class Game{
public:
	int powerUpSpawn, healthUpSpawn, powerUpFrequency, healthUpFrequency;

	Game(){
		init();
	}

	void init(){
		//Call all the sprite initialisation here
		splashTexture = CreateSprite("./images/splash.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
		instructionTexture = CreateSprite("./images/instructions.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
		menuButtons[0] = CreateSprite("./images/buttons/play.png", 200, 60, true);
		menuButtons[1] = CreateSprite("./images/buttons/hscores.png", 200, 60, true);
		menuButtons[2] = CreateSprite("./images/buttons/exit.png", 200, 60, true);
		menuButtons[3] = CreateSprite("./images/buttons/ok.png", 200, 60, true);
		stars = CreateSprite("./images/stars.png", WORLD_WIDTH, WORLD_HEIGHT, false);
		MoveSprite(menuButtons[0], (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 - 65);
		MoveSprite(menuButtons[1], (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2);
		MoveSprite(menuButtons[2], (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 + 65);
		MoveSprite(menuButtons[3], (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 - 10);

		//Call all the sound initialisation here
		backgroundLoop = BASS_StreamCreateFile(false, "./sounds/ambience/ambient1.wav", 0, 0, BASS_SAMPLE_LOOP);
		laserFireSound = BASS_StreamCreateFile(false, "./sounds/fire_laser.wav", 0, 0, 0);
		speedUpSound = BASS_StreamCreateFile(false, "./sounds/speedup.wav", 0, 0, 0);
		BASS_ChannelSetAttribute(speedUpSound, BASS_ATTRIB_VOL, 0.50F);
		powerUpSound = BASS_StreamCreateFile(false, "./sounds/powerup.wav", 0, 0, 0);
		powerDownSound = BASS_StreamCreateFile(false, "./sounds/powerdown.wav", 0, 0, 0);
		healthUpSound = BASS_StreamCreateFile(false, "./sounds/pickup.wav", 0, 0, 0);
	}

	void generatePlanets(){
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

	void initGame(){
		powerUpFrequency = (int)(30 * tickLimit);
		healthUpFrequency = (int)(20 * tickLimit);
		powerUpSpawn = 0;
		healthUpSpawn = healthUpFrequency / 4;

		//Call all the IDrawable initialisation here
		Player* pl = new Player("./images/tanker.png");
		aiTrackTarget = pl;
		addDrawable(pl);

		generatePlanets();

		BASS_ChannelSetAttribute(backgroundLoop, BASS_ATTRIB_VOL, 0.15F);
		BASS_ChannelPlay(backgroundLoop, false);
	}

	int update(){
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
				if(mouseX > SCREEN_WIDTH / 2 - 100 && mouseX < SCREEN_WIDTH / 2 + 100 && mouseY > SCREEN_HEIGHT / 2 - 30 - 65 && mouseY < SCREEN_HEIGHT / 2 + 30 - 65){
					currentState = TUTORIAL;
				}else if(mouseX > SCREEN_WIDTH / 2 - 100 && mouseX < SCREEN_WIDTH / 2 + 100 && mouseY > SCREEN_HEIGHT / 2 - 30 && mouseY < SCREEN_HEIGHT / 2 + 30){
					currentState = HSCORES;
				}else if(mouseX > SCREEN_WIDTH / 2 - 100 && mouseX < SCREEN_WIDTH / 2 + 100 && mouseY > SCREEN_HEIGHT / 2 - 30 + 65 && mouseY < SCREEN_HEIGHT / 2 + 30 + 65){
					return 0;
				}
			}

			break;
		case TUTORIAL:
			if(GetMouseButtonDown(0)){
				if(mouseX > SCREEN_WIDTH / 2 - 100 && mouseX < SCREEN_WIDTH / 2 + 100 && mouseY > SCREEN_HEIGHT / 2 - 30 - 10 && mouseY < SCREEN_HEIGHT / 2 + 30 - 10){
					initGame();
					currentState = GAME;
				}
			}

			break;
		case GAME:
			if(powerUpSpawn <= 0){

				int powerUpX = Random::random(64, WORLD_WIDTH - 64);
				int powerUpY = Random::random(64, WORLD_HEIGHT - 64);
				int powerUpDespawn = (int)(Random::random(50, 300) * tickLimit);

				addDrawable(new Powerup("laser", "./images/powerkit.png", powerUpX, powerUpY, powerUpDespawn));
				powerUpSpawn = powerUpFrequency;
			}else{
				powerUpSpawn--;
			}

			if(healthUpSpawn <= 0){
				healthUpSpawn = healthUpFrequency;

				int healthUpX = Random::random(64, WORLD_WIDTH - 64);
				int healthUpY = Random::random(64, WORLD_HEIGHT - 64);
				int healthUpDespawn = (int)(Random::random(100, 450) * tickLimit);

				addDrawable(new Powerup("health", "./images/healthkit.png", healthUpX, healthUpY, healthUpDespawn));
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

					d->update();
				}
			}
			break;
		}
		
		return -1;
	}
};


static void addDrawable(IDrawable* drawable){
	drawables.push_back(drawable);
}

static void removeDrawable(IDrawable* drawable){
	DestroySprite(drawable->getTexture());
	for(unsigned int i = 0; i < drawables.size(); i++){
		if(drawables.at(i) == drawable){
			drawables.erase(drawables.begin() + i);
			drawables.shrink_to_fit();
			i -= 1;
		}
	}
	delete drawable;
}

static void clearDrawables(){
	for(unsigned int i = 0; i < drawables.size(); i++){
		drawables.shrink_to_fit();
		DestroySprite(drawables.at(i)->getTexture());
		drawables.erase(drawables.begin() + i);
		i -= 1;
	}
	drawables.clear();
}

static void addPlanet(IDrawable* drawable){
	planets.push_back(drawable);
}

static void removePlanet(IDrawable* drawable){
	DestroySprite(drawable->getTexture());
	for(unsigned int i = 0; i < planets.size(); i++){
		if(planets.at(i) == drawable){
			planets.erase(planets.begin() + i);
			planets.shrink_to_fit();
			i -= 1;
		}
	}
	delete drawable;
}

static void clearPlanets(){
	for(unsigned int i = 0; i < planets.size(); i++){
		planets.shrink_to_fit();
		DestroySprite(planets.at(i)->getTexture());
		planets.erase(planets.begin() + i);
		i -= 1;
	}
	planets.clear();
}

int update(Game* g){
	drawables.shrink_to_fit();
	planets.shrink_to_fit();
	return g->update();
}

void draw(){
	switch(currentState){
	case SPLASH:
		MoveCamera(0, 0);
		DrawSprite(splashTexture);
		break;
	case MAIN_MENU:
		MoveCamera(0, 0);
		DrawSprite(menuButtons[0]);
		DrawSprite(menuButtons[1]);
		DrawSprite(menuButtons[2]);
		break;
	case TUTORIAL:
		MoveCamera(0, 0);
		DrawSprite(instructionTexture);
		DrawSprite(menuButtons[3]);
		break;
	case GAME:
		if(!planets.empty()){
			for(unsigned int i = 0; i < planets.size(); i++){
				IDrawable* d = planets.at(i);

				DrawSprite(d->getTexture());
			}
		}

		DrawSprite(stars);

		if(!drawables.empty()){
			for(unsigned int i = 0; i < drawables.size(); i++){
				IDrawable* d = drawables.at(i);
			
				DrawSprite(d->getTexture());
			}
		}
		break;
	}

	DrawString(std::string(std::string("FPS: ") + std::to_string(fps) + std::string(" TPS: ") + std::to_string(tps)).c_str(), 10, 10); 

	ClearScreen();
}

//Cleans the memory at the end of execution
void cleanup(){
	clearDrawables();
	clearPlanets();
	//Triggers a breakpoint in AIE Template_d.exe
	//delete[] menuButtons;
	//_CrtIsValidHeapPointer( pUserData ) Debug assertion failed
	//delete &SCREEN_WIDTH, &SCREEN_HEIGHT, &WORLD_WIDTH, &WORLD_HEIGHT, &fps, &tps, &splashTexture, &instructionTexture, &stars, &backgroundLoop, &laserFireSound, &speedUpSound;
}

int main( int argc, char* argv[] )
{	
	srand(timeGetTime());
	int scale = 1;
	bool fscreen = false;
	if(argc >= 2){
		for(int ind = 1; ind < argc; ind++){
			if(std::string(argv[ind]) == "-dres"){
				scale = 2;
				SCREEN_WIDTH *= scale;
				SCREEN_HEIGHT *= scale;
			}else if(std::string(argv[ind]) == "-tres"){
				scale = 3;
				SCREEN_WIDTH *= scale;
				SCREEN_HEIGHT *= scale;
			}else if(std::string(argv[ind]) == "-qres"){
				scale = 4;
				SCREEN_WIDTH *= scale;
				SCREEN_HEIGHT *= scale;
			}else if(std::string(argv[ind]) == "-fscreen"){
				//Fullscreen hides the mouse, which can be a problem, might draw a sprite at the mouse location
				fscreen = true;
			}
		}

	}
	
    Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, fscreen, (std::string("Space Tanker ") + std::string(VERSION)).c_str());

	//BASS init stuffs
	BASS_Init(-1, 44100, 0, 0, 0); 

	SetBackgroundColour(SColour(0x000));
	
	Game* game = new Game();

	int quitStatus = -1;

	//Unfortunately, most precise time I can get in C++ reliably are the milliseconds, would've loved to use nanotime
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
			quitStatus = update(game);
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

	cleanup();

	Shutdown();

    return quitStatus == -1 ? 0 : quitStatus;
}

