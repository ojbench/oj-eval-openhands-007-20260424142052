/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() {
    clear();
}

void Program::clear() {
    sourceLines.clear();
    for (auto const& [lineNumber, stmt] : parsedStatements) {
        delete stmt;
    }
    parsedStatements.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    sourceLines[lineNumber] = line;
    if (parsedStatements.count(lineNumber)) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
}

void Program::removeSourceLine(int lineNumber) {
    sourceLines.erase(lineNumber);
    if (parsedStatements.count(lineNumber)) {
        delete parsedStatements[lineNumber];
        parsedStatements.erase(lineNumber);
    }
}

std::string Program::getSourceLine(int lineNumber) {
    if (sourceLines.count(lineNumber)) {
        return sourceLines[lineNumber];
    }
    return "";
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if (sourceLines.count(lineNumber) == 0) {
        error("Line number does not exist");
    }
    if (parsedStatements.count(lineNumber)) {
        delete parsedStatements[lineNumber];
    }
    parsedStatements[lineNumber] = stmt;
}

Statement *Program::getParsedStatement(int lineNumber) {
    if (parsedStatements.count(lineNumber)) {
        return parsedStatements[lineNumber];
    }
    return nullptr;
}

int Program::getFirstLineNumber() {
    if (sourceLines.empty()) {
        return -1;
    }
    return sourceLines.begin()->first;
}

int Program::getNextLineNumber(int lineNumber) {
    auto it = sourceLines.upper_bound(lineNumber);
    if (it == sourceLines.end()) {
        return -1;
    }
    return it->first;
}


