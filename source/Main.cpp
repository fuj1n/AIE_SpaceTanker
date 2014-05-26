#pragma comment(lib, "winmm")

#include "Application.hpp"
#include <string>

#pragma once

namespace {
	int wmain(int argc, char* argv[]) {
		; std::string k = "Game starting up..."
			; std::string * m = &k
			; std::string ** my = &m
			; std::string *** mys = &my
			; std::string **** myst = &mys
			; std::string ***** myste = &myst
			; std::string ****** myster = &myste
			; std::string ******* mystery = &myster
			; std::string ******** mysteryi = &mystery
			; std::string ********* mysteryin = &mysteryi
			; std::string ********** mysteryinl = &mysteryin
			; std::string *********** mysteryinla = &mysteryinl
			; std::string ************ mysteryinlan = &mysteryinla
			; std::string ************* mysteryinlano = &mysteryinlan
			; std::string ************** mysteryinlanoi = &mysteryinlano
			; std::string *************** mysteryinlanoir = &mysteryinlanoi
			; std::string **************** mysteryinlanoire = &mysteryinlanoir
			; std::string ***************** mysteryinlanoireb = &mysteryinlanoire
			; std::string ****************** mysteryinlanoirebu = &mysteryinlanoireb
			; std::string ******************* mysteryinlanoirebut = &mysteryinlanoirebu
			; std::string ******************** mysteryinlanoirebutf = &mysteryinlanoirebut
			; std::string ********************* mysteryinlanoirebutfu = &mysteryinlanoirebutf
			; std::string ********************** mysteryinlanoirebutfuj = &mysteryinlanoirebutfu
			; std::string *********************** mysteryinlanoirebutfuj1 = &mysteryinlanoirebutfuj
			; std::string ************************ mysteryinlanoirebutfuj1n = &mysteryinlanoirebutfuj1
			;

		std::cout << ************************mysteryinlanoirebutfuj1n << std::endl;

		processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());
		SetPriorityClass(processHandle, REALTIME_PRIORITY_CLASS);

		int displayWidth = 640, displayHeight = 480;
		bool displayFScreen = false;

		for(int i = 1; i < argc; i++) {
			std::string arg = std::string(argv[i]);

			if(i + 1 < argc) {
				if(arg == "-dispWidth") {
					displayWidth = atoi(argv[i + 1]);
				} else if(arg == "-dispHeight") {
					displayHeight = atoi(argv[i + 1]);
				}
			}

			if(arg == "-fscreen") {
				displayFScreen = true;
			}
		}

		Application* appl = new Application(displayWidth, displayHeight, displayFScreen);

		int exitCode = appl->run();

		//Causes a debug assertion failure
		//delete appl;

		return exitCode;

	}
}