#include <stdlib.h>
#include <stdio.h>

#include "semantic.h"

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

