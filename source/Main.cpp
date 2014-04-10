#pragma comment(lib, "winmm") 

#include "Application.h"
#include <string>

int main(int argc, char* argv[]){
	int displayWidth = 640, displayHeight = 480;
	bool displayFScreen = false;

	for(unsigned int i = 1; i < argc; i++){
		std::string arg = std::string(argv[i]);
		
		std::cout << arg << std::endl;

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
	}

	Application* appl = new Application(displayWidth, displayHeight, displayFScreen);

	int exitCode = appl->run();

	//Causes a debug assertion failure
	//delete appl;

	return exitCode;

}