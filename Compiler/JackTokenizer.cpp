#include "JackTokenizer.h"

std::map<std::string, int> JackTokenizer::LEX_KEYWORDS =
{
	{"class", 0},
	{"method", 1},
	{"function", 2},
	{"constructor", 3},
	{"int", 4},
	{"boolean", 5},
	{"char", 6},
	{"void", 7},
	{"var", 8},
	{"static", 9},
	{"field", 10},
	{"let", 11},
	{"do", 12},
	{"if", 13},
	{"else", 14},
	{"while", 15},
	{"return", 16},
	{"true", 17},
	{"false", 18},
	{"null", 19},
	{"this", 20}
};

std::map<std::string, std::string> JackTokenizer::LEX_SYMBOLS =
{
	{"{", "{"},
	{"}", "}"},
	{"(", "("},
	{")", ")"},
	{"[", "["},
	{"]", "]"},
	{".", "."},
	{",", ","},
	{";", ";"},
	{"+", "+"},
	{"-", "-"},
	{"*", "*"},
	{"/", "/"},
	{"&", "&amp;"},
	{"|", "|"},
	{"<", "&lt;"},
	{">", "&gt;"},
	{"=", "="},
	{"~", "~"}
};

JackTokenizer::JackTokenizer(std::string file_name)
{
	curr_line = std::string();
	curr_token = std::string();
	curr_line_ptr = 0;
	regex = "^[a-zA-Z0-9_]+$";

	file.open(file_name);
	if (!file.good()) { 
		std::cerr << "JackTokenizer::JackTokenizer: Inconsistent state of fsstream 'file'!";
		exit(-1);
	}
}

bool JackTokenizer::hasMoreTokens()
{
	return !file.eof() || curr_line.length();
}

void JackTokenizer::advance()
{
	if (!hasMoreTokens()) { return; }

	/* Is there anything left to parse in the current line? */
	if (curr_line.empty() || curr_line.length() == curr_line_ptr)
	{
		curr_line_ptr = 0;
		curr_line = std::string();
		curr_token = std::string();

		std::getline(file >> std::ws, curr_line);

		advance();
		return;
	}

	/* Skip to next line if a) line is empty or b) is line comment */
	if (curr_line.rfind("//", 0) == 0) {
		curr_line = std::string();

		advance();
		return;
	}

	char curr_char = curr_line.at(curr_line_ptr++);

	std::string curr_token_with_char(curr_token);
	curr_token_with_char.push_back(curr_char);

	/** 1.) Check if we currently scan a block comment */
	if (curr_char == '/' && curr_line_ptr < curr_line.length() && curr_line.at(curr_line_ptr) == '*') {
		is_in_block_comment = true;
		advance();
		return;
	}
	else if (is_in_block_comment)
	{
		// Did we reach the end of the block comment?
		if ((curr_char == '*' && curr_line_ptr < curr_line.length() && (curr_line.at(curr_line_ptr) == '/')))
		{
			is_in_block_comment = false;
			curr_line_ptr++;
		}

		advance();
		return;
	}

	/** 2.) Check if all characters up to this one are numbers */
	if (curr_token_with_char.find_first_not_of("0123456789") == std::string::npos) {
		token_type = JACK_TOKEN_TYPE::INT_CONST;
		token_val = std::stoi(curr_token_with_char);
		curr_token.push_back(curr_char);

		advance();
		return;
	}
	// If all read characters but the previous are numbers, save number and set line pointer back by one
	else if (!curr_token.empty() && curr_token.find_first_not_of("0123456789") == std::string::npos)
	{
		token_type = JACK_TOKEN_TYPE::INT_CONST;
		token_val = std::stoi(curr_token);
		curr_line_ptr--;
		token_raw = curr_token;
		curr_token = std::string();

		return;
	}

	/** 3.) Check if currently read in a stringConstant */
	if (curr_token_with_char.find_last_of('\"') == 0)
	{
		token_type = JACK_TOKEN_TYPE::STRING_CONST;
		token_stringVal = curr_token_with_char;
		curr_token.push_back(curr_char);

		advance();
		return;
	}
	else if (curr_token_with_char.back() == '\"') {
		token_stringVal = curr_token_with_char.substr(curr_token_with_char.find_first_not_of('\"'), curr_token_with_char.find_last_not_of('\"'));
		token_raw = curr_token;
		curr_token = std::string();

		return;
	}

	/** 4.) Check if we found a keyword */
	auto keyword_it = LEX_KEYWORDS.find(curr_token_with_char);
	if (keyword_it != LEX_KEYWORDS.end())
	{
		token_type = JACK_TOKEN_TYPE::KEYWORD;
		token_keyword = JACK_KEYWORD(keyword_it->second);
		token_raw = keyword_it->first;
		curr_token = std::string();

		return;
	}

	/** 5.) Check if we found an identifier */
	if (std::regex_match(curr_token_with_char, regex)) {
		token_type = JACK_TOKEN_TYPE::IDENTIFIER;
		token_identifier = curr_token_with_char;
		curr_token.push_back(curr_char);

		advance();
		return;
	}
	else if (std::regex_match(curr_token, regex)) {
		token_type = JACK_TOKEN_TYPE::IDENTIFIER;
		token_identifier = curr_token;
		curr_line_ptr--;
		token_raw = curr_token;
		curr_token = std::string();

		return;
	}

	/** 6.) Check if we found a symbol */
	// We first need to check if the symbol marks a comment
	if (curr_char == '/' && curr_line_ptr < curr_line.length() && curr_line.at(curr_line_ptr) == '/')
	{
		curr_line = std::string();
		advance();
		return;
	}
	else {
		auto symbol_it = LEX_SYMBOLS.find(curr_token_with_char);
		if (symbol_it != LEX_SYMBOLS.end())
		{
			token_type = JACK_TOKEN_TYPE::SYMBOL;
			curr_symbol = curr_char;
			token_raw = curr_char;
			curr_token = std::string();
			
			return;
		}
	}

	if (iswblank(curr_char) || iswspace(curr_char))
	{
		advance();
	}

	return;
}

JackTokenizer::JACK_TOKEN_TYPE JackTokenizer::tokenType()
{
	return token_type;
}

JackTokenizer::JACK_KEYWORD JackTokenizer::keyWord()
{
	return token_keyword;
}

char JackTokenizer::symbol()
{
	return curr_symbol;
}

std::string JackTokenizer::identifier()
{
	return token_identifier;
}

int JackTokenizer::intVal()
{
	return token_val;
}

std::string JackTokenizer::stringVal()
{
	return token_stringVal;
}

std::string JackTokenizer::tokenRaw()
{
	return token_raw;
}

std::string JackTokenizer::resolveSymbol(std::string symbol)
{
	auto symbol_it = LEX_SYMBOLS.find(symbol);
	return symbol_it->second;
}
