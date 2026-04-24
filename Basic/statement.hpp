/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

    Statement();

    virtual ~Statement();

    virtual void execute(EvalState &state, Program &program) = 0;

};

class RemStatement : public Statement {
public:
    RemStatement() {}
    virtual void execute(EvalState &state, Program &program) override;
};

class LetStatement : public Statement {
public:
    LetStatement(std::string var, Expression *exp) : var(var), exp(exp) {}
    virtual ~LetStatement() { delete exp; }
    virtual void execute(EvalState &state, Program &program) override;
private:
    std::string var;
    Expression *exp;
};

class PrintStatement : public Statement {
public:
    PrintStatement(Expression *exp) : exp(exp) {}
    virtual ~PrintStatement() { delete exp; }
    virtual void execute(EvalState &state, Program &program) override;
private:
    Expression *exp;
};

class InputStatement : public Statement {
public:
    InputStatement(std::string var) : var(var) {}
    virtual void execute(EvalState &state, Program &program) override;
private:
    std::string var;
};

class EndStatement : public Statement {
public:
    EndStatement() {}
    virtual void execute(EvalState &state, Program &program) override;
};

class GotoStatement : public Statement {
public:
    GotoStatement(int lineNumber) : lineNumber(lineNumber) {}
    virtual void execute(EvalState &state, Program &program) override;
private:
    int lineNumber;
};

class IfStatement : public Statement {
public:
    IfStatement(Expression *lhs, std::string op, Expression *rhs, int lineNumber)
        : lhs(lhs), op(op), rhs(rhs), lineNumber(lineNumber) {}
    virtual ~IfStatement() { delete lhs; delete rhs; }
    virtual void execute(EvalState &state, Program &program) override;
private:
    Expression *lhs, *rhs;
    std::string op;
    int lineNumber;
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

#endif
