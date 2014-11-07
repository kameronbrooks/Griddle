#ifndef PARSER 
#define PARSER
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>

using namespace std;

class Parser {
private:

protected:
	

public:
	class Tokenizer {
	public:
		Tokenizer();

		string operators;
		string extendedAlphaChars;

		class Token {
		public:
			int type;
			string value;
		};
		enum {
			ALPHA = 0,
			NUMERIC = 1,
			OPERATOR = 2,
			DELIM = 3
		};

		virtual vector<Token> tokenize(string str);
		virtual int getCharType(char c);
	};

	
	class StringLiteralTokenizer : public Tokenizer {
	public:
		vector<Token> tokenize(string str);
		int getCharType(char c);
	};
	class State {
	protected:
		Parser* _parent;

	public:
		enum {
			SUCCESS = 0,
			ERROR_UNEXPECTED_TOKEN = 1
		};
		State(Parser* parser);
		void start();
		virtual void onStart();
		void end();
		virtual void onEnd();
		virtual State* getNextState() = 0;
		virtual int handleToken(Tokenizer::Token* token) = 0;
		Parser* getParent();
	};

	Parser();

	bool requestNextState(State* state);
	bool requestNextState(int stateID);
	void setState(State* state);

	int parseString(string str);
	int parseFile(string filename);

	void requestSkipLine();

	int addState(int id, State* state);
	State* Parser::getState(int id);
protected:
	
	State* _globalState;
	State* _currentState;
	State* _nextState;
	map<int, State*> _stateMap;

	bool _skipLine;

	void handleWaitingState();
	bool handleSkipLine();
	virtual void throwError(int error, int line, Tokenizer::Token tok);
	virtual void onEOF();

};


#endif