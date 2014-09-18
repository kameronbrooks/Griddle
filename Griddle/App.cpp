#include "App.h"
#include <iostream>

using namespace std;

namespace Griddle {

	MainApplication::MainApplication() {

	}
	int MainApplication::init() {
		
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
			cout << "sdl_init_everything failed" << endl;
			return -1;
		}
		if ((_window = SDL_CreateWindow(
			_name.c_str(), _windowX, _windowY, _windowWidth, _windowHeight,
			SDL_WINDOW_OPENGL)) == NULL) {
			cout << "failed to create window" << endl;
			return -1;
		}
		else {
			_glContext = SDL_GL_CreateContext(_window);
		}
		if (TTF_Init() == -1) {
			cout << "ttf_init failed" << endl;
			return -1;
		}
		int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
		int img_init_out = IMG_Init(img_flags);
		if ((img_init_out & img_flags) != img_flags) {
			cout << "img_init failed" << endl;
			return -1;
		}
		initGL();

		if (!(glewInit() == GLEW_OK)) {
			cout << "glew init failed" << endl;
			return -1;
		}
		/*************************************************
		*	Add Initialization code
		*	For Example:Initializing GL, GLUT GLEW, SDL, FMOD etc..
		***************************************************/
		return 0;
	}

	void MainApplication::onDraw() {
		SDL_GL_SwapWindow(_window);
	}

	void MainApplication::initGL() {
		/*************************************************
		*	Add GL Initialization code
		*	
		***************************************************/
	}

	int MainApplication::onStart() {
		/*************************************************
		*	Code that runs on the start of the application
		*	Note: runs before the current Application State ->onStart() is called
		***************************************************/
		return 0;
	}

	void MainApplication::onClose() {
		/*************************************************
		*	Code that runs on the close of the application
		*	Note: runs after the current Application State ->onClose() is called
		***************************************************/
	}
}