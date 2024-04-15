#pragma once

#include <map>
#include <regex>
#include <fstream>
#include <iostream>

class JackTokenizer
{
public:
	static std::map<std::string, int> LEX_KEYWORDS;
	static std::map<std::string, std::string> LEX_SYMBOLS;

	enum class JACK_TOKEN_TYPE
	{
		KEYWORD,
		SYMBOL,
		IDENTIFIER,
		INT_CONST,
		STRING_CONST,
	};

	enum class JACK_KEYWORD
	{
		CLASS,
		METHOD,
		FUNCTION,
		CONSTRUCTOR,
		INT,
		BOOLEAN,
		CHAR,
		VOID,
		VAR,
		STATIC,
		FIELD,
		LET,
		DO,
		IF,
		ELSE,
		WHILE,
		RETURN,
		TRUE,
		FALSE,
		NUL,	// Can't be NULL
		THIS
	};


	JackTokenizer(std::string file_name);

	int intVal();
	char symbol();
	void advance();
	bool hasMoreTokens();
	std::string tokenRaw();
	JACK_KEYWORD keyWord();
	std::string stringVal();
	std::string identifier();
	JACK_TOKEN_TYPE tokenType();
	std::string resolveSymbol(std::string symbol);

private:
	char curr_symbol;
	std::regex regex;
	unsigned int curr_line_ptr;
	std::ifstream file;
	std::string curr_line;
	std::string curr_token;
	bool is_in_block_comment;

	int token_val;
	JACK_KEYWORD token_keyword;
	JACK_TOKEN_TYPE token_type;
	std::string token_stringVal;
	std::string token_identifier;
	std::string token_raw;
};

