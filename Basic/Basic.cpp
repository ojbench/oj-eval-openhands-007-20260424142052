/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);
Statement* parseStatement(TokenScanner &scanner);
void executeProgram(Program &program, EvalState &state);
void listProgram(Program &program);

/* Main program */

int main() {
    EvalState state;
    Program program;
    while (true) {
        try {
            std::string input;
            if (!getline(std::cin, input)) break;
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        } catch (int jumpLine) {
            // Should not happen in main loop unless RUN is called
        }
    }
    return 0;
}

Statement* parseStatement(TokenScanner &scanner) {
    std::string token = scanner.nextToken();
    if (token == "REM") {
        return new RemStatement();
    } else if (token == "LET") {
        std::string var = scanner.nextToken();
        if (var == "") error("SYNTAX ERROR");
        std::string keywords[] = {"REM", "LET", "PRINT", "INPUT", "END", "GOTO", "IF", "THEN", "RUN", "LIST", "CLEAR", "QUIT", "HELP"};
        for (const std::string &kw : keywords) {
            if (var == kw) error("SYNTAX ERROR");
        }
        if (scanner.nextToken() != "=") error("SYNTAX ERROR");
        Expression *exp = readE(scanner, 0);
        if (scanner.hasMoreTokens()) {
            delete exp;
            error("SYNTAX ERROR");
        }
        return new LetStatement(var, exp);
    } else if (token == "PRINT") {
        Expression *exp = readE(scanner, 0);
        if (scanner.hasMoreTokens()) {
            delete exp;
            error("SYNTAX ERROR");
        }
        return new PrintStatement(exp);
    } else if (token == "INPUT") {
        std::string var = scanner.nextToken();
        if (var == "") error("SYNTAX ERROR");
        std::string keywords[] = {"REM", "LET", "PRINT", "INPUT", "END", "GOTO", "IF", "THEN", "RUN", "LIST", "CLEAR", "QUIT", "HELP"};
        for (const std::string &kw : keywords) {
            if (var == kw) error("SYNTAX ERROR");
        }
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        return new InputStatement(var);
    } else if (token == "END") {
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        return new EndStatement();
    } else if (token == "GOTO") {
        std::string next = scanner.nextToken();
        if (next == "" || !isdigit(next[0])) error("SYNTAX ERROR");
        int line = std::stoi(next);
        if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
        return new GotoStatement(line);
    } else if (token == "IF") {
        Expression *lhs = readE(scanner, 1); // Use precedence 1 to stop at =, <, >
        std::string op = scanner.nextToken();
        if (op != "=" && op != "<" && op != ">") {
            delete lhs;
            error("SYNTAX ERROR");
        }
        Expression *rhs = readE(scanner, 1);
        if (scanner.nextToken() != "THEN") {
            delete lhs; delete rhs;
            error("SYNTAX ERROR");
        }
        std::string next = scanner.nextToken();
        if (next == "" || !isdigit(next[0])) {
            delete lhs; delete rhs;
            error("SYNTAX ERROR");
        }
        int line = std::stoi(next);
        if (scanner.hasMoreTokens()) {
            delete lhs; delete rhs;
            error("SYNTAX ERROR");
        }
        return new IfStatement(lhs, op, rhs, line);
    } else {
        error("SYNTAX ERROR");
    }
    return nullptr;
}

void executeProgram(Program &program, EvalState &state) {
    int lineNumber = program.getFirstLineNumber();
    while (lineNumber != -1) {
        Statement *stmt = program.getParsedStatement(lineNumber);
        if (stmt == nullptr) {
            TokenScanner scanner;
            scanner.ignoreWhitespace();
            scanner.scanNumbers();
            std::string line = program.getSourceLine(lineNumber);
            scanner.setInput(line);
            scanner.nextToken(); // skip line number
            stmt = parseStatement(scanner);
            program.setParsedStatement(lineNumber, stmt);
        }
        try {
            stmt->execute(state, program);
            lineNumber = program.getNextLineNumber(lineNumber);
        } catch (int jump) {
            if (jump == 0) break; // END
            lineNumber = jump;
        }
    }
}

void listProgram(Program &program) {
    int lineNumber = program.getFirstLineNumber();
    while (lineNumber != -1) {
        std::cout << program.getSourceLine(lineNumber) << std::endl;
        lineNumber = program.getNextLineNumber(lineNumber);
    }
}

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (scanner.hasMoreTokens()) {
        std::string token = scanner.nextToken();
        if (isdigit(token[0])) {
            int lineNumber = std::stoi(token);
            if (scanner.hasMoreTokens()) {
                program.addSourceLine(lineNumber, line);
            } else {
                program.removeSourceLine(lineNumber);
            }
        } else {
            // Direct command
            if (token == "RUN") {
                executeProgram(program, state);
            } else if (token == "LIST") {
                listProgram(program);
            } else if (token == "CLEAR") {
                program.clear();
                state.Clear();
            } else if (token == "QUIT") {
                exit(0);
            } else if (token == "HELP") {
                // Optional
            } else {
                // Immediate execution of statements
                scanner.setInput(line); // Reset scanner to include the first token
                Statement *stmt = parseStatement(scanner);
                if (stmt) {
                    try {
                        stmt->execute(state, program);
                    } catch (int jump) {
                        if (jump != 0) {
                            // For GOTO/IF in immediate mode, we start execution from that line
                            executeProgram(program, state);
                        }
                    }
                    delete stmt;
                }
            }
        }
    }
}

