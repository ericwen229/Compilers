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

void handleLocalDefinition(SyntaxTreeNode* syntaxTreeNode, IdTable* idTable, SymbolTable symbolTable) {
	printf("local definition!\n");
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, IdTable* idTable, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		// TODO: insert
	}
	else if (syntaxTreeNode->type == N_DEF) { // local definition
		handleLocalDefinition(syntaxTreeNode, idTable, symbolTable);
	}
	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, idTable, symbolTable);
		child = child->nextSibling;
	}
}

