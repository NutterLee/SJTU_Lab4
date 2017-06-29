/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include"statement.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */


/* Main program */
//全局变量数据说明：
//program 存储整个程序的变量
Program program;
//state 存储的是当前的声明的变量和相应变量名
EvalState state;
//this_line存储的是当前要运行的行数
int this_line;

int main() {

	//cout << "Stub implementation of BASIC" << endl;
	while (true) {
		try {
			//true program starts here
			string line;
			getline(cin, line);
			//先对输入进行判断是否是命令
			if (line == "RUN")
			{
				this_line = program.getFirstLineNumber();
				while (this_line != -1)
				{
					int line_record = this_line;
					(program.getParsedStatement(this_line))->execute(state);
					if (this_line == line_record) this_line = program.getNextLineNumber(this_line);
					//else (program.getParsedStatement(this_line))->execute(state);
				}
			}
			else if (line == "LIST")
			{
				int this_line = program.getFirstLineNumber();
				while (this_line != -1)
				{
					cout << (program.getParsedStatement(this_line))->asString() << endl;
					this_line = program.getNextLineNumber(this_line);
				}
			}
			else if (line == "CLEAR")
			{
				program.clear();
				state.clear();
			}
			else if (line == "QUIT")
			{
				exit(0);
			}
			else if (line == "HELP")
			{ //TODO 完成帮助信息
				cout << "this is help message" << endl;
			}
			else
				//能运行到这里说明要么是错误的输入，要么是basic语句了
				//先尝试判断语句的类型，对应执行相应的构造函数，最后都没有找到对应则报错
			{
				int line_number;
				stringstream tmp_flow;
				tmp_flow << line;
				tmp_flow >> line_number;
				//如果输入的是纯数字，按照批改程序应为删除对应行
				if (tmp_flow.str() == std::to_string(line_number))
				{
					program.removeSourceLine(line_number);
					continue;
				}
				//不是以数字开头的 说明是要直接运行的或者是输入错误的
				if (line[0]<'0' || line[0]>'9')
				{
					if (line.find("LET") != string::npos)
					{
						Statement *tmp_stmt = new LetStmt(line, state);
						tmp_stmt->execute(state);
						delete tmp_stmt;
					}
					else if (line.find("PRINT") != string::npos)
					{
						Statement *tmp_stmt = new PrintStmt(line, state);
						tmp_stmt->execute(state);
						delete tmp_stmt;
					}
					else if (line.find("INPUT") != string::npos)
					{
						Statement *tmp_stmt = new InputStmt(line, state);
						tmp_stmt->execute(state);
						delete tmp_stmt;
					}
					else error("SYNTAX ERROR");

				}
				else
				{
						program.addSourceLine(line_number, line);
				}
			}
		}
		catch (ErrorException & ex) {
			cout << ex.getMessage() << endl;
		}
	}
	return 0;
}
