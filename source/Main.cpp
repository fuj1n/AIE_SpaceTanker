/////////////////////////////////////////////////////////////////////////

#include "AIE.h"
#include "Game.h"
#include <cctype>
#include <windows.h>
#include <vector>
#include <string>
#include <cmath>
#include <time.h>
#include <assert.h>
#include <crtdbg.h>
#include <iostream>

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

enum States{
	SPLASH,
	MAIN_MENU,
	TUTORIAL,
	GAME,
	HSCORES
}; States currentState = SPLASH;

std::vector<IDrawable*> drawables;

class Projectile : public IDrawable, public ICollidable{
public:
	static const int width = 10;
	int height;
	int xDir, yDir, timeUntilDeath;
	boolean isSpecial;
	float x, y, rotation, currentRotation, speed, scale;

	unsigned int texture;
	Projectile(const char* textureName, int x, int y, int xDir, int yDir, float rotation, float speed, float stayTime, SColour color = SColour(0xFFFFFFFF), int length = 20, bool special = false){
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

	unsigned int getX(){
		return (int)x;
	}

	unsigned int getY(){
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

	char* getColliderName(){
		return "bullet";
	}
};

class Player : public IDrawable, public ITrackable{
public:
	static const int width = 28;
	static const int height = 41;
	float x, y, rotation, currentRotation, speed, scale;

	int shootCooldown, betterAmmoCooldown;

	unsigned int texture;
	Player(const char* textureName){
		scale = 2;
		x = WORLD_WIDTH / 2;
		y = WORLD_HEIGHT / 2;
		rotation = ROT_EAST;
		currentRotation = rotation;
		speed = 1.5F;
		texture = CreateSprite(textureName, (int)(width * scale), (int)(height * scale), true);
		MoveSprite(texture, x, y);
	}

	unsigned int getTexture(){
		return texture;
	}

	unsigned int getX(){
		return (unsigned int)x;
	}

	unsigned int getY(){
		return (unsigned int)y;
	}

	void update(){
		//Decrement the values for timed "things"
		if(shootCooldown > 0){
			shootCooldown--;
		}
		if(betterAmmoCooldown > 0){
			betterAmmoCooldown--;
		}

		float speed = this->speed * (IsKeyDown(KEY_LSHIFT) || IsKeyDown(KEY_RSHIFT) ? 2 : 1);

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
				Projectile* projectile = new Projectile("./images/beam.png", projX, projY, projXDir, projYDir, rotation, betterAmmoCooldown > 0 ? 5.5f : 4.5f, betterAmmoCooldown > 0 ? 1.5f : 1, betterAmmoCooldown > 0 ? SColour(0x00FFFFFF) : SColour(0xFFFF00FF), betterAmmoCooldown > 0 ? 30 : 20, betterAmmoCooldown > 0);
				addDrawable(projectile);
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
};

class Game{
public:
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
	}

	void initGame(){
		//Call all the IDrawable initialisation here
		Player* pl = new Player("./images/tracker.png");
		aiTrackTarget = pl;
		addDrawable(pl);
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
		case GAME:
			if(!drawables.empty()){
				for(unsigned int i = 0; i < drawables.size(); i++){
					IDrawable* d = drawables.at(i);

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

int update(Game g){
	drawables.shrink_to_fit();
	return g.update();
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
		DrawSprite(stars);
		DrawLine(0, 0, 1, WORLD_HEIGHT);
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

int main( int argc, char* argv[] )
{	
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

    Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, fscreen, "Space Tanker");

	SetBackgroundColour(SColour(0x000));
	
	Game game;

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

	for(unsigned int i = 0; i < drawables.size(); i++){
		IDrawable* d = drawables.at(i);

		DestroySprite(d->getTexture());
		delete d;
	}

    Shutdown();

    return quitStatus == -1 ? 0 : quitStatus;
}
