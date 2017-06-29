/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "statement.h"
using namespace std;

Program::Program() {
}

Program::~Program() {   
}

void Program::clear() {   
	inner_programs.clear();
}

void Program::addSourceLine(int lineNumber, string line) {
	if (line.find("REM") != string::npos)
	{
		Statement* tmp_stmt = new RemStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else if (line.find("LET") != string::npos)
	{
		Statement* tmp_stmt = new LetStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else if (line.find("IF") != string::npos)
	{
		Statement* tmp_stmt = new IfStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else if (line.find("PRINT") != string::npos)
	{
		Statement* tmp_stmt = new PrintStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else if (line.find("GOTO") != string::npos)
	{
		Statement* tmp_stmt = new GotoStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else if (line.find("INPUT") != string::npos)
	{
		Statement* tmp_stmt = new InputStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else if (line.find("END") != string::npos)
	{
		Statement* tmp_stmt = new EndStmt(line, state);
		inner_programs[lineNumber] = tmp_stmt;
	}
	else error("SYNTAX ERROR");

}

void Program::removeSourceLine(int lineNumber) {
	map<int,Statement*>::iterator map_iter = inner_programs.find(lineNumber);
	if (map_iter == inner_programs.end()) return;
	else inner_programs.erase(map_iter);
}

string Program::getSourceLine(int lineNumber) {
   return "";   
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
	map<int, Statement*>::iterator  map_iter = inner_programs.find(lineNumber);
	if (map_iter == inner_programs.end()) error("LINE NUMBER ERROR");
	else inner_programs[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
	map<int, Statement*>::iterator  map_iter = inner_programs.find(lineNumber);
	if (map_iter == inner_programs.end())return NULL;
	else return inner_programs[lineNumber];
}

int Program::getFirstLineNumber() {
	if (inner_programs.size() == 0)return -1;
	else
	{
		map<int, Statement*>::iterator  map_iter = inner_programs.begin();
		return map_iter->first;
	}   
}

int Program::getNextLineNumber(int lineNumber) {
	//check the line number is valid
	if (inner_programs.find(lineNumber) == inner_programs.end()) error("LINE NUMBER ERROR");
	else
	{
		map<int, Statement*>::iterator  map_iter = inner_programs.find(lineNumber);
		map_iter++;
		//check any statement remain
		if (map_iter == inner_programs.end()) return -1;
		else return map_iter->first;
	}
}
