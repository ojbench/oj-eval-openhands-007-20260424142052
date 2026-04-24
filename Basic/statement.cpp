/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "program.hpp"
#include "statement.hpp"

Statement::Statement() = default;

Statement::~Statement() = default;

void RemStatement::execute(EvalState &state, Program &program) {}

void LetStatement::execute(EvalState &state, Program &program) {
    state.setValue(var, exp->eval(state));
}

void PrintStatement::execute(EvalState &state, Program &program) {
    std::cout << exp->eval(state) << std::endl;
}

void InputStatement::execute(EvalState &state, Program &program) {
    while (true) {
        std::cout << " ? ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        try {
            // Check if it's a valid integer (possibly with sign)
            size_t pos;
            int val = std::stoi(line, &pos);
            // Ensure the whole line was consumed, or at least no trailing non-whitespace
            bool valid = true;
            for (size_t i = pos; i < line.length(); ++i) {
                if (!isspace(line[i])) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                state.setValue(var, val);
                break;
            }
        } catch (...) {
        }
        std::cout << "INVALID NUMBER" << std::endl;
    }
}

void EndStatement::execute(EvalState &state, Program &program) {
    throw 0;
}

void GotoStatement::execute(EvalState &state, Program &program) {
    if (program.getSourceLine(lineNumber) == "") {
        error("LINE NUMBER ERROR");
    }
    throw lineNumber;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int v1 = lhs->eval(state);
    int v2 = rhs->eval(state);
    bool condition = false;
    if (op == "=") condition = (v1 == v2);
    else if (op == "<") condition = (v1 < v2);
    else if (op == ">") condition = (v1 > v2);
    
    if (condition) {
        if (program.getSourceLine(lineNumber) == "") {
            error("LINE NUMBER ERROR");
        }
        throw lineNumber;
    }
}
