#include <stdlib.h>
#include <stdio.h>

#include "semantic.h"

void handleLocalDefinition(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		// TODO: insert
	}
	else if (syntaxTreeNode->type == N_DEF) { // local definition
		handleLocalDefinition(syntaxTreeNode, symbolTable);
	}
	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, symbolTable);
		child = child->nextSibling;
	}
}

