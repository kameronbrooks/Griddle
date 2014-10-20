#ifndef GRIDDLE_GUI
#define GRIDDLE_GUI
#include "includes.h"
#include "blocks.h"
#include <vector>"
#include "grid.h"
#include "grid_model.h"
#include "giggle\gui\giggle_gui.h"
#include "giggle\gui\giggle_window.h"

namespace Griddle {
	class GriddleUI {
		

	public:
		struct BlockCoords {
			int x, y, z;
			BlockCoords() { x = 0, y = 0, z = 0; }
			BlockCoords(int x, int y, int z) { this->x = x, this->y = y, this->z = z; }

			bool operator==(BlockCoords b);
		};
		glgl::GUI* _gui;
		GriddleUI(gaf::ApplicationState* appState);

		void setGrid(grid::StandardGrid* grid);
		void setGridModel(GridModel* model);
		void setRenderState(glgl::RenderState* state);
		void setFrustum(bool* frustum);
		void setCamera(glgl::Camera* camera);
		

		Blocks::Block getCurrentBlockType();
		void setCurrentBlockType(Blocks::Block block);

		BlockCoords getMouseOverCoords();
		void setMouseOverCoords(int x, int y, int z);
		void setMouseOverCoords(BlockCoords coords);

		int getActiveLayer();

		glm::vec3 GriddleUI::findMouseCoords(float mx, float my, int row);

		void addCoordsToSelection(int x, int y, int z);
		void addCoordsToSelection(BlockCoords coords);
		std::vector <BlockCoords>& getSelectedBlockCoords();
		void clearSelection();

		void handleMouseDown(float x, float y, int button);
		void handleMouseUp(float x, float y, int button);
		void handleMouseMove(float x, float y);
		void handleMouseWheel(float x, float y, int dir);
		void handleKeyUp(int keycode);
		void handleKeyDown(int keycode);

		void drawActiveBlockSprite(float x, float y, float z, float size);

		void setBlockType(int x, int y, int z, int type = -1);
	private:
		std::vector<BlockCoords> _selectedBlockCoordsList;
		BlockCoords _mouseOverCoords;
		Blocks::Block _currentBlockType;
		grid::StandardGrid* _grid;
		GridModel* _gridModel;
		bool* _frustum;
		glgl::RenderState* _pipeline;
		glgl::Camera* _camera;
		gaf::ApplicationState* _appState;
		int _activeLayer;
		int _currentBlockID;
		float _mouseScreenCoords[2];
		bool _isLMouseDown, _isRMouseDown;

		
	};
}

#endif