#include "blocks.h"

namespace Griddle {
	Blocks* Blocks::_instance = NULL;
	Blocks::Block Blocks::registerBlockType(Block& block) {
		_registeredBlockList[block._id] = block;
		return block;
	}
	Blocks::Block Blocks::registerBlockType(int id, std::string name, float xOffset, float yOffset, float r, float g, float b, float a, float hardness) {
		Block block;
		block._id = id;
		block.name = name;
		block._xOffset = xOffset;
		block._yOffset = yOffset;
		block._hardness = hardness;
		block._r = r;
		block._g = g;
		block._b = b;
		block._a = a;
		_registeredBlockList[id] = block;
		return block;
	}
	Blocks::Block Blocks::getBlock(int id) {
		if (_registeredBlockList.find(id) == _registeredBlockList.end()) {
			return Block();
		}
		else {
			return _registeredBlockList[id];
		}
	}
	Blocks* Blocks::getInstance() {
		if (_instance == NULL)_instance = new Blocks();
		return _instance;
	}
	
	Blocks::Blocks() {
		
	}

	BlockTextures::BlockTextures() {
		_baseSpriteSize = 64;
		_atlasSize = 1024;
	}

	BlockTextures* BlockTextures::_instance = NULL;

	BlockTextures* BlockTextures::getInstance() {
		if (_instance == NULL)_instance = new BlockTextures();
		return _instance;
	}
	BlockTextures* BlockTextures::newInstance(float baseSpriteSize, float atlasSize) {
		if (_instance != NULL) {
			delete _instance;
		}
		_instance = new BlockTextures();
		_instance->_baseSpriteSize = baseSpriteSize;
		_instance->_atlasSize = atlasSize;
		return _instance;
	}
	float BlockTextures::getBaseSpriteSize() {
		return _baseSpriteSize;
	}
	float BlockTextures::getAtlasSize() {
		return _atlasSize;
	}
	float BlockTextures::getTextureToAtlasRatio() {
		return _baseSpriteSize / _atlasSize;
	}
	bool BlockTextures::loadTexture(Blocks::Block block, std::string filename) {
		return _textureAtlasBuilder.loadImage(filename, block._xOffset*_baseSpriteSize, block._yOffset*_baseSpriteSize);
	}
	GLuint BlockTextures::build() {
		return _textureAtlasBuilder.createTextureAtlas();
	}
}