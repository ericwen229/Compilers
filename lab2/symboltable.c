#include <stdlib.h>
#include <stdio.h>

#include "symboltable.h"

SymbolTable initSymbolTable() {
	return initTrieNode();
}

void insertSymbol(SymbolTable table, char* symbol, SymbolTableItem* item) {
	insertTrie(table, symbol, item);
}

void printSymbolTable(SymbolTable table) {
	printTrie(table);
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, idTable* idTable, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF || syntaxTreeNode->type == N_DEF) {
		printf("definition!\n");
	}
	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, idTable, symbolTable);
		child = child->nextSibling;
	}
}

