#ifndef MAIN_APP_H
#define MAIN_APP_H

#include "includes.h"


namespace Griddle {
	class MainApplication : public gaf::Application {
	protected:
		SDL_Window* _window;
		SDL_GLContext _glContext;
	public:
		MainApplication::MainApplication();
		int MainApplication::init();
		void MainApplication::onDraw();
		int MainApplication::onStart();
		void initGL();
		void MainApplication::onClose();
	};
}




#endif