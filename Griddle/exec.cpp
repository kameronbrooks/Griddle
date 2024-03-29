#include "App.h"
#include "Utility.h"
#include "grid.h"
#include "blocks.h"
#include "grid_model.h"
#include "griddle_ui.h"
#include "FileDefLoader.h"
#include <sstream>

namespace Griddle {
	
	class InitialState : public gaf::ApplicationState, public gaf::iEventHandler {
		
		glgl::RenderState _pipeline;
		glgl::Camera* _camera;
		glgl::ShaderProgram _standardShader;
		glgl::Texture _textureGrass;
		glgl::Texture _textureAtlas;
		grid::StandardGrid* _grid;
		Griddle::GriddleUI* _ui;
		GridModel* _gridModel;
		float _maxOrthographicZoom;
		float rotation;
		bool _frustum;
		float _orthographicZoom;
		float _textureScale;
		int activeLayer;

		TTF_Font* _screenFont;
		glgl::TextRenderer* _textRenderer;

		void initGL() {
			glClearColor(0, 0, 0, 1);
			
			glEnable(GL_DEPTH_TEST);

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glEnable(GL_COLOR_MATERIAL);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glViewport(0, 0, _parentApp->getWidth(), _parentApp->getHeight());
			
			
		}
		void initTextures() {

		}

		void initModels() {
			
			glgl::ObjLoader* objLoader = new glgl::ObjLoader();

			delete objLoader;

		}
		void initShaders() {
			glgl::Shader* frag = new glgl::Shader(glgl::ShaderManager::FRAGMENT, "shaders/standard/frag_shader.txt");
			glgl::Shader* vert = new glgl::Shader(glgl::ShaderManager::VERTEX, "shaders/standard/vertex_shader.txt");

			_standardShader.create();
			_standardShader.attachShader(vert);
			_standardShader.attachShader(frag);
			_standardShader.link();
			_standardShader.use();

			delete frag; delete vert;
		}
		void initBlocks() {
			FileDefLoader loader;
			loader.parseFile("config/block_defs.txt");
			
			_textureAtlas.getHandle() = BlockTextures::getInstance()->build();
		}

		void initHUD() {
			_screenFont = TTF_OpenFont("fonts/arial.ttf", 16);
			if (_screenFont == NULL) {
				std::cout << TTF_GetError() << std::endl;
			}

			_ui->setCurrentBlockType(Blocks::getInstance()->getBlock(4));
		}

		void initGrid() {
			
			_grid->setAllBlockType(0);
			/*
			int passes = 70;
			for (int i = 0; i < _gridModel->_width; i++) {
				for (int j = 0; j < _gridModel->_height; j++) {
					_grid->setBlockType(i, j, 0, 5);
					_grid->setBlockType(i, j, 1, 0);
					_grid->setBlockType(i, j, 2, 0);
					

				}
			}
			*/
			
			_grid->load("saves/saved_grid.gcf", true);
			_gridModel->build(_grid);
			
		}

	public:
		InitialState(gaf::Application* app) : ApplicationState(app) {
			this->_eventHandler = this;
			_textRenderer = new MySDLTextRenderer();

			this->_eventHandler = this;
			_camera = new glgl::Camera();

			_camera->setPos(glm::vec3(45, 45, 45));
			_camera->setTarg(glm::vec3(0, 0, 0));
			_camera->setVert(glm::vec3(0, 1, 0));
			
			
				
			_maxOrthographicZoom = 200;
			rotation = 0;
			_frustum = false;
			_orthographicZoom = 60;
			_textureScale = 64.0f / 1024.0f;

			_grid = new grid::StandardGrid(32, 32, 3);
			_grid->allocate();
			
			_gridModel = new GridModel(32, 32, 3, 0, 0, _textureScale, glgl::StaticModel::VERTICES | glgl::StaticModel::COLORS);

			_ui = new GriddleUI(this);
			_ui->setCamera(_camera);
			_ui->setFrustum(&_frustum);
			_ui->setGrid(_grid);
			_ui->setGridModel(_gridModel);
			_ui->setRenderState(&_pipeline);

			BlockTextures::newInstance(64.0f, 1024.0f);
			
		}
		void InitialState::onStart() {
			initBlocks();
			initGL();
			initTextures();
			initModels();
			initShaders();
			initGrid();
			initHUD();

			
		}

		void changePerspective() {
			_frustum = !_frustum;
			if (_frustum) {
				glm::vec3 newPos = _camera->getTarg();
				_camera->setPos(newPos + glm::vec3(20,20, 20));
			}
			else {
				glm::vec3 newPos = _camera->getTarg();
				_camera->setPos(newPos + glm::vec3(45, 45, 45));
			}
		}
		void InitialState::onClose() {
			
		}

		void drawScreenText() {
			std::ostringstream activeLayerStream;
			std::ostringstream posStream;
			GriddleUI::BlockCoords block = _ui->getMouseOverCoords();
			activeLayerStream << "Active Layer: " << _ui->getActiveLayer();
			posStream << "[" << block.x << ", " << block.y << ", " << block.z << "]";
			this->_textRenderer->renderText("Griddle 0.4", _screenFont, 255, 255, 255, 0, 0, -1);
			this->_textRenderer->renderText(activeLayerStream.str(), _screenFont, 255, 255, 255, 0, (float)_parentApp->getHeight()-18, -1);
			this->_textRenderer->renderText(posStream.str(), _screenFont, 255, 255, 255, 0, (float)_parentApp->getHeight() - 40, -1);

			if (this->_ui->consoleActive()) {
				std::ostringstream consoleStream;
				consoleStream << ">> " << this->_ui->getConsoleLine();
				this->_textRenderer->renderText(consoleStream.str(), _screenFont, 255, 255, 0, 0, 20, -1);
			}
		}
		void drawUI() {
			_textureAtlas.bind();
			_ui->drawActiveBlockSprite((float)_parentApp->getWidth() - 70, 10, -1, 60);
			drawScreenText();
		}
		
		void InitialState::onDraw() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			//glViewport(0, 0, _parentApp->getWidth(), _parentApp->getHeight());
			_pipeline.matrices().matrixMode(glgl::Matrices::PROJECTION_MATRIX);
			_pipeline.matrices().loadIdentity();
			if (_frustum) {
				_pipeline.matrices().perspective(45.0, (float)_parentApp->getWidth() / (float)_parentApp->getHeight(), 1, 1000);
			}
			else {
				
				_pipeline.matrices().ortho(-((int)(float)_parentApp->getWidth() / _orthographicZoom), ((int)(float)_parentApp->getWidth() / _orthographicZoom), -((int)(float)_parentApp->getHeight() / _orthographicZoom), ((int)(float)_parentApp->getHeight() / _orthographicZoom), 0.1, 100);
			}
			
			
			_pipeline.matrices().matrixMode(glgl::Matrices::MODEL_MATRIX);
			_pipeline.matrices().loadIdentity();
			_camera->look(&(_pipeline.matrices()));
			

			_pipeline.useProgram(_standardShader.getHandle());
			
			
			_standardShader.use();
			//_standardShader.uniform3f("lightPos", this->_ui->getMouseOverCoords().x, this->_ui->getMouseOverCoords().z, this->_ui->getMouseOverCoords().y);
			_standardShader.uniform1f("use_texture", 1);
			_pipeline.matrices().sendToShader(_standardShader);
			_textureAtlas.bind();
			glColor4f(1, 1, 1, 1);
			
			_gridModel->draw(&_pipeline);
			
			_standardShader.uniform1f("use_texture", 0);
			//glDisable(GL_DEPTH_TEST);
			glLineWidth(2);
			_gridModel->drawWireFrames(&_pipeline);
			
			
			
			
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
			//								HUD
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
			/*
			glDisable(GL_DEPTH_TEST);

			_pipeline.matrices().matrixMode(glgl::Matrices::PROJECTION_MATRIX);
			_pipeline.matrices().loadIdentity();
			int zoom = 2;
			
			_pipeline.matrices().ortho(-100,100,100,-100,1,10);
			
			_pipeline.matrices().matrixMode(glgl::Matrices::MODEL_MATRIX);
			_pipeline.matrices().loadIdentity();
			_pipeline.matrices().lookat(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

			_pipeline.matrices().sendToShader(_standardShader);
			_textureAtlas.bind();
			glBegin(GL_QUADS);
			
			glTexCoord2d(0, 0); glColor4f(1, 1, 1, 1);
			glVertex3f(-100, -100, -2);
			glTexCoord2d(1, 0); glColor4f(1, 1, 1, 1);
			glVertex3f(100, -100, -2);
			glTexCoord2d(1, 1); glColor4f(1, 1, 1, 1);
			glVertex3f(100,100, -2);
			glTexCoord2d(0, 1); glColor4f(1, 1, 1, 1);
			glVertex3f(-100, 100, -2);
			

			

			glEnd();
			*/

			
			drawHUD();
			
		}

		void drawHUD() {
			/*
			_pipeline.matrices().matrixMode(glgl::Matrices::PROJECTION_MATRIX);
			_pipeline.matrices().pushMatrix();
			_pipeline.matrices().loadIdentity();
			_pipeline.matrices().ortho(-((int)(float)_parentApp->getWidth() / _orthographicZoom), ((int)(float)_parentApp->getWidth() / _orthographicZoom), -((int)(float)_parentApp->getHeight() / _orthographicZoom), ((int)(float)_parentApp->getHeight() / _orthographicZoom), 0.1, 100);

			_pipeline.matrices().matrixMode(glgl::Matrices::MODEL_MATRIX);
			_pipeline.matrices().pushMatrix();
			_pipeline.matrices().loadIdentity();
			_standardShader.uniform1f("use_texture", 1);
			_pipeline.matrices().sendToShader(_standardShader);
			drawScreenText();
			_pipeline.matrices().popMatrix();
			_pipeline.matrices().matrixMode(glgl::Matrices::PROJECTION_MATRIX);
			_pipeline.matrices().popMatrix();
			*/
			_pipeline.matrices().pushMatrix(glgl::Matrices::MODEL_VIEW_MATRIX | glgl::Matrices::PROJECTION_MATRIX);
			_pipeline.matrices().matrixMode(glgl::Matrices::PROJECTION_MATRIX);
			_pipeline.matrices().loadIdentity();
			_pipeline.matrices().ortho(0, ((int)(float)_parentApp->getWidth()), 0, ((int)(float)_parentApp->getHeight()), 0.1, 10);
			_standardShader.uniform1f("use_texture", 1);
			_pipeline.matrices().matrixMode(glgl::Matrices::VIEW_MATRIX);
			_pipeline.matrices().loadIdentity();
			_pipeline.matrices().matrixMode(glgl::Matrices::MODEL_MATRIX);
			_pipeline.matrices().loadIdentity();
			_pipeline.matrices().sendToShader(_standardShader);
			drawUI();
			_pipeline.matrices().popMatrix(glgl::Matrices::MODEL_VIEW_MATRIX | glgl::Matrices::PROJECTION_MATRIX);
			

		}
		void InitialState::onUpdate(double delta) {
			updateKeys(delta);
		}

		glm::vec3 findMouseCoords(float mx, float my, int row) {
			
			if (_frustum) {
				glm::vec3 cameraPos = _camera->getPos();
				float x = mx;
				float y = (float)this->getParent()->getHeight() - my;
				float z = 1;
				float depth = ((float)row) + 0.85f;
				
				glm::vec3 rayWorld = glm::unProject(glm::vec3(x, y, z), _pipeline.matrices().getModelViewMatrix(), _pipeline.matrices().getProjectionMatrix(), glm::vec4(0, 0, _parentApp->getWidth(), _parentApp->getHeight()));
				
				rayWorld = glm::normalize(rayWorld - cameraPos);

				float t = (depth - cameraPos.y) / (rayWorld.y);
				rayWorld = _camera->getPos() + rayWorld * t;
				
				return rayWorld;
			}
			else {
				glm::vec3 cameraPos = _camera->getPos();
				float x = mx;
				float y = (float)this->getParent()->getHeight() - my;
				float z = 0;
				float depth = ((float)row) + 0.5f;
				


				glm::vec3 rayWorld = glm::unProject(glm::vec3(x, y, z), _pipeline.matrices().getModelViewMatrix(), _pipeline.matrices().getProjectionMatrix(), glm::vec4(0, 0, _parentApp->getWidth(), _parentApp->getHeight()));
				glm::vec3 dir = glm::vec3(-1, -1, -1);
				dir = glm::normalize(dir);

				float t = (depth - rayWorld.y) / (dir.y);
				rayWorld = rayWorld + dir * t;
				
				return rayWorld;
			}
			
		}

		
		void updateKeys(double delta) {
			if (this->_ui->consoleActive())return;
			const Uint8* keys = SDL_GetKeyboardState(NULL);

			if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {

				glm::vec3 p = _camera->getPos();
				glm::vec3 t = _camera->getTarg();
				glm::vec3 d = glm::vec3(-1, 0, -1);

				d = d * 10.0f* (float)delta;
				_camera->setPos(p + d);
				_camera->setTarg(t + d);
				
			}
			if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
				glm::vec3 p = _camera->getPos();
				glm::vec3 t = _camera->getTarg();
				glm::vec3 d = glm::vec3(1, 0, 1);

				d = d * 10.0f* (float)delta;
				_camera->setPos(p + d);
				_camera->setTarg(t + d);
				
			}
			if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {

				glm::vec3 p = _camera->getPos();
				glm::vec3 t = _camera->getTarg();
				glm::vec3 d = glm::vec3(-1, 0, 1);

				d = d * 10.0f* (float)delta;
				_camera->setPos(p + d);
				_camera->setTarg(t + d);

			}
			if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
				glm::vec3 p = _camera->getPos();
				glm::vec3 t = _camera->getTarg();
				glm::vec3 d = glm::vec3(1, 0, -1);

				d = d * 10.0f* (float)delta;
				_camera->setPos(p + d);
				_camera->setTarg(t + d);

			}
			if (keys[SDL_SCANCODE_Z]) {
				if (_frustum) {
					glm::vec3 p = _camera->getPos();
					glm::vec3 t = _camera->getTarg();
					glm::vec3 d = glm::vec3(0, -1, 0);

					d = d * 10.0f* (float)delta;
					_camera->setPos(p + d);
					_camera->setTarg(t + d);
				}
				else {
					if (_orthographicZoom<_maxOrthographicZoom)_orthographicZoom += 0.5;
				}
				
			}
			if (keys[SDL_SCANCODE_X]) {
				if (_frustum) {
					glm::vec3 p = _camera->getPos();
					glm::vec3 t = _camera->getTarg();
					glm::vec3 d = glm::vec3(0, 1, 0);

					d = d * 10.0f* (float)delta;
					_camera->setPos(p + d);
					_camera->setTarg(t + d);
				}
				else {
					if (_orthographicZoom>30)_orthographicZoom -= 0.5;
					
				}

			}
			if (keys[SDL_SCANCODE_PERIOD]) {
				

			}
			if (keys[SDL_SCANCODE_C]) {
				

			}
			else if (keys[SDL_SCANCODE_COMMA]) {

			}
			if (keys[SDL_SCANCODE_0]) {
				_camera->setTarg(glm::vec3(0, 0, 0));
				_camera->setPos(glm::vec3(45, 45, 45));
				_orthographicZoom = 45;

			}
			
			
		}
		void InitialState::onPause() {

		}
		void InitialState::handleEvent(gaf::ApplicationState* appState, void* e) {

			SDL_Event* Event = ((SDL_Event*)e);

			if (Event->type == SDL_MOUSEMOTION) {
				_ui->handleMouseMove(Event->motion.x, Event->motion.y);
			}
			else if (Event->type == SDL_MOUSEBUTTONDOWN) {
				_ui->handleMouseDown(Event->button.x, Event->button.y, Event->button.button);
			}
			else if (Event->type == SDL_MOUSEBUTTONUP) {
				_ui->handleMouseUp(Event->button.x, Event->button.y, Event->button.button);
			}
			else if (Event->type == SDL_QUIT) {
				_parentApp->requestClose();
			}
			else if (Event->type == SDL_KEYDOWN) {
				_ui->handleKeyDown(Event->key.keysym.sym);
			}
			else if (Event->type == SDL_KEYUP) {
				if (this->_ui->consoleActive()) {
					_ui->handleKeyUp(Event->key.keysym.sym);
					
					return;
				}
				if (Event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					_parentApp->requestClose();
				}
				else if (Event->key.keysym.scancode == SDL_SCANCODE_9) {
					changePerspective();
				}
				else if (Event->key.keysym.scancode == SDL_SCANCODE_1) {
					_grid->save("saved_grid.gcf", true);

				}
				else if (Event->key.keysym.scancode == SDL_SCANCODE_2) {
					_grid->load("saved_grid.gcf", true);
					_gridModel->build(_grid);

				}
				else {
					_ui->handleKeyUp(Event->key.keysym.scancode);
				}

			}
			else if (Event->type == SDL_MOUSEMOTION) {
				_ui->handleMouseMove(Event->motion.x, Event->motion.y);
			}
			else if (Event->type == SDL_MOUSEWHEEL) {
				_ui->handleMouseWheel(Event->motion.x, Event->motion.y, Event->wheel.y);
				
			}
		}
		friend class GriddleUI;

	};

	
	
}

int main(int argc, char* args[])
{
	gaf::Application* app = new Griddle::MainApplication();
	app->setWindowSize(700, 700);
	app->setWindowPos(100, 100);
	gaf::ApplicationState* initialState = new Griddle::InitialState(app);
	
	app->setInitialState(initialState);
	app->setEventPoller(new Griddle::MySDLEventPoller());
	app->setTimer(new Griddle::MySDLTimer());

	
	return app->run();

}