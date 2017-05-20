#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include "syntaxtree.h"
#include "symboltable.h"

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable, SymbolTable functionTable);

#endif

