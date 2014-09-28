#include "blocks.h"

namespace Griddle {
	Blocks* Blocks::_instance = NULL;
	Blocks::Block Blocks::registerBlockType(Block& block) {
		_registeredBlockList[block._id] = block;
		return block;
	}

	Blocks::Block& Blocks::Block::addState(int data, float xOffset, float yOffset, float r, float g, float b, float a, float hardness) {
		State state;
		state._xOffset = xOffset;
		state._yOffset = yOffset;
		state._r = r;
		state._g = g;
		state._b = b;
		state._a = a;
		state._hardness = hardness;
		_stateList[data] = state;
		return (*this);
	}
	Blocks::Block Blocks::registerBlockType(int id, std::string name, float xOffset, float yOffset, float r, float g, float b, float a, float hardness) {
		Block block;
		block._id = id;
		block._name = name;
		block._stateList[0] = Block::State();
		block._stateList[0]._xOffset = xOffset;
		block._stateList[0]._yOffset = yOffset;
		block._stateList[0]._hardness = hardness;
		block._stateList[0]._r = r;
		block._stateList[0]._g = g;
		block._stateList[0]._b = b;
		block._stateList[0]._a = a;
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
		return _textureAtlasBuilder.loadImage(filename, block._stateList[0]._xOffset*_baseSpriteSize, block._stateList[0]._yOffset*_baseSpriteSize);
	}
	bool BlockTextures::loadTexture(Blocks::Block block, int data, std::string filename) {
		return _textureAtlasBuilder.loadImage(filename, block._stateList[data]._xOffset*_baseSpriteSize, block._stateList[data]._yOffset*_baseSpriteSize);
	}
	GLuint BlockTextures::build() {
		return _textureAtlasBuilder.createTextureAtlas();
	}
}