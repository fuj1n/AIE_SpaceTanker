#include "Main.cpp"

#pragma once

#include <iostream>
#include <windows.h>

#define DEBUG TRUE

int retAns = -1337;

//TL;DR
//So I used WinAPI for my wrapper. Why you ask? Because I can! Also calling Shutdown() in AIE framework prevents you from ever initializing it again in that instance;;

long __stdcall WindowProcedure(HWND window, unsigned int msg, WPARAM wp, LPARAM lp) {
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	case WM_LBUTTONDOWN:
		//std::cout << "\nMouse pressed at: (" << LOWORD(lp) << ',' << HIWORD(lp) << ")\n";
		if(LOWORD(lp) > 10 && LOWORD(lp) < 60 && HIWORD(lp) > 50 && HIWORD(lp) < 62) {
			//std::cout << "Yes" << std::endl;
			retAns = true;
		} else if(LOWORD(lp) > 100 && LOWORD(lp) < 150 && HIWORD(lp) > 50 && HIWORD(lp) < 62) {
			//std::cout << "No" << std::endl;
			retAns = false;
		}
	default:
		//Draw
		HDC hdc = GetDC(window);

		LPRECT textRect = new RECT;
		textRect->left = 10;
		textRect->top = 10;

		DrawText(hdc, "Would you like fullscreen?", 26, textRect, DT_LEFT);

		textRect->top = 50;
		DrawText(hdc, "> Yes <", 7, textRect, DT_LEFT);
		textRect->left = 100;
		DrawText(hdc, "> No <", 6, textRect, DT_LEFT);

		return DefWindowProc(window, msg, wp, lp);
	}
}

int main(int argc, char* argv[]) {
	if(!DEBUG) {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	for(int i = 1; i < argc; i++) {
		std::string arg = std::string(argv[i]);

		if(arg == "-fscreen" || arg == "-nowrapper") {
			return wmain(argc, argv);
		}
	}
	int screenW, screenH;

	WindowUtils::getScreenSize(screenW, screenH);
	const char* const myclass = "fuj1n.spacetanker.wrapper";
	WNDCLASSEX wndclass = {sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure, 0, 0, GetModuleHandle(0), LoadIcon(0, IDI_APPLICATION), LoadCursor(0, IDC_ARROW), HBRUSH(RGB(0, 0, 0)), 0, myclass, LoadIcon(0, IDI_APPLICATION)};
	if(RegisterClassEx(&wndclass)) {
		HWND window = CreateWindowEx(0, myclass, "Space Tanker Wrapper", WS_OVERLAPPEDWINDOW, screenW / 2 - 210 / 2, screenH / 2 - 115 / 2, 210, 115, 0, 0, GetModuleHandle(0), 0);
		if(window) {
			ShowWindow(window, SW_SHOWDEFAULT);
			MSG msg;
			while(GetMessage(&msg, 0, 0, 0) && retAns != 1 && retAns != 0) DispatchMessage(&msg);
			CloseWindow(window);
		}
	}

	if(retAns == 1) {
		argv[argc] = "-fscreen";
		argc++;
	}

	return wmain(argc, argv);
}