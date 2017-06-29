/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface specified in Chapter 17, which is an
 * excellent model for the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include"program.h"
#include<sstream>
#include "evalstate.h"
#include "exp.h"
#include "parser.h"
#include<unordered_set>
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/error.h"

 /*
  * Class: Statement
  * ----------------
  * This class is used to represent a statement in a program.
  * The model for this class is Expression in the exp.h interface.
  * Like Expression, Statement is an abstract class with subclasses
  * for each of the statement and command types required for the
  * BASIC interpreter.
  */

//变量说明：
//keep_words里面存的是保留字符，可以用find方法判断一个string是否是保留字符
extern unordered_set<string> keep_words;

//函数说明：
//word_is_num返回bool类型，当且仅当参数word是整数（包括负数）时返回true
extern bool word_is_num(string word);

class Statement {

public:

	/*
	 * Constructor: Statement
	 * ----------------------
	 * The base class constructor is empty.  Each subclass must provide
	 * its own constructor.
	 */

	Statement();

	/*
	 * Destructor: ~Statement
	 * Usage: delete stmt;
	 * -------------------
	 * The destructor deallocates the storage for this expression.
	 * It must be declared virtual to ensure that the correct subclass
	 * destructor is called when deleting a statement.
	 */

	virtual ~Statement();

	/*
	 * Method: execute
	 * Usage: stmt->execute(state);
	 * ----------------------------
	 * This method executes a BASIC statement.  Each of the subclasses
	 * defines its own execute method that implements the necessary
	 * operations.  As was true for the expression evaluator, this
	 * method takes an EvalState object for looking up variables or
	 * controlling the operation of the interpreter.
	 */

	virtual void execute(EvalState & state) = 0;

	//函数说明：
	//asString返回一行语句的string形式
	//标准格式：
	//10 LET X = 5
	virtual string asString() { return original_exp; }

	//变量说明：
	//original_exp存储的格式与用户输入的一行完全一致
	//格式：
	//10 LET X = 5
	string original_exp;

	//变量说明：
	//line_number存储的是行数
	int line_number;
	
};

/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

 //This statement is used for comments
class RemStmt :public Statement
{
public:
	RemStmt();
	//以10  REM  this is a comment格式来初始化
	RemStmt(string line, EvalState & state);
	//执行这个语句不做任何事情
	void execute(EvalState & state) {}
	~RemStmt() = default;


private:
	//数据说明:
	//line number行号
	//comments存储的是注释的内容
	//identify存储的是标识符
	int line_number = 0;
	string comments;
	string identify = "REM";

};

class LetStmt :public Statement
{
public:
	//根据11 LET T=10 格式或者LET X=1格式来初始化
	LetStmt(string line, EvalState & state);
	//执行这个语句会创建一个相应的变量/给相应变量重新赋值
	void execute(EvalState & state) override;
private:
	//数据说明：
	//inner_expression是指向赋值的expression的指针
	int line_number = 0;
	string identify = "LET";
	Expression* inner_expression;

};
class PrintStmt :public Statement
{
public:
	//格式：
	//PRINT X
	//10 PRINT X
	PrintStmt(string line, EvalState& state);
	//执行会输出对应的变量值
	void execute(EvalState & state) override;

private:
	int line_number = 0;
	Expression* inner_expression;
	string identify = "PRINT";
};

class InputStmt :public Statement
{
public:
	//格式：
	//INPUT X
	//11 INPUT X
	InputStmt(string line, EvalState& state);
	//执行：
	//输出" ？"并要求用户输入，只有接受了整数输入才能继续运行
	//同时会创建相应的变量
	void execute(EvalState & state) override;

private:
	//数据说明：
	//var_name存储的是将要定义的变量名
	int line_number = 0;
	string var_name;
	Expression* inner_expression;
	string identify = "INPUT";
};

class GotoStmt :public Statement
{
public:
	//格式：10 GOTO 20
	GotoStmt(string line, EvalState& state);
	//执行：对target_line进行检查，如果合法则修改当前访问的行数为target_line
	void execute(EvalState& state) override;

private:
	int target_line;
	int line_number;
	string identify;
};

class EndStmt : public Statement
{
public:
	//格式：
	//10 END
	EndStmt(string line, EvalState& state);
	void execute(EvalState& state);

private:
	int line_number;
	string identify;
};

class IfStmt : public Statement
{
public:
	//格式：
	//10 IF x>1 then 20
	//20 IF X + 1 = 3 THEN 40
	IfStmt(string line, EvalState& state);
	void execute(EvalState& state);

private:
	int line_number;
	//数据说明：
	//identify_if是"IF"
	//identify_then是"THEN"
	//if_exp1是判断的前部分，if_exp2是判断的后部分
	//char cmp是比较符号 
	//target_line是目标行数
	string identify_if;
	string identify_then;
	Expression* if_exp1;
	Expression* if_exp2;
	char cmp;
	int target_line;
};



#endif
