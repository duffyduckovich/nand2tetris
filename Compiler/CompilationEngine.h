#pragma once

#include "JackTokenizer.h"
#include <set>

class CompilationEngine
{
public:
	CompilationEngine(std::string input_file, std::string output_file);

	static std::set<std::string> KEYWORDS;
	static std::set<std::string> UNARY_OPERATORS;
	static std::set<std::string> BINARY_OPERATORS;

	static const char COMMA_DELIMITER = ',';
	static const char OPENING_PARAN = '(';
	static const char OPENING_BRACKET = '[';
	static const char DOT_DELIMITER = '.';

	void CompileClass();
	void CompileClassVarDec();
	void CompileSubroutine();
	void compileParameterList();
	void compileVarDec();
	void compileStatements();
	void compileDo();
	void compileLet();
	void compileWhile();
	void compileReturn();
	void compileIf();
	void CompileExpression();
	void CompileTerm();
	void CompileExpressionList();

	void close();

private:
	bool test_is(JackTokenizer::JACK_TOKEN_TYPE type);
	bool test_is(JackTokenizer::JACK_TOKEN_TYPE type, JackTokenizer::JACK_KEYWORD key);
	void next_token();

	void write_xml(std::string tag);
	void write_xml(std::string tag, std::string token);
	void write_xml(std::string tag, char c);
	void write_xml(std::string tag, int val);

	std::ofstream output;
	JackTokenizer tokenizer;
};

