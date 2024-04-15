#include "CompilationEngine.h"

std::set<std::string> CompilationEngine::BINARY_OPERATORS =
{
	{"+"},
	{"-"},
	{"*"},
	{"/"},
	{"&"},
	{"|"},
	{"<"},
	{">"},
	{"="}
};

std::set<std::string> CompilationEngine::UNARY_OPERATORS =
{
	{"-"},
	{"~"}
};

std::set<std::string> CompilationEngine::KEYWORDS =
{
	{"true"},
	{"false"},
	{"null"},
	{"this"}
};

CompilationEngine::CompilationEngine(std::string input_file, std::string output_file) : tokenizer(input_file)
{
	output.open(output_file);
	if (!output.good()) { 
		std::cerr << "CompilationEngine::CompilationEngine: Inconsistent state of fsstream 'output'!"; 
	}
}

void CompilationEngine::CompileClass()
{
	next_token();

	// Class open tag
	write_xml("class");

	// Class keyword
	write_xml("keyword", tokenizer.tokenRaw());
	next_token();

	// identifier 
	write_xml("identifier", tokenizer.identifier());
	next_token();

	// Open curly
	write_xml("symbol", tokenizer.symbol());
	next_token();

	// Variable declaration(s)
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::STATIC) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::FIELD))
	{
		CompileClassVarDec();
	}

	// Function declaration(s)
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::CONSTRUCTOR) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::FUNCTION) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::METHOD))
	{
		CompileSubroutine();

	}

	// Close curly
	write_xml("symbol", tokenizer.symbol());
	next_token();

	// Class close tag
	write_xml("/class");
}

void CompilationEngine::CompileClassVarDec()
{
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::STATIC) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::FIELD))
	{
		write_xml("classVarDec");

		write_xml("keyword", tokenizer.tokenRaw());
		next_token();

		// We can either encounter a build-in type, or an indentifier as a type.
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::INT) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::BOOLEAN) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::CHAR))
		{
			write_xml("keyword", tokenizer.tokenRaw());
			next_token();
		}
		else if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		bool is_multiple = test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == COMMA_DELIMITER;
		while (is_multiple)
		{
			write_xml("symbol", tokenizer.symbol());
			next_token();

			write_xml("identifier", tokenizer.identifier());
			next_token();

			is_multiple = test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == COMMA_DELIMITER;
		}

		write_xml("symbol", tokenizer.symbol());
		next_token();

		write_xml("/classVarDec");
		CompileClassVarDec();
	}

	return;
}

void CompilationEngine::CompileSubroutine()
{
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::CONSTRUCTOR) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::FUNCTION) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::METHOD))
	{
		/** Function signature */
		write_xml("subroutineDec");

		write_xml("keyword", tokenizer.tokenRaw());
		next_token();

		// Function type
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::INT) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::CHAR) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::BOOLEAN) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::VOID))
		{
			write_xml("keyword", tokenizer.tokenRaw());
			next_token();
		}
		else if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		// Function name
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		// Parameters
		write_xml("symbol", tokenizer.symbol());
		next_token();

		write_xml("parameterList");
		compileParameterList();
		write_xml("/parameterList");

		write_xml("symbol", tokenizer.symbol());
		next_token();

		/** Function body */
		write_xml("subroutineBody");

		write_xml("symbol", tokenizer.symbol());
		next_token();

		compileVarDec();			// Function variables

		write_xml("statements");
		compileStatements();		// Function statements
		write_xml("/statements");

		write_xml("symbol", tokenizer.symbol());
		next_token();

		/** End Function body */
		write_xml("/subroutineBody");

		/** Function signature */
		write_xml("/subroutineDec");

		CompileSubroutine();
	}

	return;
}

void CompilationEngine::compileParameterList()
{
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::INT) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::BOOLEAN) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::CHAR) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
	{
		write_xml("keyword", tokenizer.tokenRaw());
		next_token();

		write_xml("identifier", tokenizer.identifier());
		next_token();


		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == COMMA_DELIMITER)
		{
			write_xml("symbol", tokenizer.symbol());
			next_token();

			compileParameterList();
		}
	}
}

void CompilationEngine::compileVarDec()
{

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::VAR))
	{
		write_xml("varDec");

		write_xml("keyword", tokenizer.tokenRaw());
		next_token();

		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::INT) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::BOOLEAN) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::CHAR))
		{
			write_xml("keyword", tokenizer.tokenRaw());
			next_token();
		}
		else if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		while (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == COMMA_DELIMITER)
		{
			write_xml("symbol", tokenizer.symbol());
			next_token();

			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		write_xml("symbol", tokenizer.symbol());
		next_token();

		write_xml("/varDec");

		compileVarDec();
		return;
	}
}

void CompilationEngine::compileStatements()
{
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::LET))
	{
		compileLet();
		compileStatements();
	}
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::IF))
	{
		compileIf();
		compileStatements();
	}
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::WHILE))
	{
		compileWhile();
		compileStatements();
	}
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::DO))
	{
		compileDo();
		compileStatements();
	}
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::RETURN))
	{
		compileReturn();
		compileStatements();
	}
}

void CompilationEngine::compileDo()
{
	write_xml("doStatement");

	// Class keyword
	write_xml("keyword", tokenizer.tokenRaw());
	next_token();

	write_xml("identifier", tokenizer.identifier());
	next_token();

	// Is function call on object?
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == DOT_DELIMITER)
	{
		write_xml("symbol", tokenizer.symbol());
		next_token();
	}

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
	{
		write_xml("identifier", tokenizer.identifier());
		next_token();
	}

	// Paranthesis
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == OPENING_PARAN)
	{
		write_xml("symbol", tokenizer.symbol());
		next_token();

		CompileExpressionList();

		write_xml("symbol", tokenizer.symbol());
		next_token();
	}

	write_xml("symbol", tokenizer.symbol());
	next_token();

	write_xml("/doStatement");
}

void CompilationEngine::compileWhile()
{
	write_xml("whileStatement");

	write_xml("keyword", tokenizer.tokenRaw());
	next_token();

	// Paranthesis
	write_xml("symbol", tokenizer.symbol());
	next_token();

	CompileExpression();

	write_xml("symbol", tokenizer.symbol());
	next_token();

	// Body
	write_xml("symbol", tokenizer.symbol());
	next_token();

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::LET) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::IF) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::WHILE) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::DO) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::RETURN))
	{
		write_xml("statements");
		compileStatements();
		write_xml("/statements");
	}

	write_xml("symbol", tokenizer.symbol());
	next_token();

	write_xml("/whileStatement");
}

void CompilationEngine::compileReturn()
{
	write_xml("returnStatement");

	write_xml("keyword", tokenizer.tokenRaw());
	next_token();

	CompileExpression();

	write_xml("symbol", tokenizer.symbol());
	next_token();

	write_xml("/returnStatement");
}

void CompilationEngine::compileIf()
{
	write_xml("ifStatement");

	write_xml("keyword", tokenizer.tokenRaw());
	next_token();

	// Paranthesis
	write_xml("symbol", tokenizer.symbol());
	next_token();

	CompileExpression();

	write_xml("symbol", tokenizer.symbol());
	next_token();

	// Body
	write_xml("symbol", tokenizer.symbol());
	next_token();

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::LET) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::IF) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::WHILE) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::DO) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::RETURN))
	{
		write_xml("statements");
		compileStatements();
		write_xml("/statements");
	}

	write_xml("symbol", tokenizer.symbol());
	next_token();

	// Else clause
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::ELSE))
	{
		write_xml("keyword", tokenizer.tokenRaw());
		next_token();

		// Body
		write_xml("symbol", tokenizer.symbol());
		next_token();

		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::LET) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::IF) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::WHILE) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::DO) ||
			test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD, JackTokenizer::JACK_KEYWORD::RETURN))
		{
			write_xml("statements");
			compileStatements();
			write_xml("/statements");
		}

		write_xml("symbol", tokenizer.symbol());
		next_token();
	}

	write_xml("/ifStatement");
}

void CompilationEngine::compileLet()
{
	write_xml("letStatement");

	write_xml("keyword", tokenizer.tokenRaw());
	next_token();

	write_xml("identifier", tokenizer.identifier());
	next_token();

	// Is accessing array index?
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == OPENING_BRACKET)
	{
		write_xml("symbol", tokenizer.symbol());
		next_token();

		CompileExpression();

		write_xml("symbol", tokenizer.symbol());
		next_token();
	}

	// =
	write_xml("symbol", tokenizer.symbol());
	next_token();

	CompileExpression();

	// ;
	write_xml("symbol", tokenizer.symbol());
	next_token();

	write_xml("/letStatement");
}

void CompilationEngine::CompileExpression()
{
	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::INT_CONST) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::STRING_CONST) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD) && KEYWORDS.count(tokenizer.tokenRaw()) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && (BINARY_OPERATORS.count(tokenizer.tokenRaw()) || UNARY_OPERATORS.count(tokenizer.tokenRaw())) ||
		test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == OPENING_PARAN)
	{
		write_xml("expression");

		CompileTerm();

		while (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && (BINARY_OPERATORS.count(tokenizer.tokenRaw()) || UNARY_OPERATORS.count(tokenizer.tokenRaw())))
		{
			write_xml("symbol", tokenizer.resolveSymbol(tokenizer.tokenRaw()));
			next_token();

			CompileTerm();
		}

		write_xml("/expression");
	}
}

void CompilationEngine::CompileTerm()
{
	write_xml("term");

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && UNARY_OPERATORS.count(tokenizer.tokenRaw()))
	{
		write_xml("symbol", tokenizer.resolveSymbol(tokenizer.tokenRaw()));
		next_token();

		CompileTerm();
	}

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == OPENING_PARAN)
	{
		write_xml("symbol", tokenizer.symbol());
		next_token();

		CompileExpression();

		write_xml("symbol", tokenizer.symbol());
		next_token();
	}

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::INT_CONST))
	{
		write_xml("integerConstant", tokenizer.intVal());
		next_token();
	}

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::STRING_CONST))
	{
		write_xml("stringConstant", tokenizer.stringVal());
		next_token();
	}

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::KEYWORD) && KEYWORDS.count(tokenizer.tokenRaw()))
	{
		write_xml("keyword", tokenizer.tokenRaw());
		next_token();
	}

	if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
	{
		write_xml("identifier", tokenizer.identifier());
		next_token();

		// []
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == OPENING_BRACKET)
		{
			write_xml("symbol", tokenizer.symbol());
			next_token();

			CompileExpression();

			write_xml("symbol", tokenizer.symbol());
			next_token();
		}

		// .
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == DOT_DELIMITER)
		{
			write_xml("symbol", tokenizer.symbol());
			next_token();
		}

		// varName
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::IDENTIFIER))
		{
			write_xml("identifier", tokenizer.identifier());
			next_token();
		}

		// ()
		if (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == OPENING_PARAN)
		{
			write_xml("symbol", tokenizer.symbol());
			next_token();

			CompileExpressionList();

			write_xml("symbol", tokenizer.symbol());
			next_token();
		}
	}

	write_xml("/term");
}

void CompilationEngine::CompileExpressionList()
{
	write_xml("expressionList");

	CompileExpression();

	while (test_is(JackTokenizer::JACK_TOKEN_TYPE::SYMBOL) && tokenizer.symbol() == COMMA_DELIMITER)
	{
		write_xml("symbol", tokenizer.symbol());
		next_token();

		CompileExpression();
	}

	write_xml("/expressionList");
}

void CompilationEngine::close()
{
	output.close();
}

/*********************************************************************************************************
*
*  Helper
*
**********************************************************************************************************/

bool CompilationEngine::test_is(JackTokenizer::JACK_TOKEN_TYPE type)
{
	if (type != tokenizer.tokenType())
	{
		return false;
	}
	return true;
}

bool CompilationEngine::test_is(JackTokenizer::JACK_TOKEN_TYPE type, JackTokenizer::JACK_KEYWORD key)
{
	if (type != tokenizer.tokenType() || key != tokenizer.keyWord())
	{
		return false;
	}
	return true;
}

void CompilationEngine::next_token()
{
	tokenizer.advance();
}

void CompilationEngine::write_xml(std::string tag)
{
	output << "<" << tag << ">" << "\n";
}

void CompilationEngine::write_xml(std::string tag, std::string token)
{
	output << "<" << tag << "> " << token << " </" << tag << ">" << "\n";
}

void CompilationEngine::write_xml(std::string tag, char c)
{
	output << "<" << tag << "> " << c << " </" << tag << ">" << "\n";
}

void CompilationEngine::write_xml(std::string tag, int val)
{
	output << "<" << tag << "> " << val << " </" << tag << ">" << "\n";
}