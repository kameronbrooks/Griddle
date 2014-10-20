#ifndef GRID_MODEL_H
#define GRID_MODEL_H

#include "includes.h"
#include "blocks.h"
#include "grid.h"

namespace Griddle {
	class GridModel : public glgl::DynamicModel {
		
		glm::vec3 v[8];
		glm::vec2 vt[8];
		glm::vec3 n[6];
		bool _yMajorOrder;
		void addBlockToModel(int x, int y, int z, float tx, float ty);
		void initBaseBlockModel();
		int _selectedBlock[3];
		bool _hideInActiveLayers, _ghostInActiveLayers;
		int _activeLayer;
	public:
		float _textureScale;
		int _width, _height, _depth;
		float _x, _y, _z;
		GridModel::GridModel();
		GridModel::GridModel(int width, int height, int depth, float x, float y, float ts, int state = glgl::StaticModel::COLORS);
		GridModel::GridModel(grid::StandardGrid* object, float ts, int state = glgl::StaticModel::COLORS);
		GridModel::GridModel(grid::IntGrid* object, float ts, int state = glgl::StaticModel::COLORS);
		GridModel::GridModel(grid::ByteGrid* object, float ts, int state = glgl::StaticModel::COLORS);
		void build();
		void build(grid::StandardGrid* object);
		void build(grid::IntGrid* object);
		void build(grid::ByteGrid* object);
		void setBlockTexture(int x, int y, int z, float xOffset, float yOffset);
		void setBlockColor(int x, int y, int z, float r, float g, float b, float a);
		void updateBlock(int x, int y, int z, int blockType, int data = 0);
		void setMouseOverBlock(int x, int y, int z);
		void draw(glgl::RenderState* state);

		void setLayerAlpha(int layer, float alpha);
		void setActiveLayer(int layer);
	};
}

#endif