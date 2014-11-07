#include "parser.h"




Parser::Tokenizer::Tokenizer() {
	operators = "-+=()/<>;,*&^%#\"\\";
}

vector<Parser::Tokenizer::Token> Parser::Tokenizer::tokenize(string string) {
	vector<Token> output;
	const char* cstr = string.c_str();
	char* buffer = new char[string.length() + 1];
	memset(buffer, '\0', string.length() + 1);
	int index = 0;
	int bufferIndex = 0;
	int state = -1;

	while (index < string.length()) {
		int currentType = getCharType(cstr[index]);
		bool replace = false;
		if (currentType == NUMERIC && state == ALPHA)currentType = ALPHA;
		else if (cstr[index] == '/' && state == ALPHA)currentType = ALPHA;
		else if (cstr[index] == '+' && state == ALPHA) {
			currentType = ALPHA;
			replace = true;
		}
		if (currentType != state && state != -1) {
			Token t;
			std::string val(buffer);
			t.type = state;
			t.value = val;
			if (t.type != DELIM)output.push_back(t);

			bufferIndex = 0;
			memset(buffer, '\0', string.length() + 1);
		}
		if (replace) {
			state = currentType;
			char subst = '\0';
			if (cstr[index] == '+') {
				subst = ' ';
			}
			buffer[bufferIndex++] = subst;
		}
		else {
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
int Parser::Tokenizer::getCharType(char c) {
	
	
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

vector<Parser::Tokenizer::Token> Parser::StringLiteralTokenizer::tokenize(string string) {
	vector<Token> output;
	const char* cstr = string.c_str();
	char* buffer = new char[string.length() + 1];
	memset(buffer, '\0', string.length() + 1);
	int index = 0;
	int bufferIndex = 0;
	int state = -1;
	bool stringLiteralState = false;
	bool despecializeState = false;

	while (index < string.length()) {
		int currentType = getCharType(cstr[index]);
		bool skip = false;
		if (currentType == NUMERIC && state == ALPHA)currentType = ALPHA;
		
		if (currentType == OPERATOR) {
			if (cstr[index] == '"') {
				if (!despecializeState) {
					stringLiteralState = !stringLiteralState;
					skip = true;
					
				}
			}
			if (cstr[index] == '\\') {
				if (stringLiteralState) {
					if (!despecializeState) {
						despecializeState = true;
						skip = true;
					}
					
				}
				
			}
			
		}
		if (stringLiteralState) currentType = ALPHA;
		
		
		if (currentType != state && state != -1) {
			Token t;
			std::string val(buffer);
			t.type = state;
			t.value = val;
			if (t.type != DELIM)output.push_back(t);

			bufferIndex = 0;
			memset(buffer, '\0', string.length() + 1);
		}
		if (!skip) {
			state = currentType;
			buffer[bufferIndex++] = cstr[index];
			despecializeState = false;
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
int Parser::StringLiteralTokenizer::getCharType(char c) {


	if (isalpha(c) || c == '.' || c == '_') {
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

Parser::Parser() {
	_currentState = NULL;
	_globalState = NULL;
	_nextState = NULL;
	_skipLine = false;
}



Parser::State::State(Parser* parser) {
	this->_parent = parser;
}

Parser* Parser::State::getParent() {
	return this->_parent;
}

void Parser::State::start() {
	onStart();
}

void Parser::State::end() {
	onEnd();
}

void Parser::State::onStart() {

}

void Parser::State::onEnd() {

}


bool Parser::requestNextState(State* state) {
	_nextState = state;
	return true;
}
bool Parser::requestNextState(int stateID) {
	_nextState = _stateMap[stateID];
	return true;
}
int Parser::addState(int id, State* state) {
	_stateMap[id] = state;
	return id;
}
Parser::State* Parser::getState(int id) {
	return _stateMap[id];
}
void Parser::setState(State* state) {
	if (_currentState)_currentState->onEnd();
	_currentState = state;
	if (_currentState)_currentState->onStart();
}

void Parser::requestSkipLine() {
	_skipLine = true;
}

int Parser::parseString(string str) {
	return 0;
}
int Parser::parseFile(string filename) {
	ifstream stream;
	StringLiteralTokenizer tokenizer;
	vector<Tokenizer::Token> tokenList;
	stream.open(filename.c_str(), ios::in);

	if (!stream.is_open()) {
		return false;
	}
	bool reading = true;
	string line;
	int lineNumber = 0;
	while (reading) {
		if (stream.eof()) {
			reading = false;
			continue;
		}
		++lineNumber;
		getline(stream, line);
		tokenList = tokenizer.tokenize(line);

		vector<Tokenizer::Token>::iterator lineItor;
		for (lineItor = tokenList.begin(); lineItor != tokenList.end(); lineItor++) {
			if (_currentState) {
				int result = _currentState->handleToken(&(*lineItor));
				if (result) {
					throwError(result, lineNumber, (*lineItor));
					stream.close();
					return 1;
				}
			}
			else {
				int result = _globalState->handleToken(&(*lineItor));
				if (result) {
					throwError(result, lineNumber, (*lineItor));
					stream.close();
					return 1;
				}
			}
			
			handleWaitingState();
			if (handleSkipLine())break;
		}

	}
	stream.close();
	onEOF();
	return 0;
}



void Parser::handleWaitingState() {
	if (_nextState != NULL) {
		setState(_nextState);
		_nextState = NULL;
	}
}

bool Parser::handleSkipLine() {
	bool out = _skipLine;
	_skipLine = false;
	return out;
}

void Parser::throwError(int error, int line, Tokenizer::Token tok) {
	string errorType;
	if (error == 1) {
		errorType = "UNEXPECTED TOKEN";
	}
	cout << "ERROR [" << error << "] on line " << line << ":  " << errorType << ": "<<tok.value << endl;
}
void Parser::onEOF() {

}