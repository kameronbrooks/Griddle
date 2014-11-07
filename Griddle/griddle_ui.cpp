#include "griddle_ui.h"

namespace Griddle {

	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				STATES
	=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


	class DataState : public GriddleUI::State {
		int data;
		int index;
		std::string word;
	public:
		DataState(GriddleUI* parent) : State(parent) {
			word = "0";
			data = 0;
			index = 0;
		}
		void begin() {
			(*_consoleActive) = true;
			(*_consoleLine) = "0";
			
			data = 0;
			index = 0;
		}
		void handleRightMouseDown(float x, float y) {

		}
		void handleRightMouseUp(float x, float y) {
			_parent->setState(NULL);
		}
		void handleLeftMouseDown(float x, float y) {

		}
		void handleLeftMouseUp(float x, float y) {

		}
		void handleMouseMove(float x, float y) {

		}
		void handleMouseWheel(float x, float y, int dir) {

		}
		void handleKeyUp(int keycode) {
			if (keycode == SDLK_RETURN) {
				std::cout << data << std::endl;
				_parent->setBlockData(_mouseOverCoords->x, _mouseOverCoords->y, _mouseOverCoords->z, data);
				(*_consoleActive) = false;
				_parent->setState(NULL);
			}
			else {
				int value = 0;
				if (keycode == SDL_SCANCODE_0)value = 0;
				else if (keycode == SDLK_1)value = 1;
				else if (keycode == SDLK_2)value = 2;
				else if (keycode == SDLK_3)value = 3;
				else if (keycode == SDLK_4)value = 4;
				else if (keycode == SDLK_5)value = 5;
				else if (keycode == SDLK_6)value = 6;
				else if (keycode == SDLK_7)value = 7;
				else if (keycode == SDLK_8)value = 8;
				else if (keycode == SDLK_9)value = 9;
				data = data * 10 + value;
				
				(*_consoleLine) = std::to_string(data);

			}
		}
		void handleKeyDown(int keycode) {

		}
	};

	class SaveState : public GriddleUI::State {
		
		
		std::string word;
	public:
		SaveState(GriddleUI* parent) : State(parent) {
			word = "0";
			
		}
		void begin() {
			(*_consoleActive) = true;
			(*_consoleLine) = "saves/";

		}
		void handleRightMouseDown(float x, float y) {

		}
		void handleRightMouseUp(float x, float y) {
			_parent->setState(NULL);
		}
		void handleLeftMouseDown(float x, float y) {

		}
		void handleLeftMouseUp(float x, float y) {

		}
		void handleMouseMove(float x, float y) {

		}
		void handleMouseWheel(float x, float y, int dir) {

		}
		void handleKeyUp(int keycode) {
			if (keycode == SDLK_BACKSPACE) {
				(*_consoleLine) = (*_consoleLine).substr(0, (*_consoleLine).length() - 1);
			}
			else if (keycode == SDLK_RETURN) {
				if ((*_consoleLine).find(".gcf") == std::string::npos) {
					return;
				}
				_parent->getGrid()->save((*_consoleLine), true);
				(*_consoleActive) = false;
				_parent->setState(NULL);
			}
			else {
				(*_consoleLine) = (*_consoleLine) + (char)(keycode);
			}
		}
		void handleKeyDown(int keycode) {
			
		}
	};

	class LoadState : public GriddleUI::State {


		std::string word;
	public:
		LoadState(GriddleUI* parent) : State(parent) {
			word = "0";

		}
		void begin() {
			(*_consoleActive) = true;
			(*_consoleLine) = "saves/";

		}
		void handleRightMouseDown(float x, float y) {

		}
		void handleRightMouseUp(float x, float y) {
			_parent->setState(NULL);
		}
		void handleLeftMouseDown(float x, float y) {

		}
		void handleLeftMouseUp(float x, float y) {

		}
		void handleMouseMove(float x, float y) {

		}
		void handleMouseWheel(float x, float y, int dir) {

		}
		void handleKeyUp(int keycode) {
			if (keycode == SDLK_BACKSPACE) {
				(*_consoleLine) = (*_consoleLine).substr(0, (*_consoleLine).length() - 1);
			}
			else if (keycode == SDLK_RETURN) {
				if ((*_consoleLine).find(".gcf") == std::string::npos) {
					return;
				}
				_parent->getGrid()->load((*_consoleLine), true);
				_parent->getGridModel()->build(_parent->getGrid());
				(*_consoleActive) = false;
				_parent->setState(NULL);
			}
			else {
				(*_consoleLine) = (*_consoleLine) + (char)(keycode);
			}
		}
		void handleKeyDown(int keycode) {

		}
	};

	class HelpState : public GriddleUI::State {


		std::string word;
	public:
		HelpState(GriddleUI* parent) : State(parent) {
			word = "0";

		}
		void begin() {
			(*_consoleActive) = true;
			(*_consoleLine) = "help \n console: \n";

		}
		void handleRightMouseDown(float x, float y) {

		}
		void handleRightMouseUp(float x, float y) {
			_parent->setState(NULL);
		}
		void handleLeftMouseDown(float x, float y) {

		}
		void handleLeftMouseUp(float x, float y) {

		}
		void handleMouseMove(float x, float y) {

		}
		void handleMouseWheel(float x, float y, int dir) {

		}
		void handleKeyUp(int keycode) {
			
		}
		void handleKeyDown(int keycode) {

		}
	};

	


	/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				UI
	=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

	bool GriddleUI::BlockCoords::operator==(BlockCoords b) {
		return !(x != b.x || y != b.y || z != b.z);
	}
	GriddleUI::State::State(GriddleUI* parent) {
		this->_parent = parent;
		this->_isLMouseDown = &(parent->_isLMouseDown);
		this->_isRMouseDown = &(parent->_isRMouseDown);
		this->_grid = _parent->_grid;
		this->_gridModel = _parent->_gridModel;
		this->_currentBlockType = &(_parent->_currentBlockType);
		this->_mouseOverCoords = &(_parent->_mouseOverCoords);
		this->_selectedBlockCoordsList = &(_parent->_selectedBlockCoordsList);
		this->_consoleActive = &(_parent->_consoleActive);
		this->_consoleLine = &(_parent->_consoleLine);
	}
	GriddleUI::GriddleUI(gaf::ApplicationState* appState) {
		_grid = NULL;
		_gridModel = NULL;
		_appState = appState;
		_activeLayer = 0;
		_isLMouseDown = _isRMouseDown = false;
		_gui = glgl::GUI::getInstance();
		_activeState = NULL;
		_consoleActive = false;
		_consoleLine = "";

		_dataState = new DataState(this);
		_saveState = new SaveState(this);
		_loadState = new LoadState(this);
		_helpState = new HelpState(this);
		
	}
	void GriddleUI::setState(State* state) {
		_activeState = state;
		if(state!=NULL)state->begin();
	}

	void GriddleUI::setGrid(grid::StandardGrid* grid) {
		this->_grid = grid;

	}
	void GriddleUI::setGridModel(GridModel* model) {
		this->_gridModel = model;
	}
	void GriddleUI::setRenderState(glgl::RenderState* state) {
		this->_pipeline = state;
	}
	void GriddleUI::setFrustum(bool* frustum) {
		this->_frustum = frustum;
	}
	void GriddleUI::setCamera(glgl::Camera* camera) {
		this->_camera = camera;
	}

	grid::StandardGrid* GriddleUI::getGrid() {
		return _grid;
	}
	GridModel* GriddleUI::getGridModel() {
		return _gridModel;
	}

	Blocks::Block GriddleUI::getCurrentBlockType() {
		return this->_currentBlockType;
	}
	void GriddleUI::setCurrentBlockType(Blocks::Block block) {
		this->_currentBlockType = block;
	}

	GriddleUI::BlockCoords GriddleUI::getMouseOverCoords() {
		return this->_mouseOverCoords;
	}
	void GriddleUI::setMouseOverCoords(int x, int y, int z) {
		this->_mouseOverCoords.x = x;
		this->_mouseOverCoords.y = y;
		this->_mouseOverCoords.z = z;
		_gridModel->setMouseOverBlock(x, y, z);
	}
	void GriddleUI::setMouseOverCoords(BlockCoords coords) {
		this->_mouseOverCoords = coords;
		_gridModel->setMouseOverBlock(coords.x, coords.y, coords.z);
	}
	int GriddleUI::getActiveLayer() {
		return this->_activeLayer;
	}

	void GriddleUI::addCoordsToSelection(int x, int y, int z) {
		this->_selectedBlockCoordsList.push_back(BlockCoords(x, y, z));
	}
	void GriddleUI::addCoordsToSelection(BlockCoords coords) {
		this->_selectedBlockCoordsList.push_back(coords);
	}
	std::vector<GriddleUI::BlockCoords>& GriddleUI::getSelectedBlockCoords() {
		return this->_selectedBlockCoordsList;
	}
	void GriddleUI::clearSelection() {
		_selectedBlockCoordsList.clear();
	}

	void GriddleUI::drawActiveBlockSprite(float x, float y, float z, float size) {
		float u = this->_currentBlockType._stateList[0]._xOffset;
		float v = this->_currentBlockType._stateList[0]._yOffset;
		
		float baseSpriteSize = BlockTextures::getInstance()->getTextureToAtlasRatio();

		float u0 = u*baseSpriteSize;
		float v0 = v*baseSpriteSize;
		float u1 = u*baseSpriteSize + baseSpriteSize;
		float v1 = v*baseSpriteSize + baseSpriteSize;
		glBegin(GL_QUADS);

		glTexCoord2d(u1, v1); glColor4f(1, 1, 1, 1);
		glVertex3f(x, y, z);
		glTexCoord2d(u0, v1); glColor4f(1, 1, 1, 1);
		glVertex3f(x+size, y, z);
		glTexCoord2d(u0, v0); glColor4f(1, 1, 1, 1);
		glVertex3f(x+size, y+size, z);
		glTexCoord2d(u1, v0); glColor4f(1, 1, 1, 1);
		glVertex3f(x, y+size, z);
		glEnd();
	}

	glm::vec3 GriddleUI::findMouseCoords(float mx, float my, int row) {

		if ((*_frustum)) {
			glm::vec3 cameraPos = _camera->getPos();
			float x = mx;
			float y = (float)_appState->getParent()->getHeight() - my;
			float z = 1;
			float depth = ((float)row) + 0.85f;

			glm::vec3 rayWorld = glm::unProject(glm::vec3(x, y, z), _pipeline->matrices().getModelViewMatrix(), _pipeline->matrices().getProjectionMatrix(), glm::vec4(0, 0, _appState->getParent()->getWidth(), _appState->getParent()->getHeight()));

			rayWorld = glm::normalize(rayWorld - cameraPos);

			float t = (depth - cameraPos.y) / (rayWorld.y);
			rayWorld = _camera->getPos() + rayWorld * t;

			return rayWorld;
		}
		else {
			glm::vec3 cameraPos = _camera->getPos();
			float x = mx;
			float y = (float)_appState->getParent()->getHeight() - my;
			float z = 0;
			float depth = ((float)row) + 0.5f;



			glm::vec3 rayWorld = glm::unProject(glm::vec3(x, y, z), _pipeline->matrices().getModelViewMatrix(), _pipeline->matrices().getProjectionMatrix(), glm::vec4(0, 0, _appState->getParent()->getWidth(), _appState->getParent()->getHeight()));
			glm::vec3 dir = glm::vec3(-1, -1, -1);
			dir = glm::normalize(dir);

			float t = (depth - rayWorld.y) / (dir.y);
			rayWorld = rayWorld + dir * t;

			return rayWorld;
		}

	}

	void GriddleUI::setBlockType(int x, int y, int z, int type) {
		if (type < 0) type = this->_currentBlockType._id;
		if (x < 0 || x >= _grid->getWidth() ||
			y < 0 || y >= _grid->getHeight() ||
			z < 0 || z >= _grid->getDepth()) {
		}
		else {
			
			if (_grid->getData(x, y, z, 0) != type) {

				_grid->setBlockType(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, this->_currentBlockType._id);
				_gridModel->setBlockModel(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, this->_currentBlockType._stateList[0]._drawType);
				_gridModel->setBlockTexture(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, this->_currentBlockType._stateList[0]._xOffset, this->_currentBlockType._stateList[0]._yOffset);
			}
		}
		
	}
	void GriddleUI::setBlockData(int x, int y, int z, int data) {
		if (x < 0 || x >= _grid->getWidth() ||
			y < 0 || y >= _grid->getHeight() ||
			z < 0 || z >= _grid->getDepth()) {
		}
		else {
			unsigned short type = _grid->getData(x, y, z, 0);
			Blocks::Block blockType = Blocks::getInstance()->getBlock(type);
			std::cout << "seting data " << data << std::endl;
			if (_grid->getData(x, y, z, 1) != data) {
				std::cout << "data != " << data << std::endl;
				_grid->setBlockData(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, 0, (unsigned short)data);
				if (blockType._stateList.find(data) != blockType._stateList.end()) {
					std::cout << "state exists != " << data << std::endl;
					_gridModel->setBlockModel(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, blockType._stateList[data]._drawType);
					_gridModel->setBlockTexture(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, blockType._stateList[data]._xOffset, blockType._stateList[data]._yOffset);
				}
			}
		}
	}

	void GriddleUI::handleMouseDown(float x, float y, int button) {
		if (button == SDL_BUTTON_LEFT) {
			_isLMouseDown = true;
			if (_activeState) {
				_activeState->handleLeftMouseDown(x, y);
				return;
			}
			setBlockType(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z);
		}
		else if (button == SDL_BUTTON_RIGHT) {
			_isRMouseDown = true;
			if (_activeState) {
				_activeState->handleRightMouseDown(x, y);
				return;
			}

		}
		else {

		}
	}
	void GriddleUI::handleMouseUp(float x, float y, int button) {
		if (button == SDL_BUTTON_LEFT) {

			_isLMouseDown = false;
			if (_activeState) {
				_activeState->handleLeftMouseUp(x, y);
				return;
			}
			
		}
		else if (button == SDL_BUTTON_RIGHT) {
			_isRMouseDown = false;
			if (_activeState) {
				_activeState->handleRightMouseUp(x, y);
				return;
			}
			setState(_dataState);
		}
		else {

		}
	}

	void GriddleUI::handleMouseMove(float x, float y) {
		if (_activeState) {
			_activeState->handleMouseMove(x,y);
			return;
		}
		_mouseScreenCoords[0] = x;
		_mouseScreenCoords[1] = y;
		glm::vec3 mousePos3D = findMouseCoords(x, y, _activeLayer);
		int blockPos[3];
		blockPos[0] = int((mousePos3D.x - _gridModel->_x));
		blockPos[1] = int((mousePos3D.z - _gridModel->_z));
		blockPos[2] = int((mousePos3D.y - _gridModel->_y));
		
		setMouseOverCoords(BlockCoords(blockPos[0], blockPos[1], blockPos[2]));

		if (_isLMouseDown) {
			setBlockType(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z);
		}
	
	}
	void GriddleUI::handleMouseWheel(float x, float y, int dir) {
		if (_activeState) {
			_activeState->handleMouseWheel(x,y,dir);
			return;
		}
		if(dir > 0) {
			if (_currentBlockID < 255) {
				this->_currentBlockID += 1;
			}
		}
		else {
			if (_currentBlockID > 0) {
				this->_currentBlockID -= 1;
			}
		}
		_currentBlockType = Blocks::getInstance()->getBlock(_currentBlockID);
		
		
	}
	void GriddleUI::handleKeyUp(int keycode) {
		if (_activeState) {
			_activeState->handleKeyUp(keycode);
			return;
		}
	}
	void GriddleUI::handleKeyDown(int keycode) {
		if (_activeState) {
			_activeState->handleKeyDown(keycode);
			return;
		}
		if (_consoleActive) {
			if (keycode == SDLK_RETURN) {
				
				consoleCommand(_consoleLine);
				
			}
			else if (keycode == SDLK_BACKSPACE) {
				_consoleLine = _consoleLine.substr(0,_consoleLine.length() - 1);
			}
			else {
				_consoleLine = _consoleLine + (char)(keycode);
			}
			
			return;
		}
		if (keycode == SDLK_r) {
			if (_activeLayer < _grid->getDepth() - 1) {
				_activeLayer += 1;
				setMouseOverCoords(BlockCoords(this->_mouseOverCoords.x, this->_mouseOverCoords.y, this->_mouseOverCoords.z + 1));
			}
			

			_gridModel->setActiveLayer(_activeLayer);
		}
		else if (keycode == SDLK_f) {
			if (_activeLayer > 0) {
				_activeLayer -= 1;
				setMouseOverCoords(BlockCoords(this->_mouseOverCoords.x, this->_mouseOverCoords.y, this->_mouseOverCoords.z - 1));
			}
			

			_gridModel->setActiveLayer(_activeLayer);
		}
		else if (keycode == SDLK_TAB) {
			_gridModel->toggleActiveLayerGhosting();
		}
		else if (keycode == SDLK_c) {
			_consoleActive = !_consoleActive;
		}
		else {

		}
	}

	void GriddleUI::draw() {

	}
	std::string GriddleUI::getConsoleLine() {
		return _consoleLine;
	}
	bool GriddleUI::consoleActive() {
		return _consoleActive;
	}

	void GriddleUI::consoleCommand(std::string line) {
		if (line == "-s" || line == "-save" || line == "save") {
			setState(_saveState);
		}
		else if (line == "-l" || line == "-load" || line == "load") {
			setState(_loadState);
		}
		else if (line == "-h" || line == "-help" || line == "help") {
			setState(_helpState);
		}
		else {
			_consoleLine = "";
			_consoleActive = false;
		}
	}

	
}