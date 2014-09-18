#ifndef GRID_MODEL_H
#define GRID_MODEL_H

#include "includes.h"
#include "blocks.h"

namespace Griddle {
	class GridModel : public glgl::DynamicModel {
		
		glm::vec3 v[8];
		glm::vec2 vt[8];
		glm::vec3 n[6];
		bool _yMajorOrder;
		void addBlockToModel(int x, int y, int z, float tx, float ty);
		void initBaseBlockModel();
	public:
		float _textureScale;
		float _width, _height, _depth, _x, _y, _z;
		GridModel::GridModel();
		GridModel::GridModel(int width, int height, int depth, float x, float y, float ts, int state = glgl::StaticModel::COLORS);
		void build();
		void setBlockTexture(int x, int y, int z, float xOffset, float yOffset);
		void setBlockColor(int x, int y, int z, float r, float g, float b, float a);
		void updateBlock(int x, int y, int z, int blockType);
		void draw(glgl::RenderState* state);
	};
}

#endif