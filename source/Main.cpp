#pragma comment(lib, "winmm") 

#include "Application.h"
#include <string>

int main(int argc, char* argv[]){
	int displayWidth = 640, displayHeight = 480, displayFScreen = false;
	for(int i = 0; i < argc; i++){
		char* arg = argv[i];
		
		if(i + 1 < argc){
			if(arg == "-dispWidth"){
				displayWidth = atoi(argv[i + 1]);
			}else if(arg == "-dispHeight"){
				displayHeight = atoi(argv[i + 1]);
			}
		}

		if(arg == "-fscreen"){
			displayFScreen = true;
		}

		Application* appl = new Application(displayWidth, displayHeight, displayFScreen);
		int exitCode = appl->run();

		delete appl;

		return exitCode;
	}

}