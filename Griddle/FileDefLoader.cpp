#include "FileDefLoader.h"
#include <ctype.h>
using namespace std;

namespace Griddle {

	FileDefLoader::FileDefLoader() {
		addState(GLOBAL_STATE, new GlobalState(this));
		_globalState = _stateMap[GLOBAL_STATE];
		_currentState = _globalState;

		addState(AWAITING_COMMAND, new AwaitingCommandState(this));
		addState(BLOCK_STATE, new BlockState(this));
		addState(STATE_STATE, new StateState(this));
		addState(RGB_STATE, new RGBState(this));
		addState(XY_STATE, new XYState(this));
		addState(IMAGE_STATE, new ImageState(this));
		addState(DRAW_TYPE_STATE, new DrawTypeState(this));

	}
	FileDefLoader::~FileDefLoader() {

	}

	void FileDefLoader::registerCurrentBlock() {
		cout << " registering block " << _currentBlock->_id << " " << _currentBlock->_name << endl;
		Blocks::getInstance()->registerBlockType(*_currentBlock);
		for (map<int, Blocks::Block::State>::iterator itor = _currentBlock->_stateList.begin(); itor != _currentBlock->_stateList.end(); itor++) {
			if (itor->second._imagePath != "") {
				cout << " loading image " << _currentBlock->_id << " " << _currentBlock->_name << " state = " << itor->first << " path = " << itor->second._imagePath << endl;
				BlockTextures::getInstance()->loadTexture(*_currentBlock, itor->first, itor->second._imagePath);
			}
		}
		delete _currentBlock;
	}
	void FileDefLoader::addBlock(int id) {
		if (_currentBlock)registerCurrentBlock();
		_currentBlock = new Blocks::Block();
		_currentBlock->_id = id;
		_blockState = 0;
		_currentBlock->addState(0, 0, 0);
		_lastBlockID = id;
	}
	Blocks::Block* FileDefLoader::getBlock() {
		return _currentBlock;
	}
	Blocks::Block::State* FileDefLoader::getCurrentBlockState() {
		if (_currentBlock) {
			return &(_currentBlock->_stateList[_blockState]);
		}
		else {
			return NULL;
		}
		
	}
	void FileDefLoader::setBlockState(int state) {
		_blockState = state;
		_lastStateID = state;
		if (_currentBlock) {
			_currentBlock->addState(state, 0, 0);
		}
	}

	void FileDefLoader::onEOF() {
		if (_currentBlock) {
			registerCurrentBlock();
		}
	}



	FileDefLoader::GlobalState::GlobalState(Parser* parser) : State(parser) {
		
	}
	Parser::State* FileDefLoader::GlobalState::getNextState() {
		return NULL;
	}
	int FileDefLoader::GlobalState::handleToken(Tokenizer::Token* token) {
		cout << " Global State: token = " << token->value << endl;
		if (token->value == "#") {
			_parent->requestSkipLine();
			return SUCCESS;
		}
		else if (token->value == "-") {
			_parent->requestNextState(AWAITING_COMMAND);
			return SUCCESS;
		}
		else {
			return ERROR_UNEXPECTED_TOKEN;
		}
	}


	FileDefLoader::AwaitingCommandState::AwaitingCommandState(Parser* parser) : State(parser) {

	}
	Parser::State* FileDefLoader::AwaitingCommandState::getNextState() {
		return NULL;
	}
	int FileDefLoader::AwaitingCommandState::handleToken(Tokenizer::Token* token) {
		cout << " Command State: token = " << token->value << endl;
		if (token->type == Tokenizer::ALPHA) {
			int commandID = 0;
			if (token->value == "block") {
				commandID = BLOCK_STATE;
			}
			else if (token->value == "state") {
				commandID = STATE_STATE;
			}
			else if (token->value == "rgb") {
				commandID = RGB_STATE;
			}
			else if (token->value == "xy") {
				commandID = XY_STATE;
			}
			else if (token->value == "image") {
				commandID = IMAGE_STATE;
			}
			else if (token->value == "drawtype") {
				commandID = DRAW_TYPE_STATE;
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
			_parent->requestNextState(commandID);
			return SUCCESS;
		}
		else {
			return ERROR_UNEXPECTED_TOKEN;
		}
	}

	FileDefLoader::BlockState::BlockState(Parser* parser) : State(parser) {
		this->_fileDefLoader = (FileDefLoader*)parser;
	}
	void FileDefLoader::BlockState::onStart() {
		_index = 0;
	}
	Parser::State* FileDefLoader::BlockState::getNextState() {
		return NULL;
	}
	int FileDefLoader::BlockState::handleToken(Tokenizer::Token* token) {
		
		if (_index == 0) {
			if (token->type == Tokenizer::NUMERIC) {
				cout << " Block State: token = " << token->value << endl;
				this->_fileDefLoader->addBlock(atoi(token->value.c_str()));
				++_index;
			}
			else if (token->type == Tokenizer::OPERATOR) {
				cout << " Block State: token = " << this->_fileDefLoader->_lastBlockID + 1 << endl;
				this->_fileDefLoader->addBlock(this->_fileDefLoader->_lastBlockID + 1);
				++_index;
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		else if(_index == 1) {
			if (token->type == Tokenizer::ALPHA) {
				cout << " Block State: token = " << token->value << endl;
				this->_fileDefLoader->getBlock()->_name = token->value;
				_parent->requestNextState(GLOBAL_STATE);
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		return SUCCESS;
	}

	

	FileDefLoader::StateState::StateState(Parser* parser) : State(parser) {
		this->_fileDefLoader = (FileDefLoader*)parser;
	}
	Parser::State* FileDefLoader::StateState::getNextState() {
		return NULL;
	}
	int FileDefLoader::StateState::handleToken(Tokenizer::Token* token) {
		
		if (token->type == Tokenizer::NUMERIC) {
			cout << " State State: token = " << token->value << endl;
			this->_fileDefLoader->setBlockState(atoi(token->value.c_str()));
			_parent->requestNextState(GLOBAL_STATE);
		}
		else if (token->type == Tokenizer::OPERATOR) {
			cout << " State State: token = " << _fileDefLoader->_lastStateID + 1 << endl;
			this->_fileDefLoader->setBlockState(_fileDefLoader->_lastStateID+1);
			_parent->requestNextState(GLOBAL_STATE);
		}
		else {
			return ERROR_UNEXPECTED_TOKEN;
		}
		return SUCCESS;
	}

	void FileDefLoader::XYState::onStart() {
		_index = 0;
	}

	FileDefLoader::XYState::XYState(Parser* parser) : State(parser) {
		this->_fileDefLoader = (FileDefLoader*)parser;
	}
	Parser::State* FileDefLoader::XYState::getNextState() {
		return NULL;
	}
	int FileDefLoader::XYState::handleToken(Tokenizer::Token* token) {
		if (_index == 0) {
			if (token->type == Tokenizer::NUMERIC) {
				Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
				if (state) {
					state->_xOffset = atof(token->value.c_str());
				}
				else {

				}
				++_index;
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		else if (_index == 1) {
			if (token->type == Tokenizer::NUMERIC) {
				Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
				if (state) {
					state->_yOffset = atof(token->value.c_str());
				}
				else {

				}
				 
				_parent->requestNextState(GLOBAL_STATE);
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		return SUCCESS;
	}

	void FileDefLoader::RGBState::onStart() {
		_index = 0;
	}

	FileDefLoader::RGBState::RGBState(Parser* parser) : State(parser) {
		this->_fileDefLoader = (FileDefLoader*)parser;
	}
	Parser::State* FileDefLoader::RGBState::getNextState() {
		return NULL;
	}
	int FileDefLoader::RGBState::handleToken(Tokenizer::Token* token) {
		if (_index == 0) {
			if (token->type == Tokenizer::NUMERIC) {
				Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
				if (state) {
					state->_r = atof(token->value.c_str());
				}
				else {

				}
				++_index;
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		else if (_index == 1) {
			if (token->type == Tokenizer::NUMERIC) {
				Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
				if (state) {
					state->_g = atof(token->value.c_str());
				}
				else {

				}
				++_index;
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		else if (_index == 2) {
			if (token->type == Tokenizer::NUMERIC) {
				Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
				if (state) {
					state->_b = atof(token->value.c_str());
				}
				else {

				}

				_parent->requestNextState(GLOBAL_STATE);
			}
			else {
				return ERROR_UNEXPECTED_TOKEN;
			}
		}
		else {
			return ERROR_UNEXPECTED_TOKEN;
		}
		return SUCCESS;
	}

	void FileDefLoader::ImageState::onStart() {
		_index = 0;
	}

	FileDefLoader::ImageState::ImageState(Parser* parser) : State(parser) {
		this->_fileDefLoader = (FileDefLoader*)parser;
	}
	Parser::State* FileDefLoader::ImageState::getNextState() {
		return NULL;
	}
	int FileDefLoader::ImageState::handleToken(Tokenizer::Token* token) {
		cout << " Image State: token = " << token->value << endl;
		if (token->type == Tokenizer::ALPHA) {
			this->_fileDefLoader->getCurrentBlockState()->_imagePath = token->value;
			_parent->requestNextState(GLOBAL_STATE);
			return SUCCESS;
		}
		else {
			return ERROR_UNEXPECTED_TOKEN;
		}
	}

	FileDefLoader::DrawTypeState::DrawTypeState(Parser* parser) : State(parser) {
		this->_fileDefLoader = (FileDefLoader*)parser;
	}
	Parser::State* FileDefLoader::DrawTypeState::getNextState() {
		return NULL;
	}
	int FileDefLoader::DrawTypeState::handleToken(Tokenizer::Token* token) {
		cout << " Draw Type State: token = " << token->value << endl;
		if (token->type == Tokenizer::NUMERIC) {
			Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
			if (state) {
				state->_drawType = atoi(token->value.c_str());
			}
			else {

			}
			_parent->requestNextState(GLOBAL_STATE);
		}
		else if (token->type == Tokenizer::OPERATOR) {
			Blocks::Block::State* state = this->_fileDefLoader->getCurrentBlockState();
			if (state) {
				state->_drawType = 0;
			}
			else {

			}
			_parent->requestNextState(GLOBAL_STATE);
		}
		else {
			return ERROR_UNEXPECTED_TOKEN;
		}
		return SUCCESS;
	}

	/*
	vector<FileDefLoader::Tokenizer::Token> FileDefLoader::Tokenizer::Tokenize(string string) {
		vector<Token> output;
		const char* cstr = string.c_str();
		char* buffer = new char[string.length() + 1];
		memset(buffer, '\0', string.length() + 1);
		int index = 0;
		int bufferIndex = 0;
		int state = -1;

		while (index < string.length()) {
			int currentType = getCharType(cstr[index]);
			if (currentType == NUMERIC && state == ALPHA)currentType = ALPHA;
			else if (cstr[index] == '/' && state == ALPHA)currentType = ALPHA;
			if (currentType != state && state != -1) {
				Token t;
				std::string val(buffer);
				t.type = state;
				t.value = val;
				if (t.type != DELIM)output.push_back(t);

				bufferIndex = 0;
				memset(buffer, '\0', string.length() + 1);
			}
			if (true) {
				state = currentType;
				buffer[bufferIndex++] = cstr[index];
			}
			++index;

		}
		if (bufferIndex > 0) {
			Token t;
			std::string val(buffer);
			t.type = state;
			t.value = val;
			if (t.type != DELIM)output.push_back(t);

			bufferIndex = 0;

		}

		delete[] buffer;
		return output;
	}
	int FileDefLoader::Tokenizer::getCharType(char c) {
		string operators = "-+=()/<>;,*&^%";
		string safeOperators = "._";
		if (isalpha(c) || c == '.' || c == '_' || c == '\\') {
			return ALPHA;
		}
		else if (isdigit(c)) {
			return NUMERIC;
		}
		else if (operators.find(c) != string::npos) {
			return OPERATOR;
		}
		else {
			return DELIM;
		}
	}
	bool FileDefLoader::loadDefinitions(std::string file) {
		ifstream stream;
		Tokenizer tokenizer;
		vector<Tokenizer::Token> tokenList;
		stream.open(file.c_str(), ios::in);

		if (!stream.is_open()) {
			return false;
		}
		bool reading = true;
		string line;

		int state = 0;
		int lineNumber = 0;

		Blocks::Block* currentBlock = NULL;
		int stateID = 0;

		while (reading) {
			if (stream.eof()) {
				reading = false;
				continue;
			}
			++lineNumber;
			getline(stream, line);
			tokenList = tokenizer.Tokenize(line);

			vector<Tokenizer::Token>::iterator lineItor;
			for (lineItor = tokenList.begin(); lineItor != tokenList.end(); lineItor++) {
				if ((state & AWAITING_COMMAND) != 0) {
					if (lineItor->type == Tokenizer::ALPHA) {
						if (lineItor->value == "block") {
							state = AWAITING_DATA | BLOCK_NAME;
						}
						else if (lineItor->value == "image") {
							state = AWAITING_DATA | IMAGE_PATH;
						}
						else if (lineItor->value == "state") {
							state = AWAITING_DATA | STATE_ID;
						}
						else if (lineItor->value == "rgb") {
							state = AWAITING_DATA | RGB_R;
						}
					}
					else {
						cout << "Error on line[" << lineNumber << "]" << ": '" << lineItor->value << "' is not a valid operation" << endl;
					}
				}
				else if ((state & AWAITING_DATA) != 0) {
					if ((state & BLOCK_NAME) != 0) {

					}
					else if ((state & STATE_ID) != 0) {

					}
					else if ((state & IMAGE_PATH) != 0) {

					}
					else if ((state & RGB_R) != 0) {

					}
					else if ((state & RGB_G) != 0) {

					}
					else if ((state & RGB_B) != 0) {

					}
				}
				else {
					if (lineItor->value == "-") {
						state = AWAITING_COMMAND;
					}
					else {
						cout << "Error on line[" << lineNumber << "]" << ": '" << lineItor->value << "' is not a command initialzer" << endl;
					}

				}

			}

		}
		stream.close();
		return true;
	}
	*/
}