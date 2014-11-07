#ifndef FILE_DEF_LOADER_H
#define FILE_DEF_LOADER_H

#include "includes.h"
#include "blocks.h"
#include "parser.h"
#include <string>

namespace Griddle {
	class FileDefLoader : public Parser {
		Blocks::Block* _currentBlock;
		int _blockState;
		string _currentImagePath;
		int _lastBlockID;
		int _lastStateID;
	public:
		FileDefLoader();
		~FileDefLoader();
		enum {
			GLOBAL_STATE = 0,
			AWAITING_COMMAND = 1,
			BLOCK_STATE = 2,
			STATE_STATE = 3,
			XY_STATE = 4,
			RGB_STATE = 5,
			IMAGE_STATE = 6,
			DRAW_TYPE_STATE = 7
		};

		void registerCurrentBlock();
		void addBlock(int id);
		Blocks::Block* getBlock();
		Blocks::Block::State* getCurrentBlockState();
		void setBlockState(int state);
		void onEOF();

		class GlobalState : public Parser::State {
		public:
			GlobalState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);
			
		};

		class AwaitingCommandState : public Parser::State {
		public:
			AwaitingCommandState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};

		class BlockState : public Parser::State {
			FileDefLoader* _fileDefLoader;
			int _index;
		public:
			void onStart();
			BlockState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};

		class StateState : public Parser::State {
			FileDefLoader* _fileDefLoader;

		public:
			StateState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};
		class XYState : public Parser::State {
			FileDefLoader* _fileDefLoader;
			int _index;
		public:
			void onStart();
			XYState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};
		class RGBState : public Parser::State {
			FileDefLoader* _fileDefLoader;
			int _index;
		public:
			void onStart();
			RGBState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};
		class ImageState : public Parser::State {
			FileDefLoader* _fileDefLoader;
			int _index;
		public:
			void onStart();
			ImageState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};
		class DrawTypeState : public Parser::State {
			FileDefLoader* _fileDefLoader;

		public:
			DrawTypeState(Parser* parser);
			State* getNextState();
			int handleToken(Tokenizer::Token* token);

		};

		
	};

	/*
	class FileDefLoader {
	public:
		class Tokenizer {
		public:
			class Token {
			public:
				int type;
				std::string value;
			};
			enum {
				ALPHA = 0,
				NUMERIC = 1,
				OPERATOR = 2,
				DELIM = 3,
			};
			std::vector<Token> Tokenize(std::string string);
			int getCharType(char c);
		};
		enum {
			AWAITING_COMMAND = 0x0001,
			AWAITING_DATA = 0x0002,
			BLOCK_NAME = 0x0004,
			BLOCK_ID = 0x0008,
			STATE_ID = 0x0010,
			RGB_R = 0x0020,
			RGB_G = 0x0040,
			RGB_B = 0x0080,
			HARDNESS = 0x0100,
			IMAGE_PATH = 0x0200,




		};
		
		int _currentState;
		Blocks::Block* _currentBlock;
		string _currentImagePath;
		float _currentX, _currentY;
		float _currentR, _currentG, _currentB;
		void registerCurrentBlock();
		bool loadDefinitions(std::string file);
	};
	*/
}

#endif