#include "grid_model.h"

namespace Griddle {
	GridModel::GridModel() {
		_textureScale = 64.0f / 1024.0f;
		_x = _y = _z = 0;
		_width = _height = 32;
		_depth = 3;
		_yMajorOrder = false;
		initBaseBlockModel();

		_hideInActiveLayers = false;
		_ghostInActiveLayers = true;
		_activeLayer = 0;

	}
	GridModel::GridModel(int width, int height, int depth, float x, float y, float ts, int state ) {
		_width = width;
		_height = height;
		_depth = depth;
		_x = x;
		_y = y;
		_textureScale = ts;
		_yMajorOrder = false;
		initBaseBlockModel();
		_state = state;

		_hideInActiveLayers = false;
		_ghostInActiveLayers = true;
		_activeLayer = 0;

	}
	GridModel::GridModel(grid::StandardGrid* object, float ts, int state) {
		_width = object->getWidth();
		_height = object->getHeight();
		_depth = object->getDepth();
		_textureScale = ts;
		_yMajorOrder = false;
		initBaseBlockModel();
		_state = state;
		build(object);

		_hideInActiveLayers = false;
		_ghostInActiveLayers = true;
		_activeLayer = 0;

	}
	GridModel::GridModel(grid::IntGrid* object, float ts, int state) {
		_width = object->getWidth();
		_height = object->getHeight();
		_depth = object->getDepth();
		_textureScale = ts;
		_yMajorOrder = false;
		initBaseBlockModel();
		_state = state;
		build(object);

		_hideInActiveLayers = false;
		_ghostInActiveLayers = true;
		_activeLayer = 0;
	}
	GridModel::GridModel(grid::ByteGrid* object, float ts, int state) {
		_width = object->getWidth();
		_height = object->getHeight();
		_depth = object->getDepth();
		_textureScale = ts;
		_yMajorOrder = false;
		initBaseBlockModel();
		_state = state;
		build(object);

		_hideInActiveLayers = false;
		_ghostInActiveLayers = true;
		_activeLayer = 0;
	}
	void GridModel::initBaseBlockModel() {
		v[0] = glm::vec3(0, 0, 1);
		v[1]= glm::vec3(0, 0, 0);
		v[2] = glm::vec3(1, 0, 0);
		v[3] = glm::vec3(1, 0, 1);
		v[4] = glm::vec3(0, 1, 1);
		v[5] = glm::vec3(0, 1, 0);
		v[6] = glm::vec3(1, 1, 0);
		v[7] = glm::vec3(1, 1, 1);

		vt[0] = glm::vec2(0.06, 0.224) *_textureScale;
		vt[1] = glm::vec2(0.464, 0.471) *_textureScale;
		vt[2] = glm::vec2(0.056, 0.662) *_textureScale;
		vt[3] = glm::vec2(0.457, 0.875) *_textureScale; //19
		vt[4] = glm::vec2(0.896, 0.226) *_textureScale;
		vt[5] = glm::vec2(0.464, 0.471) *_textureScale;
		vt[6] = glm::vec2(0.455, 0.012) *_textureScale;
		vt[7] = glm::vec2(0.06, 0.224) *_textureScale; //24

		n[0] = glm::vec3(-1, 0, 0);
		n[1] = glm::vec3(0, 0, -1);
		n[2] = glm::vec3(1, 0, 0);
		n[3] = glm::vec3(1, 0, 1);
		n[4] = glm::vec3(0, -1, 0);
		n[5] = glm::vec3(0, 1, 0);

		wall_v[0] = glm::vec3(0, 0, 0.75);
		wall_v[1] = glm::vec3(0, 0, 0.25);
		wall_v[2] = glm::vec3(1, 0, 0.25);
		wall_v[3] = glm::vec3(1, 0, 0.75);
		wall_v[4] = glm::vec3(0, 1, 0.75);
		wall_v[5] = glm::vec3(0, 1, 0.25);
		wall_v[6] = glm::vec3(1, 1, 0.25);
		wall_v[7] = glm::vec3(1, 1, 0.75);

		wall_v_rotated[0] = glm::vec3(0.25, 0, 1);
		wall_v_rotated[1] = glm::vec3(0.25, 0, 0);
		wall_v_rotated[2] = glm::vec3(0.75, 0, 0);
		wall_v_rotated[3] = glm::vec3(0.75, 0, 1);
		wall_v_rotated[4] = glm::vec3(0.25, 1, 1);
		wall_v_rotated[5] = glm::vec3(0.25, 1, 0);
		wall_v_rotated[6] = glm::vec3(0.75, 1, 0);
		wall_v_rotated[7] = glm::vec3(0.75, 1, 1);

		wall_vt[0] = glm::vec2(0.06, 0.224) *_textureScale;
		wall_vt[1] = glm::vec2(0.464, 0.471) *_textureScale;
		wall_vt[2] = glm::vec2(0.056, 0.662) *_textureScale;
		wall_vt[3] = glm::vec2(0.457, 0.875) *_textureScale; //19
		wall_vt[4] = glm::vec2(0.896, 0.226) *_textureScale;
		wall_vt[5] = glm::vec2(0.464, 0.471) *_textureScale;
		wall_vt[6] = glm::vec2(0.455, 0.012) *_textureScale;
		wall_vt[7] = glm::vec2(0.06, 0.224) *_textureScale; //24
	}
	void GridModel::addBlockToModel(int x, int y, int z, float tx, float ty, int model) {
		

		glm::vec3 blockPos = glm::vec3(x, z, y);
		glm::vec2 texOffset = glm::vec2(tx, ty) *_textureScale;
		glm::vec4 color = glm::vec4(1, 1, 1, 1);

		glm::vec3* arr = NULL;
		if (model == BLOCK) {
			arr = v;
		}
		else if (model == WALL) {
			arr = wall_v;
		}
		else if (model == WALL_R) {
			arr = wall_v_rotated;
		}
		else {
			return;
		}

		this->addPoint(blockPos + arr[4], texOffset + vt[0], n[0], color); this->addPoint(blockPos + arr[5], texOffset + vt[1], n[0], color); this->addPoint(blockPos + arr[0], texOffset + vt[2], n[0], color);
		this->addPoint(blockPos + arr[5], texOffset + vt[0], n[1], color); this->addPoint(blockPos + arr[6], texOffset + vt[1], n[1], color); this->addPoint(blockPos + arr[1], texOffset + vt[2], n[1], color);
		this->addPoint(blockPos + arr[6], texOffset + vt[0], n[2], color); this->addPoint(blockPos + arr[7], texOffset + vt[1], n[2], color); this->addPoint(blockPos + arr[2], texOffset + vt[2], n[2], color);
		this->addPoint(blockPos + arr[7], texOffset + vt[0], n[3], color); this->addPoint(blockPos + arr[4], texOffset + vt[1], n[3], color); this->addPoint(blockPos + arr[3], texOffset + vt[2], n[3], color);
		this->addPoint(blockPos + arr[0], texOffset + vt[0], n[4], color); this->addPoint(blockPos + arr[1], texOffset + vt[1], n[4], color); this->addPoint(blockPos + arr[3], texOffset + vt[2], n[4], color);
		this->addPoint(blockPos + arr[7], texOffset + vt[4], n[5], color); this->addPoint(blockPos + arr[6], texOffset + vt[5], n[5], color); this->addPoint(blockPos + arr[4], texOffset + vt[6], n[5], color);

		this->addPoint(blockPos + arr[5], texOffset + vt[1], n[0], color); this->addPoint(blockPos + arr[1], texOffset + vt[3], n[0], color); this->addPoint(blockPos + arr[0], texOffset + vt[2], n[0], color);
		this->addPoint(blockPos + arr[6], texOffset + vt[1], n[1], color); this->addPoint(blockPos + arr[2], texOffset + vt[3], n[1], color); this->addPoint(blockPos + arr[1], texOffset + vt[2], n[1], color);
		this->addPoint(blockPos + arr[7], texOffset + vt[1], n[2], color); this->addPoint(blockPos + arr[3], texOffset + vt[3], n[2], color); this->addPoint(blockPos + arr[2], texOffset + vt[2], n[2], color);
		this->addPoint(blockPos + arr[4], texOffset + vt[1], n[3], color); this->addPoint(blockPos + arr[0], texOffset + vt[3], n[3], color); this->addPoint(blockPos + arr[3], texOffset + vt[2], n[3], color);
		this->addPoint(blockPos + arr[1], texOffset + vt[1], n[4], color); this->addPoint(blockPos + arr[2], texOffset + vt[3], n[4], color); this->addPoint(blockPos + arr[3], texOffset + vt[2], n[4], color);
		this->addPoint(blockPos + arr[6], texOffset + vt[5], n[5], color); this->addPoint(blockPos + arr[5], texOffset + vt[7], n[5], color); this->addPoint(blockPos + arr[4], texOffset + vt[6], n[5], color);


	}

	void GridModel::build() {
		this->clear();
		
		for (int y = 0; y < _height; y++) {
			for (int x = 0; x < _width; x++) {
				for (int z = 0; z < _depth; z++) {
					addBlockToModel((float)x, (float)y, (float)z,0, 0);
				}
			}
		}
	}
	void GridModel::build(grid::StandardGrid* object) {
		if (object) {
			this->clear();
			unsigned short* buffer = new unsigned short[object->getBlockAtomCount()];
			for (int y = 0; y < _height; y++) {
				for (int x = 0; x < _width; x++) {
					for (int z = 0; z < _depth; z++) {
						object->getData_safe(x, y, z, buffer);
						float xOff = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._xOffset;
						float yOff = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._yOffset;
						int model = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._drawType;
						addBlockToModel((float)x, (float)y, (float)z, xOff, yOff, model);
					}
				}
			}
			delete[] buffer;
		}
	}
	void GridModel::build(grid::IntGrid* object) {
		if (object) {
			this->clear();
			unsigned int* buffer = new unsigned int[object->getBlockAtomCount()];
			for (int y = 0; y < _height; y++) {
				for (int x = 0; x < _width; x++) {
					for (int z = 0; z < _depth; z++) {
						object->getData_safe(x, y, z, buffer);
						float xOff = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._xOffset;
						float yOff = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._yOffset;
						int model = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._drawType;
						addBlockToModel((float)x, (float)y, (float)z, xOff, yOff, model);
					}
				}
			}
			delete[] buffer;
		}
	}
	void GridModel::build(grid::ByteGrid* object) {
		if (object) {
			this->clear();
			unsigned char* buffer = new unsigned char[object->getBlockAtomCount()];
			for (int y = 0; y < _height; y++) {
				for (int x = 0; x < _width; x++) {
					for (int z = 0; z < _depth; z++) {
						object->getData_safe(x, y, z, buffer);
						float xOff = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._xOffset;
						float yOff = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._yOffset;
						int model = Blocks::getInstance()->getBlock(buffer[0])._stateList[(int)buffer[1]]._drawType;
						addBlockToModel((float)x, (float)y, (float)z, xOff, yOff, model);
					}
				}
			}
			delete[] buffer;
		}
	}
	void GridModel::setBlockTexture(int x, int y, int z, float xOffset, float yOffset) {
		glm::vec2 texOffset = glm::vec2(xOffset, yOffset) *_textureScale;
		int a = (_yMajorOrder) ? y : x;
		int b = (_yMajorOrder) ? x : y;
		int start = ((b*_width*_depth) + (a*_depth) + z)*36;
		_uvs[start++] = texOffset + vt[0]; _uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[0]; _uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[0]; _uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[0]; _uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[0]; _uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[4]; _uvs[start++] = texOffset + vt[5]; _uvs[start++] = texOffset + vt[6];

		_uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[3]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[3]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[3]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[3]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[1]; _uvs[start++] = texOffset + vt[3]; _uvs[start++] = texOffset + vt[2];
		_uvs[start++] = texOffset + vt[5]; _uvs[start++] = texOffset + vt[7]; _uvs[start++] = texOffset + vt[6];


	}
	void GridModel::setBlockModel(int x, int y, int z, int model) {
		int a = (_yMajorOrder) ? y : x;
		int b = (_yMajorOrder) ? x : y;
		int start = ((b*_width*_depth) + (a*_depth) + z) * 36;
		glm::vec3 blockPos = glm::vec3(x, z, y);
		glm::vec3* arr = NULL;
		if (model == BLOCK) {
			arr = v;
		}
		else if (model == WALL) {
			arr = wall_v;
		}
		else if (model == WALL_R) {
			arr = wall_v_rotated;
		}
		else {
			return;
		}
		_vertices[start++] = blockPos + arr[4]; _vertices[start++] = blockPos + arr[5]; _vertices[start++] = blockPos + arr[0];
		_vertices[start++] = blockPos + arr[5]; _vertices[start++] = blockPos + arr[6]; _vertices[start++] = blockPos + arr[1];
		_vertices[start++] = blockPos + arr[6]; _vertices[start++] = blockPos + arr[7]; _vertices[start++] = blockPos + arr[2];
		_vertices[start++] = blockPos + arr[7]; _vertices[start++] = blockPos + arr[4]; _vertices[start++] = blockPos + arr[3];
		_vertices[start++] = blockPos + arr[0]; _vertices[start++] = blockPos + arr[1]; _vertices[start++] = blockPos + arr[3];
		_vertices[start++] = blockPos + arr[7]; _vertices[start++] = blockPos + arr[6]; _vertices[start++] = blockPos + arr[4];

		_vertices[start++] = blockPos + arr[5]; _vertices[start++] = blockPos + arr[1]; _vertices[start++] = blockPos + arr[0];
		_vertices[start++] = blockPos + arr[6]; _vertices[start++] = blockPos + arr[2]; _vertices[start++] = blockPos + arr[1];
		_vertices[start++] = blockPos + arr[7]; _vertices[start++] = blockPos + arr[3]; _vertices[start++] = blockPos + arr[2];
		_vertices[start++] = blockPos + arr[4]; _vertices[start++] = blockPos + arr[0]; _vertices[start++] = blockPos + arr[3];
		_vertices[start++] = blockPos + arr[1]; _vertices[start++] = blockPos + arr[2]; _vertices[start++] = blockPos + arr[3];
		_vertices[start++] = blockPos + arr[6]; _vertices[start++] = blockPos + arr[5]; _vertices[start++] = blockPos + arr[4];
	}
	void GridModel::setBlockColor(int x, int y, int z, float r, float g, float b, float a) {
		int a1 = (_yMajorOrder) ? y : x;
		int b1 = (_yMajorOrder) ? x : y;
		int start = ((b1*_width*_depth) + (a1*_depth) + z) * 36;
		for (int i = 0; i < 36; i++) {
			_colors[start + i] = glm::vec4(r, g, b, a);
		}
	}
	void GridModel::updateBlock(int x, int y, int z, int blockType, int data) {
		Blocks::Block block = Blocks::getInstance()->getBlock(blockType);
		setBlockTexture(x, y, z, block._stateList[data]._xOffset, block._stateList[data]._yOffset);
		setBlockColor(x, y, z, block._stateList[data]._r, block._stateList[data]._g, block._stateList[data]._b, block._stateList[data]._a);
		std::cout << block._stateList[data]._drawType << std::endl;
		setBlockModel(x, y, z, block._stateList[data]._drawType);

	}
	void GridModel::setMouseOverBlock(int x, int y, int z) {
		if (x != _selectedBlock[0] || y != _selectedBlock[1] || z != _selectedBlock[2]) {
			if (_selectedBlock[0] < 0 || _selectedBlock[1] < 0 || _selectedBlock[2] < 0) {
				
			}
			else {
				setBlockColor(_selectedBlock[0], _selectedBlock[1], _selectedBlock[2], 1, 1, 1, 1);
			}
			

			if (x < 0 || x >= this->_width || y < 0 || y >= this->_height || z < 0 || z >= this->_depth) {
				
				_selectedBlock[0] = -1;
				_selectedBlock[1] = -1;
				_selectedBlock[2] = -1;
			}
			else {
				_selectedBlock[0] = x;
				_selectedBlock[1] = y;
				_selectedBlock[2] = z;
				setBlockColor(_selectedBlock[0], _selectedBlock[1], _selectedBlock[2], 0, 1, 0, 1);
			}

			
			
		}
		//std::cout << "selected block: " << _selectedBlock[0] << ", " << _selectedBlock[1] << ", " << _selectedBlock[2] << std::endl;
	}
	void GridModel::draw(glgl::RenderState* state) {
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		state->matrices().pushMatrix();
		state->matrices().translate(_x, _y, _z);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(_vertices[0]));
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &(_normals[0]));
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, &(_uvs[0]));
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, &(_colors[0]));

		glDrawArrays(GL_TRIANGLES, 0, _vertices.size());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		state->matrices().popMatrix();
	}

	void GridModel::drawWireFrames(glgl::RenderState* state) {
		int start = ((this->_selectedBlock[1] * _width*_depth) + (this->_selectedBlock[0] * _depth) + this->_selectedBlock[2]) * 36;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		state->matrices().pushMatrix();
		state->matrices().translate(_x, _y, _z);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &(_vertices[0]));
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &(_normals[0]));
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, &(_uvs[0]));
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, &(_colors[0]));
		
		glColor4f(0.1, 1, 0.1, 0.1);
		glDrawArrays(GL_TRIANGLES, start, 36);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glBegin(GL_LINES);
		glColor4f(1, 0, 0, 0.9);
		glVertex3f(-1, this->_selectedBlock[2] + 0.5f, this->_selectedBlock[1] + 0.5f);
		glVertex3f(_width + 1, this->_selectedBlock[2] + 0.5f, this->_selectedBlock[1] + 0.5f);
		glColor4f(0, 1, 0, 0.9);
		glVertex3f(this->_selectedBlock[0] + 0.5f, this->_selectedBlock[2] + 0.5f, - 1);
		glVertex3f(this->_selectedBlock[0] + 0.5f, this->_selectedBlock[2] + 0.5f, _width + 1);
		glColor4f(0, 0, 1, 0.9);
		glVertex3f(this->_selectedBlock[0] + 0.5f, -1, this->_selectedBlock[1] + 0.5f);
		glVertex3f(this->_selectedBlock[0] + 0.5f, _depth + 1, this->_selectedBlock[1] + 0.5f);
		glEnd();
		
		state->matrices().popMatrix();
	}
	void GridModel::setLayerAlpha(int layer, float alpha) {

		for (int x = 0; x < _width; x++) {
			for (int y = 0; y < _height; y++) {
				int z = layer;
				int start = ((y*_width*_depth) + (x*_depth) + z) * 36;
				for (int i = 0; i < 36; i++) {
					_colors[start + i].a = alpha;
				}
			}
		}
	}
	void GridModel::setActiveLayer(int layer) {
		_activeLayer = layer;
		for (int i = 0; i < _depth; i++) {
			if (i == layer) {
				setLayerAlpha(i, 1);
			}
			else {
				if (_hideInActiveLayers) {
					if (_colors[i * 36].a != 0) {
						setLayerAlpha(i, 0);
					}	
				}
				else if (_ghostInActiveLayers) {
					if (_colors[i * 36].a != 0.25) {
						setLayerAlpha(i, 0.25);
					}
				}
				else {
					setLayerAlpha(i, 1);
				}
			}
		}
	}
	void GridModel::toggleActiveLayerGhosting() {
		_ghostInActiveLayers = !_ghostInActiveLayers;
		for (int i = 0; i < _depth; i++) {
			if (i == this->_activeLayer) {
				setLayerAlpha(i, 1);
			}
			else {
				if (_ghostInActiveLayers) {
					if (_colors[i * 36].a != 0.25) {
						setLayerAlpha(i, 0.25);
					}
				}
				else {
					setLayerAlpha(i, 1);
				}
			}
		}
	}
}