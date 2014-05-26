#include "AIE.h"
#include "CharSpriteUtils.cpp"
#include "WorldData.hpp"
#include "PrimitiveClasses.cpp"
#include <random>
#include <vector>
#include <string>
#include <fstream>
#include <ostream>
#include <type_traits>
#include <bitset>

#pragma once

namespace {
	template <class kty, class vty>
	/*
	A simplified version of a hashmap
	Note: does not work well with primitive types, use PrimitiveClasses.cpp instead
	*/
	class SimplifiedHashmap {
	public:
		typedef kty keyType;
		typedef vty valueType;
	private:
		std::vector<keyType>* keys;
		std::vector<valueType>* values;
	public:
		const keyType noElement;

		SimplifiedHashmap() {
			keys = new std::vector<keyType>();
			values = new std::vector<valueType>();
		}

		valueType get(keyType key) {
			int vecID = getElementID(key);

			if(vecID < 0) {
				return NULL;
			} else {
				return values->at(vecID);
			}
		}

		const valueType &operator[](keyType key) const {
			return get(key);
		}

		int getElementID(keyType key) {
			if(keys->size() != values->size()) {
				std::cout << "Fatal Error! Map desync.";
				throw(42);
			}

			for(unsigned int i = 0; i < keys->size(); i++) {
				if(keys->at(i) == key) {
					return i;
				}
			}

			return -1;
		}

		void put(keyType key, valueType value) {
			int vecID = getElementID(key);
			if(vecID >= 0) {
				remove(key);
			}
			keys->shrink_to_fit();
			values->shrink_to_fit();

			keys->emplace_back(key);
			values->emplace_back(value);
		}

		void remove(keyType key) {
			int vecID = getElementID(key);
			if(vecID >= 0) {
				keys->erase(keys->begin() + vecID);
				values->erase(values->begin() + vecID);
				keys->shrink_to_fit();
				values->shrink_to_fit();
			}
		}

		void clear() {
			keys->clear();
			values->clear();
			keys->shrink_to_fit();
			values->shrink_to_fit();
		}

		unsigned int size() {
			if(keys->size() != values->size()) {
				std::cout << "Fatal Error! Map desync.";
				throw(42);
			}

			return keys->size();
		}

		std::vector<keyType>* getKeys() {
			return keys;
		}

		std::vector<valueType>* getValues() {
			return values;
		}

		bool isEmpty() {
			return keys->empty();
		}

		bool containsKey(keyType key) {
			if(keys->size() != values->size()) {
				std::cout << "Fatal Error! Map desync.";
				throw(42);
			}

			for(unsigned int i = 0; i < keys->size(); i++) {
				if(keys->at(i) == key) {
					return true;
				}
			}

			return false;
		}

		bool containsValue(valueType value) {
			if(keys->size() != values->size()) {
				std::cout << "Fatal Error! Map desync.";
				throw(42);
			}

			for(unsigned int i = 0; i < keys->size(); i++) {
				if(values->at(i) == value) {
					return true;
				}
			}

			return false;
		}

		//friend std::ostream& operator<<(std::ostream& stream, const SimplifiedHashmap& shmp) {
		//	shmp.print(stream);
		//	return stream;
		//}

		void print(std::ostream& stream) {
			for(unsigned int i = 0; i < this->size(); i++) {
				stream << getKeys()->at(i) << " : " << getValues()->at(i) << std::endl;
			}
		}

		SimplifiedHashmap<keyType, valueType>* clone() {
			SimplifiedHashmap<keyType, valueType>* temp = new SimplifiedHashmap<keyType, valueType>();

			if(!isEmpty()) {
				for(unsigned int i = 0; i < size(); i++) {
					temp->put(keys->at(i), values->at(i));
				}
			}

			return temp;
		}
	};

	namespace Input {
		SimplifiedHashmap<Integer, Boolean> keys = SimplifiedHashmap<Integer, Boolean>();
		int alphabet[26];
		int typekit[28];

		void fillDefaults() {
			int realIndex = 0;
			for(unsigned int i = 'A'; i <= 'Z'; i++) {
				alphabet[realIndex] = i;
				typekit[realIndex] = i;
				realIndex++;
			}
			typekit[26] = KEY_BACKSPACE;
			typekit[27] = KEY_SPACE;
		}

		int count(int* keyCodes) {
			int count = 0;
			int key = keyCodes[count];
			while(key != '\0') {
				count++;
				key = keyCodes[count];
			}
			return count;
		}

		int getNewPressedKey(int* keyCodes...) {
			unsigned int size = count(keyCodes);
			for(unsigned int i = 0; i < size; i++) {
				if(!IsKeyDown(keyCodes[i])) {
					keys.put(keyCodes[i], false);
				}
			}

			for(unsigned int i = 0; i < size; i++) {
				if(IsKeyDown(keyCodes[i]) && (!keys.containsKey(keyCodes[i]) || keys.get(keyCodes[i]) == false)) {
					keys.put(keyCodes[i], true);
					return keyCodes[i];
				}
			}
			return 0;
		}
	}

	namespace WindowUtils {
		void getScreenSize(int &width, int &height) {
			RECT desktop;

			const HWND hDesktop = GetDesktopWindow();

			GetWindowRect(hDesktop, &desktop);

			width = desktop.right;
			height = desktop.bottom;
		}

		void getWindowSize(HWND handle, int &width, int &height) {
			RECT window;

			GetWindowRect(handle, &window);

			width = window.right;
			height = window.bottom;
		}
	}

	namespace ConversionIO {
		LPCWSTR to_lpcwstr(const std::string s) {
			int len;
			int slength = (int)s.length() + 1;
			len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
			wchar_t* buf = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
			std::wstring r(buf);
			delete[] buf;
			return r.c_str();
		}
	}

	namespace GameUtils {

		/*
			Get modifiers for projectile position, rotation and direction
			@param par1 Corner offset
			@param par2 Side offset (usually double the corner offset)
			*/
		void getProjectilePropertyModifiers(float rotation, int& projXDir, int& projYDir, int& projX, int& projY, int par1 = 25, int par2 = 50) {
			switch((int)rotation) {
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

		void movePlayer(float& x, float& y, float& rotation, float speed, bool wDown, bool sDown, bool aDown, bool dDown, int width, int height) {
			if(IsKeyDown('W') && IsKeyDown('A')) {
				rotation = ROT_NORTHWEST;
				x -= 1 * speed;
				y -= 1 * speed;
			} else if(IsKeyDown('W') && IsKeyDown('D')) {
				rotation = ROT_NORTHEAST;
				x += 1 * speed;
				y -= 1 * speed;
			} else if(IsKeyDown('S') && IsKeyDown('A')) {
				rotation = ROT_SOUTHWEST;
				x -= 1 * speed;
				y += 1 * speed;
			} else if(IsKeyDown('S') && IsKeyDown('D')) {
				rotation = ROT_SOUTHEAST;
				x += 1 * speed;
				y += 1 * speed;
			} else if(IsKeyDown('W')) {
				rotation = ROT_NORTH;
				y -= 1 * speed;
			} else if(IsKeyDown('S')) {
				rotation = ROT_SOUTH;
				y += 1 * speed;
			} else if(IsKeyDown('A')) {
				rotation = ROT_WEST;
				x -= 1 * speed;
			} else if(IsKeyDown('D')) {
				rotation = ROT_EAST;
				x += 1 * speed;
			}

			if(x < width) {
				x = (float)width;
			} else if(x > WORLD_WIDTH - width / 2) {
				x = (float)(WORLD_WIDTH - width / 2);
			}

			if(y < height / 2) {
				y = (float)(height / 2);
			} else if(y > WORLD_HEIGHT - height / 2) {
				y = (float)(WORLD_HEIGHT - height / 2);
			}
		}

		void move(int& x, int& y, int xDirection, int yDirection, float speed, bool boundsCheck = false, int width = 0, int height = 0) {
			x += (int)(xDirection * speed);
			y += (int)(yDirection * speed);
		}

		void move(float& x, float& y, int xDirection, int yDirection, float speed, bool boundsCheck = false, int width = 0, int height = 0) {
			x += xDirection * speed;
			y += yDirection * speed;
		}

		void rotate(float& rotation, int xSide, int ySide) {
			if(xSide == -1 && ySide == -1) {
				rotation = ROT_NORTHWEST;
			} else if(xSide == 0 && ySide == -1) {
				rotation = ROT_NORTH;
			} else if(xSide == 1 && ySide == -1) {
				rotation = ROT_NORTHEAST;
			} else if(xSide == 1 && ySide == 0) {
				rotation = ROT_EAST;
			} else if(xSide == 1 && ySide == 1) {
				rotation = ROT_SOUTHEAST;
			} else if(xSide == 0 && ySide == 1) {
				rotation = ROT_SOUTH;
			} else if(xSide == -1 && ySide == 1) {
				rotation = ROT_SOUTHWEST;
			} else if(xSide == -1 && ySide == 0) {
				rotation = ROT_WEST;
			}
		}

		void currRotation(float& currentRotation, float rotation, int rotationMultiplier = 2) {
			if(rotation - currentRotation > 180) {
				currentRotation += 360;
			} else if(rotation - currentRotation < -180) {
				currentRotation -= 360;
			}

			for(int index = 0; index < rotationMultiplier; index++) {
				if(currentRotation > rotation) {
					currentRotation -= 5;
				} else if(currentRotation < rotation) {
					currentRotation += 5;
				}
			}

			if((int)currentRotation % 5 != 0) {
				currentRotation = 0.f;
			}
		}
	}

	namespace Collision {

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
		bool rect_intersects(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
			if(aw <= 0 || ah <= 0 || bw <= 0 || bh <= 0) {
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

	namespace Random {
		int random(int nMin, int nMax) {
			return nMin + (int)((double)rand() / (RAND_MAX + 1) * (nMax - nMin + 1));
		}
	}

	namespace Math {
		long getBobber(float f) {
			long l;
			f = (f - (long)f);
			std::string str = std::to_string(f).substr(2);
			int floaterFinder = str.find("0");
			str = str.substr(0, floaterFinder);
			l = atoi(str.c_str());

			return l;
		}

		float roundf(float x) {
			return x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f);
		}

		int countNumbers(double i) {
			std::string str = std::to_string(i);

			return str.length();
		}

		int countNumbers(float i) {
			return countNumbers((double)i);
		}

		int countNumbers(int i) {
			return countNumbers((double)i);
		}
	}

	namespace DrawIO {
		SPRITE line = 1337;
		SPRITE textSheet = 1337;

		std::vector<SPRITE> destroyQueue;

		void update() {
			if(!destroyQueue.empty()) {
				for(unsigned int i = 0; i < destroyQueue.size(); i++) {
					DestroySprite(destroyQueue.at(i));
				}
				destroyQueue.clear();
			}
		}

		void drawLine(float x, float y, float length, float thickness, float rotation, SColour color = SColour(0xFFFFFFFF)) {
			if(line == 1337) {
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

		void drawRect(float x, float y, float width, float height, float rotation, float thickness, SColour color = SColour(0xFFFFFFFF)) {
			drawLine(x, y, width, thickness, rotation, color);
			drawLine(x, y + height - thickness, width, thickness, rotation, color);
			drawLine(x, y, thickness, height, rotation, color);
			drawLine(x + width - thickness, y, thickness, height, rotation, color);
		}

		void fillRect(float x, float y, float width, float height, float rotation, SColour color = SColour(0xFFFFFFFF)) {
			drawLine(x, y, width, height, rotation, color);
		}

		void drawString(std::string s, float x, float y, float width, float height, float spacing, SColour color = SColour(0xFFFFFFFF), bool useDefaultForUnknown = false) {
			if(textSheet == 1337) {
				textSheet = loadFontSheet();
			}

			float cX = x, cY = y;

			for(unsigned int i = 0; i < s.length(); i++) {
				char c = s.c_str()[i];
				if(c != '\0') {
					int sX, sY;
					if(c != ' ') {
						getCharPosition(c, sX, sY);
						if(sX == 0 && sY == 0 && useDefaultForUnknown) {
							DrawString(&c, (int)cX, (int)cY, color);
						} else {
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
		}

		void drawString(char* s, float x, float y, float width, float height, float spacing, SColour color = SColour(0xFFFFFFFF)) {
			drawString(std::string(s), x, y, width, height, spacing, color);
		}
	}

	namespace FileIO {
		void write(char* file, SimplifiedHashmap<std::string, std::string>* map) {
			std::fstream bo;

			bo.open(file, std::ios_base::out);
			if(bo.is_open()) {
				for(unsigned int i = 0; i < map->size(); i++) {
					std::string key = map->getKeys()->at(i);
					if(key != "") {
						std::string value = map->get(key);
						if(value != "") {
							bo << key << ":" << value;
							if(i + 1 < map->size()) {
								bo << std::endl;
							}
						}
					}
				}

				bo.sync();
				bo.close();
				bo.clear();
			} else {
				std::cout << "Error, access to file " << file << " is denied!" << std::endl;
			}
		}

		void read(char* file, SimplifiedHashmap<std::string, std::string>* map) {
			std::fstream br;

			br.open(file, std::ios_base::in);
			if(br.is_open()) {
				int lineCounter = 1;
				while(!br.eof()) {
					std::string inBuffer;
					std::getline(br, inBuffer);

					if(inBuffer.find(':', 0) == std::string::npos) {
						std::cout << "Syntax error on line " << lineCounter << " in file " << file << "." << std::endl;
					} else {
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
			} else {
				std::cout << "Error, access to file " << file << " is denied or the file doesn't exist" << std::endl;
			}
		}
	}

	namespace String {
		std::string replaceChar(std::string str, char ch1, char ch2) {
			for(unsigned int i = 0; i < str.length(); ++i) {
				if(str[i] == ch1) {
					str[i] = ch2;
				}
			}

			return str;
		}
	}

	namespace Crypt {
		std::string encryptDecrypt(const char* key, std::string crypt) {
			std::string output = crypt;

			for(unsigned int i = 0; i < crypt.size(); i++)
				output[i] = crypt[i] ^ key[i % (sizeof(key) / sizeof(char))];

			return output;
		}

		template<class keyType, class valueType>
		SimplifiedHashmap<keyType, valueType>* encryptDecryptMap(const char* key, SimplifiedHashmap<keyType, valueType>* source) {
			SimplifiedHashmap<keyType, valueType>* temp = source->clone();

			for(unsigned int i = 0; i < temp->size(); i++) {
				temp->getKeys()->at(i) = encryptDecrypt(key, temp->getKeys()->at(i));
				temp->getValues()->at(i) = encryptDecrypt(key, temp->getValues()->at(i));
			}

			return temp;
		}
	}
}