#include "GameObjects.h"
#include "AIE.h"
#include <iostream>

namespace{
	static const unsigned int SHEET_WIDTH = 512, SHEET_HEIGHT = 512, CHAR_WIDTH = 32, CHAR_HEIGHT = 32;

	SPRITE loadFontSheet(){
		return CreateSprite("./images/text.png", SHEET_WIDTH, SHEET_HEIGHT, false);
	}

	void getCharPosition(const char c, int& x, int& y){
		switch(c){
		case ' ':
			x = 0; y = 2; return;
		case '!':
			x = 1; y = 2; return;
		case '"':
			x = 2; y = 2; return;
		case '#':
			x = 3; y = 2; return;
		case '$':
			x = 4; y = 2; return;
		case '%':
			x = 5; y = 2; return;
		case '&':
			x = 6; y = 2; return;
		case '\'':
			x = 7; y = 2; return;
		case '(':
			x = 8; y = 2; return;
		case ')':
			x = 9; y = 2; return;
		case '*':
			x = 10; y = 2; return;
		case '+':
			x = 11; y = 2; return;
		case ',':
			x = 12; y = 2; return;
		case '-':
			x = 13; y = 2; return;
		case '.':
			x = 14; y = 2; return;
		case '/':
			x = 15; y = 2; return;
		case '0':
			x = 0; y = 3; return;
		case '1':
			x = 1; y = 3; return;
		case '2':
			x = 2; y = 3; return;
		case '3':
			x = 3; y = 3; return;
		case '4':
			x = 4; y = 3; return;
		case '5':
			x = 5; y = 3; return;
		case '6':
			x = 6; y = 3; return;
		case '7':
			x = 7; y = 3; return;
		case '8':
			x = 8; y = 3; return;
		case '9':
			x = 9; y = 3; return;
		case ':':
			x = 10; y = 3; return;
		case ';':
			x = 11; y = 3; return;
		case '<':
			x = 12; y = 3; return;
		case '=':
			x = 13; y = 3; return;
		case '>':
			x = 14; y = 3; return;
		case '?':
			x = 15; y = 3; return;
		case '@':
			x = 0; y = 4; return;
		case 'A':
			x = 1; y = 4; return;
		case 'B':
			x = 2; y = 4; return;
		case 'C':
			x = 3; y = 4; return;
		case 'D':
			x = 4; y = 4; return;
		case 'E':
			x = 5; y = 4; return;
		case 'F':
			x = 6; y = 4; return;
		case 'G':
			x = 7; y = 4; return;
		case 'H':
			x = 8; y = 4; return;
		case 'I':
			x = 9; y = 4; return;
		case 'J':
			x = 10; y = 4; return;
		case 'K':
			x = 11; y = 4; return;
		case 'L':
			x = 12; y = 4; return;
		case 'M':
			x = 13; y = 4; return;
		case 'N':
			x = 14; y = 4; return;
		case 'O':
			x = 15; y = 4; return;
		case 'P':
			x = 0; y = 5; return;
		case 'Q':
			x = 1; y = 5; return;
		case 'R':
			x = 2; y = 5; return;
		case 'S':
			x = 3; y = 5; return;
		case 'T':
			x = 4; y = 5; return;
		case 'U':
			x = 5; y = 5; return;
		case 'V':
			x = 6; y = 5; return;
		case 'W':
			x = 7; y = 5; return;
		case 'X':
			x = 8; y = 5; return;
		case 'Y':
			x = 9; y = 5; return;
		case 'Z':
			x = 10; y = 5; return;
		case '[':
			x = 11; y = 5; return;
		case '\\':
			x = 12; y = 5; return;
		case ']':
			x = 13; y = 5; return;
		case '^':
			x = 14; y = 5; return;
		case '_':
			x = 15; y = 5; return;
		case '`':
			x = 0; y = 6; return;
		case 'a':
			x = 1; y = 6; return;
		case 'b':
			x = 2; y = 6; return;
		case 'c':
			x = 3; y = 6; return;
		case 'd':
			x = 4; y = 6; return;
		case 'e':
			x = 5; y = 6; return;
		case 'f':
			x = 6; y = 6; return;
		case 'g':
			x = 7; y = 6; return;
		case 'h':
			x = 8; y = 6; return;
		case 'i':
			x = 9; y = 6; return;
		case 'j':
			x = 10; y = 6; return;
		case 'k':
			x = 11; y = 6; return;
		case 'l':
			x = 12; y = 6; return;
		case 'm':
			x = 13; y = 6; return;
		case 'n':
			x = 14; y = 6; return;
		case 'o':
			x = 15; y = 6; return;
		case 'p':
			x = 0; y = 7; return;
		case 'q':
			x = 1; y = 7; return;
		case 'r':
			x = 2; y = 7; return;
		case 's':
			x = 3; y = 7; return;
		case 't':
			x = 4; y = 7; return;
		case 'u':
			x = 5; y = 7; return;
		case 'v':
			x = 6; y = 7; return;
		case 'w':
			x = 7; y = 7; return;
		case 'x':
			x = 8; y = 7; return;
		case 'y':
			x = 9; y = 7; return;
		case 'z':
			x = 10; y = 7; return;
		case '{':
			x = 11; y = 7; return;
		case '|':
			x = 12; y = 7; return;
		case '}':
			x = 13; y = 7; return;
		case '~':
			x = 14; y = 7; return;
		case '\u20AC':
			x = 0; y = 8; return;
		case '\u0153':
			x = 9; y = 9; return;
		case '\u00A3':
			x = 3, y = 10; return;
		case '\u00A5':
			x = 5; y = 9; return;
		case '\u00A7':
			x = 7; y = 9; return;
		case '\u00A9':
			x = 9; y = 9; return;
		case '\u00AE':
			x = 14; y = 10; return;
		case '\u00B1':
			x = 1; y = 11; return;
		case '\u00B2':
			x = 2; y = 11; return;
		case '\u00B3':
			x = 3; y = 11; return;
		case '\u00D7':
			x = 7; y = 13; return;
		case '\u00F7':
			x = 7; y = 15; return;
		default:
			x = 0; y = 0; return;
		//The remainder of the characters are unrecognisable by me;
		}
	}

	SPRITE getCharSprite(SPRITE charSheet, int x, int y){
		y = 15 - y;
		SPRITE spr = DuplicateSprite(charSheet);
		SetSpriteUVCoordinates(spr, (float)(x * CHAR_WIDTH) / SHEET_WIDTH, (float)(y * CHAR_HEIGHT) / SHEET_HEIGHT, (float)(x * CHAR_WIDTH + CHAR_WIDTH) / SHEET_WIDTH, (float)(y * CHAR_HEIGHT + CHAR_HEIGHT) / SHEET_HEIGHT);

		return spr;
	}

}