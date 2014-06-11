#include "Main.cpp"

#pragma once

#include "resource1.h"
#include <iostream>
#include <windows.h>
#include <sal.h>

//Uncomment if the console is required
//#define DEBUG

int retAns = -1337;

//TL;DR
//So I used WinAPI for my wrapper. Why you ask? Because I can! Also calling Shutdown() in AIE framework prevents you from ever initializing it again in that instance;;

#ifndef DEBUG
int mainprogcall(int, char*[]);
#endif

long __stdcall WindowProcedure(HWND window, unsigned int msg, WPARAM wp, LPARAM lp) {
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	case WM_LBUTTONDOWN:
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

#ifndef DEBUG
int _stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int cmdShow) {
	applInstance = hInstance;

	std::istringstream buf(lpCmdLine);
	std::istream_iterator<std::string> beg(buf), end;

	std::vector<std::string> tokens(beg, end);

	int argc = tokens.size();
	char* argv[16];

	for(int i = 0; i < argc; i++) {
		//Suppressing a code analysis warning that does not actually show up in the warnings screen, preventing annoyance
#pragma warning(suppress: 6386)
		argv[i + 1] = const_cast<char*>(tokens.at(i).c_str());
	}

	mainprogcall(argc + 1, argv);
}
#endif

#ifndef DEBUG
int mainprogcall(int argc, char* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
	for(int i = 1; i < argc; i++) {
		std::string arg = std::string(argv[i]);

		if(arg == "-fscreen" || arg == "-nowrapper") {
			return wmain(argc, argv);
		}
	}
	int screenW, screenH;

	WindowUtils::getScreenSize(screenW, screenH);
	const char* const myclass = "fuj1n.spacetanker.wrapper";
	WNDCLASSEX wndclass = {sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure, 0, 0, GetModuleHandle(0), LoadIcon(applInstance, MAKEINTRESOURCE(IDI_ICON1)), LoadCursor(0, IDC_ARROW), HBRUSH(RGB(0, 0, 0)), 0, myclass, LoadIcon(applInstance, MAKEINTRESOURCE(IDI_ICON1))};
	if(RegisterClassEx(&wndclass)) {
		HWND window = CreateWindowEx(WS_EX_PALETTEWINDOW | WS_THICKFRAME, myclass, "Space Tanker Wrapper", WS_OVERLAPPEDWINDOW, screenW / 2 - 210 / 2, screenH / 2 - 115 / 2, 210, 115, 0, 0, GetModuleHandle(0), 0);
		if(window) {
			ShowWindow(window, SW_SHOWDEFAULT);
			MSG msg;
			while(GetMessage(&msg, 0, 0, 0) && retAns != 1 && retAns != 0) DispatchMessage(&msg);
			ShowWindow(window, SW_HIDE);
		}
	}

	if(retAns == 1) {
		argv[argc] = "-fscreen";
		argc++;
	}

	return wmain(argc, argv);
}