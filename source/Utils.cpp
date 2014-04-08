#include "AIE.h"
#include "Application.h"
#include "CharSpriteUtils.cpp"
#include <random>
#include <vector>
#include <string>
#include <fstream>
#include <initializer_list>
#include <type_traits>

#pragma once

namespace{
	template <class kty, class vty>
	class SimplifiedHashmap{
	public:
		typedef kty keyType;
		typedef vty valueType;
	private:
		std::vector<keyType>* keys;
		std::vector<valueType>* values;
	public:
		const keyType noElement;

		SimplifiedHashmap(){
			keys = new std::vector<keyType>();
			values = new std::vector<valueType>();
		}

		valueType get(keyType key){
			int vecID = getElementID(key);

			if(vecID < 0){
				return 0;
			}
			else{
				return values->at(vecID);
			}
		}

		const valueType &operator[](keyType key) const{
			return get(key);
		}

		int getElementID(keyType key){
			if(keys->size() != values->size()){
				std::cout << "Fatal Error! Map desync.";
				throw(42);
			}

			for(unsigned int i = 0; i < keys->size(); i++){
				if(keys->at(i) == key){
					return i;
				}
			}

			return -1;
		}

		void put(keyType key, valueType value){
			int vecID = getElementID(key);
			if(vecID >= 0){
				remove(key);
			}
			keys->shrink_to_fit();
			values->shrink_to_fit();

			keys->emplace_back(key);
			values->emplace_back(value);
		}

		void remove(keyType key){
			int vecID = getElementID(key);
			if(vecID >= 0){
				keys->erase(keys->begin() + vecID);
				values->erase(values->begin() + vecID);
				keys->shrink_to_fit();
				values->shrink_to_fit();
			}
		}

		unsigned int size(){
			if(keys->size() != values->size()){
				std::cout << "Fatal Error! Map desync.";
				throw(42);
			}

			return keys->size();
		}

		std::vector<keyType>* getKeys(){
			return keys;
		}

		std::vector<valueType>* getValues(){
			return values;
		}
	};

	namespace GameUtils{

		/*
			Get modifiers for projectile position, rotation and direction
				@param par1 Corner offset
				@param par2 Side offset (usually double the corner offset)
		*/
		void getProjectilePropertyModifiers(float rotation, int& projXDir, int& projYDir, int& projX, int& projY, int par1 = 25, int par2 = 50){
			switch((int)rotation){
			case ROT_NORTH:
				projXDir = 0;
				projYDir = -1;
				projY -= par2;
				break;
			case ROT_NORTHEAST:
				projXDir = 1;
				projYDir = -1;
				projX += par1;
				projY -= par1;
				break;
			case ROT_EAST:
				projXDir = 1;
				projYDir = 0;
				projX += par2;
				break;
			case ROT_SOUTHEAST:
				projXDir = 1;
				projYDir = 1;
				projX += par1;
				projY += par1;
				break;
			case ROT_SOUTH:
				projXDir = 0;
				projYDir = 1;
				projY += par2;
				break;
			case ROT_SOUTHWEST:
				projXDir = -1;
				projYDir = 1;
				projX -= par1;
				projY += par1;
				break;
			case ROT_WEST:
				projXDir = -1;
				projYDir = 0;
				projX -= par2;
				break;
			case ROT_NORTHWEST:
				projXDir = -1;
				projYDir = -1;
				projX -= par1;
				projY -= par1;
				break;
			}
		}

		void movePlayer(float& x, float& y, float& rotation, float speed, bool wDown, bool sDown, bool aDown, bool dDown, int width, int height){
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
				x = (float)width;
			}else if(x > WORLD_WIDTH - width / 2){
				x = (float)(WORLD_WIDTH - width / 2);
			}

			if(y < height / 2){
				y = (float)(height / 2);
			}else if(y > WORLD_HEIGHT - height / 2){
				y = (float)(WORLD_HEIGHT - height / 2);
			}
		}

		void move(int& x, int& y, int xDirection, int yDirection, float speed, bool boundsCheck = false, int width = 0, int height = 0){
			x += (int)(xDirection * speed);
			y += (int)(yDirection * speed);
		}

		void move(float& x, float& y, int xDirection, int yDirection, float speed, bool boundsCheck = false, int width = 0, int height = 0){
			x += xDirection * speed;
			y += yDirection * speed;
		}

		void rotate(float& rotation, int xSide, int ySide){
			if(xSide == -1 && ySide == -1){
				rotation = ROT_NORTHWEST;
			}else if(xSide == 0 && ySide == -1){
				rotation = ROT_NORTH;
			}else if(xSide == 1 && ySide == -1){
				rotation = ROT_NORTHEAST;
			}else if(xSide == 1 && ySide == 0){
				rotation = ROT_EAST;
			}else if(xSide == 1 && ySide == 1){
				rotation = ROT_SOUTHEAST;
			}else if(xSide == 0 && ySide == 1){
				rotation = ROT_SOUTH;
			}else if(xSide == -1 && ySide == 1){
				rotation = ROT_SOUTHWEST;
			}else if(xSide == -1 && ySide == 0){
				rotation = ROT_WEST;
			}else{
				rotation = rotation > 180 ? 1.f : 181.f;
			}
		}

		void currRotation(float& currentRotation, float rotation, int rotationMultiplier = 2){
			if(rotation - currentRotation > 180){
				currentRotation += 360;
			}else if(rotation - currentRotation < -180){
				currentRotation -= 360;
			}

			for(int index = 0; index < rotationMultiplier; index++){
				if(currentRotation > rotation){
					currentRotation -= 5;
				}else if(currentRotation < rotation){
					currentRotation += 5;
				}
			}
		}
	}

	namespace Collision{

		/*
		Basic rectangular collision test
		ax = the X of the first rectangle
		ay = the Y of the first rectangle
		aw = the width of the first rectangle
		ah = the height of the first rectangle
		bx = the X of the second rectangle
		by = the Y of the second rectangle
		bw = the width of the second rectangle
		bh = the height of the second rectangle
		*/
		bool rect_intersects(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh){
			if (aw <= 0 || ah <= 0 || bw <= 0 || bh <= 0) {
				return false;
			}
			//r = a t = b
			aw += ax;
			ah += ay;
			bw += bx;
			bh += by;
			//      overflow || intersect
			return ((aw < ax || aw > bx) && (ah < ay || ah > by) && (bw < bx || bw > ax) && (bh < by || bh > ay));
		}
	}

	namespace Random{
		int random(int nMin, int nMax){
			return nMin + (int)((double)rand() / (RAND_MAX+1) * (nMax-nMin+1));
		}
	}

	namespace DrawIO{
		SPRITE line = 1337;
		SPRITE textSheet = 1337;

		std::vector<SPRITE> destroyQueue;

		void update(){
			if(!destroyQueue.empty()){
				for(unsigned int i = 0; i < destroyQueue.size(); i++){
					DestroySprite(destroyQueue.at(i));
				}
				destroyQueue.clear();
			}
		}

		void drawLine(float x, float y, float length, float thickness, float rotation, SColour color = SColour(0xFFFFFFFF)){
			if(line == 1337){
				line = CreateSprite("./images/line.png", 1, 1, false);
			}

			SPRITE line2 = DuplicateSprite(line);
			SetSpriteScale(line2, length, thickness);
			SetSpriteColour(line2, color);
			RotateSprite(line2, rotation);
			MoveSprite(line2, x, y);

			DrawSprite(line2);
			destroyQueue.push_back(line2);
		}

		void drawRect(float x, float y, float width, float height, float rotation, float thickness, SColour color = SColour(0xFFFFFFFF)){
			drawLine(x, y, width, thickness, rotation, color);
			drawLine(x, y + height - thickness, width, thickness, rotation, color);
			drawLine(x, y, thickness, height, rotation, color);
			drawLine(x + width - thickness, y, thickness, height, rotation, color);
		}

		void fillRect(float x, float y, float width, float height, float rotation, SColour color = SColour(0xFFFFFFFF)){
			drawLine(x, y, width, height, rotation, color);
		}

		void drawString(std::string s, float x, float y, float width, float height, float spacing, SColour color = SColour(0xFFFFFFFF), bool useDefaultForUnknown = false){
			if(textSheet == 1337){
				textSheet = loadFontSheet();
			}

			float cX = x, cY = y;

			for(unsigned int i = 0; i < s.length(); i++){
				char c = s.c_str()[i];
				int sX, sY;
				if(c != ' '){
					getCharPosition(c, sX, sY);
					if(sX == 0 && sY == 0 && useDefaultForUnknown){
						DrawString(&c, (int)cX, (int)cY, color);
					}
					else{
						SPRITE spr = getCharSprite(textSheet, sX, sY);
						SetSpriteScale(spr, width, height);
						SetSpriteColour(spr, color);
						MoveSprite(spr, cX, cY);
						DrawSprite(spr);
						destroyQueue.push_back(spr);
					}
				}
				cX += (width / 2) + spacing;
			}
		}

		void drawString(char* s, float x, float y, float width, float height, float spacing, SColour color = SColour(0xFFFFFFFF)){
			drawString(std::string(s), x, y, width, height, spacing, color);
		}
	}

	namespace FileIO{
		void write(char* file, SimplifiedHashmap<std::string, std::string>* map){
			std::fstream bo;
			
			bo.open(file, std::ios_base::out);
			if(bo.is_open()){
				for(unsigned int i = 0; i < map->size(); i++){
					std::string key = map->getKeys()->at(i);
					if(key != ""){
						std::string value = map->get(key);
						if(value != ""){
							bo << key << ":" << value;
							if(i + 1 < map->size()){
								bo << std::endl;
							}
						}
					}
				}

				bo.sync();
				bo.close();
				bo.clear();
			}else{
				std::cout << "Error, access to file " << file << " is denied!" << std::endl;
			}
		}

		void read(char* file, SimplifiedHashmap<std::string, std::string>* map){
			std::fstream br;

			br.open(file, std::ios_base::in);
			if(br.is_open()){
				int lineCounter = 1;
				while(!br.eof()){
					std::string inBuffer;
					br >> inBuffer;

					if(inBuffer.find(':', 0) == std::string::npos){
						std::cout << "Syntax error on line " << lineCounter << " in file " << file << "." << std::endl;
					}else{
						size_t separatorLocation = inBuffer.find(':', 0);
						std::string key = inBuffer.substr(0, separatorLocation);
						std::string value = inBuffer.substr(separatorLocation + 1);;

						map->put(key, value);
					}

					lineCounter++;
				}

				br.sync();
				br.close();
				br.clear();
			}else{
				std::cout << "Error, access to file " << file << " is denied or the file doesn't exist" << std::endl;
			}
		}
	}
}