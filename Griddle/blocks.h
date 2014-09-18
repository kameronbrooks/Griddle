#ifndef BLOCKS_H
#define BLOCKS_H

#include <string>
#include <map>
#include "Utility.h"

namespace Griddle {
	class Blocks {
	public:
		class Block {
		public:
			int _id;
			std::string name;
			float _xOffset;
			float _yOffset;
			float _r, _g, _b, _a;
			float _hardness;
		};
	public:
		Block registerBlockType(Block& block);
		Block registerBlockType(int id, std::string name, float xOffset, float yOffset, float r = 1, float g = 1, float b = 1, float a = 1, float hardness = 1);
		Block getBlock(int id);
		static Blocks* getInstance();
	protected:
		Blocks();
	private:
		static Blocks * _instance;
		std::map<int, Block> _registeredBlockList;
		
	};

	class BlockTextures {
	protected:
		TextureAtlasBuilder _textureAtlasBuilder;
		static BlockTextures* _instance;
		float _baseSpriteSize;
		float _atlasSize;
		BlockTextures();
	public:
		static BlockTextures* getInstance();
		static BlockTextures* newInstance(float baseSpriteSize, float atlasSize);
		bool loadTexture(Blocks::Block block, std::string filename);
		float getBaseSpriteSize();
		float getAtlasSize();
		float getTextureToAtlasRatio();
		GLuint build();
	};


}

#endif