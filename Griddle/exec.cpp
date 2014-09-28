#include "App.h"
#include "Utility.h"
#include "grid.h"
#include "blocks.h"
#include "grid_model.h"

namespace Griddle {
	
	class InitialState : public gaf::ApplicationState, public gaf::iEventHandler {
		glgl::RenderState _pipeline;
		glgl::Camera* _camera;
		glgl::ShaderProgram _standardShader;
		glgl::Texture _textureGrass;
		glgl::Texture _textureAtlas;
		glgl::DataBuffer* _blockDataBuffer[3];
		glgl::StaticModel* _blockModel;
		glgl::DynamicModel* _chunkModel;
		GridModel* _gridModel;
		float _maxOrthographicZoom;
		float rotation;
		bool _frustum;
		float _orthographicZoom;
		float _textureScale;

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

			objLoader->loadFile("models/block.obj", _blockModel);

			delete objLoader;
			

			_blockDataBuffer[0]->setData(&_blockModel->_vertices[0], _blockModel->_vertices.size(), 3, GL_FLOAT, glgl::DYNAMIC_DRAW);
			_blockDataBuffer[1]->setData(&_blockModel->_uvs[0], _blockModel->_uvs.size(), 3, GL_FLOAT, glgl::DYNAMIC_DRAW);
			_blockDataBuffer[2]->setData(&_blockModel->_normals[0], _blockModel->_normals.size(), 3, GL_FLOAT, glgl::DYNAMIC_DRAW);

			
			_gridModel->build();
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
			int id = 0;
			Blocks::getInstance()->registerBlockType(id++, "air", 0, 0);
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "water", 1, 0, 1, 1, 1, 0.5), "textures/water.png");
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "rock block", 2, 0), "textures/rock block.png");
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "rock grassy block", 2, 0), "textures/rock grassy block.png");
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "dirt", 0, 1), "textures/dirt.png");
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "grass", 1, 1), "textures/grass.png");

			
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "grass patchy", 2, 1), "textures/grass patchy.png");
			BlockTextures::getInstance()->loadTexture(Blocks::getInstance()->registerBlockType(id++, "grass patchy 1", 3, 1), "textures/grass patchy1.png");

			Blocks::Block stoneBrick;
			stoneBrick._id = id++;
			stoneBrick._name = "stone brick";
			stoneBrick.addState(0, 4, 1).addState(1, 5, 1).addState(2, 6, 1);
			Blocks::getInstance()->registerBlockType(stoneBrick);
			BlockTextures::getInstance()->loadTexture(stoneBrick, 0, "textures/stone brick block.png");
			BlockTextures::getInstance()->loadTexture(stoneBrick, 1, "textures/stone brick block_d0.png");
			BlockTextures::getInstance()->loadTexture(stoneBrick, 2, "textures/stone brick block_d1.png");


			_textureAtlas.getHandle() = BlockTextures::getInstance()->build();
		}

		void initGrid() {
			std::cout << Blocks::getInstance()->getBlock(8)._stateList[1]._xOffset << std::endl;
			int passes = 70;
			for (int i = 0; i < _gridModel->_width; i++) {
				for (int j = 0; j < _gridModel->_height; j++) {
					_gridModel->updateBlock(i, j, 0, 1);
					_gridModel->updateBlock(i, j, 1, 0);
					
				}
			}
			for (int h = 0; h < passes; h++) {
				int islandSize = rand() % 10;
				int randX = rand() % (_gridModel->_width);
				int randY = rand() % (_gridModel->_height);
				for (int i = randX-islandSize / 2; i < randX + islandSize / 2; i++) {
					for (int j = randY -islandSize / 2; j < randY+islandSize / 2; j++) {
						if (i < 0 || i >= _gridModel->_width || j < 0 || j >= _gridModel->_height)continue;
						int blockType = rand() % 8;
						if (blockType < 3)blockType = 3;
						
						_gridModel->updateBlock(i, j, 0, blockType);
						

					}
				}
			}


			
			
		}

	public:
		InitialState(gaf::Application* app) : ApplicationState(app) {
			this->_eventHandler = this;
			_camera = new glgl::Camera();

			_camera->setPos(glm::vec3(45, 45, 45));
			_camera->setTarg(glm::vec3(0, 0, 0));
			_camera->setVert(glm::vec3(0, 1, 0));
			
			_blockModel = new glgl::StaticModel();
			_chunkModel = new glgl::DynamicModel(glgl::StaticModel::VERTICES | glgl::StaticModel::COLORS);
				
			_maxOrthographicZoom = 200;
			rotation = 0;
			_frustum = false;
			_orthographicZoom = 60;
			_textureScale = 64.0f / 1024.0f;

			_gridModel = new GridModel(32, 32, 3, 0, 0, _textureScale, glgl::StaticModel::VERTICES | glgl::StaticModel::COLORS);
			BlockTextures::newInstance(64.0f, 1024.0f);
			_blockDataBuffer[0] = new glgl::ServerDataBuffer();
			_blockDataBuffer[1] = new glgl::ServerDataBuffer();
			_blockDataBuffer[2] = new glgl::ServerDataBuffer();
		}
		void InitialState::onStart() {
			initBlocks();
			initGL();
			initTextures();
			initModels();
			initShaders();
			initGrid();
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
			_standardShader.uniform1f("use_texture", 1);
			_pipeline.matrices().sendToShader(_standardShader);
			_textureAtlas.bind();
			glColor4f(1, 1, 1, 1);
			
			_gridModel->draw(&_pipeline);
			
			_standardShader.uniform1f("use_texture", 0);
			glPointSize(5);
			glBegin(GL_POINTS);
			glColor4f(1, 1, 1, 1);
			glTexCoord2f(0,0); glVertex3f(0, 0, 0);
			glTexCoord2f(0.5, 1); glVertex3f(4, 0, 0);
			glTexCoord2f(1, 0); glVertex3f(2, 4, 0);

			
			glVertex3f(_camera->getPos().x, _camera->getPos().y, _camera->getPos().z);
			glEnd();
			
			
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
			
				
			
		}
		void InitialState::onUpdate(double delta) {
			updateKeys(delta);
		}

		glm::vec3 findMouseCoords(float mx, float my) {
			
			if (_frustum) {
				glm::vec3 cameraPos = _camera->getPos();
				float x = mx;
				float y = (float)this->getParent()->getHeight() - my;
				float z = 1;
				float depth = 0.85f;
				
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
				float depth = 0.5f;
				


				glm::vec3 rayWorld = glm::unProject(glm::vec3(x, y, z), _pipeline.matrices().getModelViewMatrix(), _pipeline.matrices().getProjectionMatrix(), glm::vec4(0, 0, _parentApp->getWidth(), _parentApp->getHeight()));
				glm::vec3 dir = glm::vec3(-1, -1, -1);
				dir = glm::normalize(dir);

				float t = (depth - rayWorld.y) / (dir.y);
				rayWorld = rayWorld + dir * t;
				
				return rayWorld;
			}
			
		}

		
		void updateKeys(double delta) {
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

			}
			
		}
		void InitialState::onPause() {

		}
		void InitialState::handleEvent(gaf::ApplicationState* appState, void* e) {
			SDL_Event* Event = ((SDL_Event*)e);
			if (Event->type == SDL_QUIT) {
				_parentApp->requestClose();
			}
			else if (Event->type == SDL_KEYDOWN) {

			}
			else if (Event->type == SDL_KEYUP) {
				if (Event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					_parentApp->requestClose();
				}
				else if (Event->key.keysym.scancode == SDL_SCANCODE_9) {
					changePerspective();
				}

			}
			else if (Event->type == SDL_MOUSEMOTION) {
				glm::vec3 mousePos3D = findMouseCoords(Event->motion.x, Event->motion.y);
				int blockPos[3];
				blockPos[0] = int((mousePos3D.x - _gridModel->_x));
				blockPos[1] = int((mousePos3D.z - _gridModel->_z));
				blockPos[2] = int((mousePos3D.y - _gridModel->_y));
				//std::cout << "selecting block: " << blockPos[0] << ", " << blockPos[1] << ", " << blockPos[2] << std::endl;
				_gridModel->setSelectedBlock(blockPos[0], blockPos[1], blockPos[2]);
			}
		}

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