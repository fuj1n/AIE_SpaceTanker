#include "AIE.h"
#include "Application.h"
#include <random>

#pragma once

namespace{
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
}