#include "griddle_ui.h"

namespace Griddle {

	bool GriddleUI::BlockCoords::operator==(BlockCoords b) {
		return !(x != b.x || y != b.y || z != b.z);
	}
	GriddleUI::GriddleUI(gaf::ApplicationState* appState) {
		_grid = NULL;
		_gridModel = NULL;
		_appState = appState;
		_activeLayer = 0;
		_isLMouseDown = _isRMouseDown = false;
		_gui = glgl::GUI::getInstance();
		
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
				_gridModel->setBlockTexture(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z, this->_currentBlockType._stateList[0]._xOffset, this->_currentBlockType._stateList[0]._yOffset);
			}
		}
		
	}

	void GriddleUI::handleMouseDown(float x, float y, int button) {
		if (button == SDL_BUTTON_LEFT) {
			_isLMouseDown = true;
			setBlockType(_mouseOverCoords.x, _mouseOverCoords.y, _mouseOverCoords.z);
		}
		else if (button == SDL_BUTTON_RIGHT) {
			_isRMouseDown = true;
		}
		else {

		}
	}
	void GriddleUI::handleMouseUp(float x, float y, int button) {
		if (button == SDL_BUTTON_LEFT) {
			_isLMouseDown = false;
			
		}
		else if (button == SDL_BUTTON_RIGHT) {
			_isRMouseDown = false;
		}
		else {

		}
	}

	void GriddleUI::handleMouseMove(float x, float y) {
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

	}
	void GriddleUI::handleKeyDown(int keycode) {
		if (keycode == SDL_SCANCODE_CAPSLOCK) {
			if (_activeLayer < _grid->getDepth() - 1) {
				_activeLayer += 1;
				
			}
			glm::vec3 mousePos3D = findMouseCoords(_mouseScreenCoords[0], _mouseScreenCoords[1], _activeLayer);
			int blockPos[3];
			blockPos[0] = int((mousePos3D.x - _gridModel->_x));
			blockPos[1] = int((mousePos3D.z - _gridModel->_z));
			blockPos[2] = int((mousePos3D.y - _gridModel->_y));

			setMouseOverCoords(BlockCoords(blockPos[0], blockPos[1], blockPos[2]));

			_gridModel->setActiveLayer(_activeLayer);
		}
		else if (keycode == SDL_SCANCODE_LSHIFT) {
			if (_activeLayer > 0) {
				_activeLayer -= 1;
				
			}
			glm::vec3 mousePos3D = findMouseCoords(_mouseScreenCoords[0], _mouseScreenCoords[1], _activeLayer);
			int blockPos[3];
			blockPos[0] = int((mousePos3D.x - _gridModel->_x));
			blockPos[1] = int((mousePos3D.z - _gridModel->_z));
			blockPos[2] = int((mousePos3D.y - _gridModel->_y));

			setMouseOverCoords(BlockCoords(blockPos[0], blockPos[1], blockPos[2]));

			_gridModel->setActiveLayer(_activeLayer);
		}
		else {

		}
	}

	
}