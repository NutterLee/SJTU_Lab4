/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"

using namespace std;
unordered_set<string>keep_words = { "LET","REM","PRINT","IF","THEN","INPUT","END","GOTO","RUN","LIST","CLEAR","QUIT","HELP" };
/* Implementation of the Statement class */
bool word_is_num(string word)
{
	for (int i = 0; i < word.size(); i++)
	{
		if (i == 0 && word[0] == '-') continue;
		if (!isdigit(word[i])) return false;
	}
	return true;
}
Statement::Statement() {
	/* Empty */
}

Statement::~Statement() {
	/* Empty */
}

RemStmt::RemStmt() :line_number(0)
{
}

RemStmt::RemStmt(string line, EvalState& state)
{
	stringstream tmp_flow;
	tmp_flow << line;
	tmp_flow >> line_number >> identify;
	comments = line.substr(std::to_string(line_number).size() + identify.size() + 1);
	if (identify != "REM") error("SYNTAX ERROR");
	original_exp = line;
}

LetStmt::LetStmt(string line, EvalState & state)
{
	stringstream tmp_flow;
	string tmp_exp;
	tmp_flow << line;
	//看是否是以行号开头的
	if (line[0] >= '0'&&line[0] <= '9')
	{
		tmp_flow >> line_number >> identify;
		tmp_exp = line.substr(std::to_string(line_number).size() + identify.size() + 1);
	}
	else
	{
		line_number = -1;
		tmp_flow >> identify;
		tmp_exp = line.substr(identify.size() + 1);
	}
	original_exp = line;
	if (identify != "LET") error("SYNTAX ERROR");
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(tmp_exp);
	Expression *exp = parseExp(scanner);
	inner_expression = exp;
}

void LetStmt::execute(EvalState & state)
{
	int value = inner_expression->eval(state);
}

PrintStmt::PrintStmt(string line, EvalState & state)
{
	stringstream tmp_flow;
	string tmp_exp;
	tmp_flow << line;
	if (line[0] >= '0'&&line[0] <= '9')
	{
	
		tmp_flow >> line_number >> identify;
		tmp_exp = line.substr(std::to_string(line_number).size() + identify.size() + 1);
	}
	else
	{
		line_number = -1;
		tmp_flow >> identify;
		tmp_exp = line.substr(identify.size() + 1);
	}
	if (identify != "PRINT") error("SYNTAX ERROR");
	original_exp = line;
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(tmp_exp);
	Expression *exp = parseExp(scanner);
	inner_expression = exp;
}

void PrintStmt::execute(EvalState & state)
{
	int value = inner_expression->eval(state);
	cout << value << endl;
}

InputStmt::InputStmt(string line, EvalState & state)
{
	stringstream tmp_flow;
	string tmp_exp;
	tmp_flow << line;
	if (line[0] >= '0'&&line[0] <= '9')
	{
		tmp_flow >> line_number >> identify >> var_name;
	}
	else
	{
		line_number = -1;
		tmp_flow >> identify >> var_name;
	}
	if (identify != "INPUT") error("SYNTAX ERROR");
	original_exp = line;
}

void InputStmt::execute(EvalState & state)
{
	bool success = false;	
	string value;
	while (!success)
	{
		try
		{
			cout << " ? ";
			getline(cin, value);
			if (!word_is_num(value)) error("INVALID NUMBER");
			else success = true;
		}
		catch (ErrorException & ex) {
			cout << ex.getMessage() << endl;
		}
	}

	string tmp_exp = var_name + "=" + value;
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(tmp_exp);
	inner_expression = parseExp(scanner);
	inner_expression->eval(state);
}

GotoStmt::GotoStmt(string line, EvalState & state)
{
	stringstream tmp_flow;
	string tmp_exp;
	tmp_flow << line;
	if (line[0] >= '0'&&line[0] <= '9')
	{
		tmp_flow >> line_number >> identify >> target_line;
		if (cin.fail()) error("SYNTAX ERROR");
	}
	else error("LINE NUMBER ERROR");

	if (identify != "GOTO") error("SYNTAX ERROR");
	original_exp = line;
}

void GotoStmt::execute(EvalState & state)
{
	Statement* tmp_stmt = program.getParsedStatement(target_line);
	if (tmp_stmt == NULL) error("LINE NUMBER ERROR");
	this_line = target_line;
}

EndStmt::EndStmt(string line, EvalState& state)
{
	stringstream tmp_flow;
	tmp_flow << line;
	tmp_flow >> line_number >> identify;
	if (identify != "END" || cin.fail()) error("SYNAX ERROR");
	original_exp = line;
}

void EndStmt::execute(EvalState & state)
{
	this_line = -1;
	return;
}

IfStmt::IfStmt(string line, EvalState & state)
{
	stringstream tmp_flow;
	tmp_flow << line;
	string tmp_if_exp1 = "";
	string tmp_if_exp2 = "";
	tmp_flow >> line_number >> identify_if;
	if (cin.fail() || identify_if != "IF") error("SYNTAX ERROR");	
	string tmp_exp = line.substr(std::to_string(line_number).size() + 3);
	int pos_cmp = 0;
	int pos_then = tmp_exp.find("THEN");
	if (pos_then == string::npos) error("SYNTAX ERROR");
	for (char c : tmp_exp)
	{
		pos_cmp++;
		if (c == '>')
		{
			cmp = '>';
			break;
		}
		else if (c == '=')
		{
			cmp = '=';
			break;
		}
		else if (c == '<')
		{
			cmp = '<';
			break;
		}
		else tmp_if_exp1 = tmp_if_exp1 + c;
	}
	tmp_if_exp2 = tmp_exp.substr(pos_cmp, pos_then - pos_cmp);
	original_exp = line;
	target_line = stoi(tmp_exp.substr(pos_then + 5));
	TokenScanner scanner;
	scanner.ignoreWhitespace();
	scanner.scanNumbers();
	scanner.setInput(tmp_if_exp1);
	if_exp1 = parseExp(scanner);

	if (if_exp1 == NULL) error("ifstmt-1 reports an error");
	TokenScanner scanner2;
	scanner2.ignoreWhitespace();
	scanner2.scanNumbers();
	scanner2.setInput(tmp_if_exp2);
	if_exp2 = parseExp(scanner2);

	if (if_exp2 == NULL) error("ifstmt-2 reports an error");
}

void IfStmt::execute(EvalState & state)
{
	bool cmp_result;
	int value1 = if_exp1->eval(state);
	int value2 = if_exp2->eval(state);
	switch (cmp)
	{
	case '=': cmp_result = (value1 == value2); break;
	case '>': cmp_result = (value1 > value2); break;
	case'<':cmp_result = (value1 < value2); break;
	default:error("SYNTAX ERROR");
	}
	if (cmp_result)
	{
		Statement* tmp_stmt = program.getParsedStatement(target_line);
		if (tmp_stmt == NULL) error("LINE NUMBER ERROR");
		this_line = target_line;
	}
	else return;

}
