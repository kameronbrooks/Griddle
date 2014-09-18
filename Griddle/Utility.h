#ifndef MAIN_APP_UTILS_H
#define MAIN_APP_UTILS_H

#include "includes.h"

namespace Griddle {
	class MySDLTimer : public gaf::iTimer {
	public:
		unsigned int getTicks() {
			return SDL_GetTicks();
		}
	};

	class MySDLEventPoller : public gaf::iEventPoller {
		//SDL_Event e;
	public:
		void MySDLEventPoller::pollEvents(gaf::Application* app) {
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				app->handleEvent(&e);
			}
		}
	};
	class ImageLoader {
	public:
		GLuint createTexture(std::string file) {
			std::cout << "starting image load" << std::endl;
			SDL_Surface* image = NULL;
			image = IMG_Load(file.c_str());
			std::cout << "loaded image at:" << image << std::endl;
			if (image == NULL) {
				std::cout << "no image" << std::endl;
				return 0;
			}
			std::cout << SDL_GetError() << std::endl;
			GLuint texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

			SDL_FreeSurface(image);
			return texture;

		}
	};
	class TextureAtlasBuilder {
	private:
		int _width, _height;
		SDL_Surface* _surface;
	public:
		TextureAtlasBuilder() {
			_width = 1024;
			_height = 1024;

			uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x000000ff;
#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0xff000000;
#endif
			_surface = SDL_CreateRGBSurface(0, _width, _height, 32, rmask, gmask, bmask, amask);
			
		}
		TextureAtlasBuilder(int width, int height) {
			_width = width;
			_height = height;

			uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			rmask = 0xff000000;
			gmask = 0x00ff0000;
			bmask = 0x0000ff00;
			amask = 0x000000ff;
#else
			rmask = 0x000000ff;
			gmask = 0x0000ff00;
			bmask = 0x00ff0000;
			amask = 0xff000000;
#endif
			_surface = SDL_CreateRGBSurface(0, _width, _height, 32, rmask, gmask, bmask, amask);
		}
		~TextureAtlasBuilder() {
			if (_surface) {
				SDL_FreeSurface(_surface);
			}
		}
		bool loadImage(std::string filename, int x, int y) {
			if (!_surface)return false;
			SDL_Surface* image = NULL;
			image = IMG_Load(filename.c_str());
			std::cout << "loaded image at:" << image << std::endl;
			if (image == NULL) {
				std::cout << "no image" << std::endl;
				return false;
			}
			SDL_Rect sRect,dRect;
			sRect.x = 0;
			sRect.y = 0;
			sRect.w = image->w;
			sRect.h = image->h;

			dRect.x = x;
			dRect.y = y;
			dRect.w = image->w;
			dRect.h = image->h;

			if (SDL_BlitSurface(image, &sRect, _surface, &dRect) == 0) return true;
			else return false;

		}
		GLuint createTextureAtlas() {
			
			GLuint texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _surface->w, _surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, _surface->pixels);

			SDL_FreeSurface(_surface);
			return texture;
		}
	};

	class MySDLTextRenderer : public glgl::TextRenderer {
	public:
		int MySDLTextRenderer::init() {
			return 0;
		}
		void MySDLTextRenderer::renderText(std::string text, void* font, GLubyte r, GLubyte g, GLubyte b, GLdouble x, GLdouble y, GLdouble z) {
			SDL_Color color = { r, g, b, 255 };

			SDL_Surface* message = TTF_RenderText_Blended((TTF_Font*)font, text.c_str(), color);

			GLuint texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, message->w, message->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, message->pixels);

			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);

			glTexCoord2d(0, 1); glVertex3d(x, y, z);
			glTexCoord2d(1, 1); glVertex3d(x + message->w, y, z);
			glTexCoord2d(1, 0); glVertex3d(x + message->w, y + message->h, z);
			glTexCoord2d(0, 0); glVertex3d(x, y + message->h, z);
			glEnd();

			glDeleteTextures(1, &texture);
			SDL_FreeSurface(message);
		}
	};
}

#endif