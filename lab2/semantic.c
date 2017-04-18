#include <stdlib.h>
#include <stdio.h>

#include "semantic.h"

SymbolTableItem* retrieveType(SyntaxTreeNode* specifierNode) {
	specifierNode = specifierNode->firstChild;
	SymbolTableItem* newItem = (SymbolTableItem*)malloc(sizeof(SymbolTableItem));
	if (specifierNode->type == N_TYPE) { // simple type
		newItem->itemType = S_BASIC;
		newItem->type.basicType = specifierNode->attr.typeType;
		return newItem;
	}
	else { // struct specifier
		/* TODO */
		return NULL;
	}
}

void handleLocalDefinition(SyntaxTreeNode* localDefNode, SymbolTable symbolTable) {
	SymbolTableItem* item = retrieveType(localDefNode->firstChild);
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
	}
	else if (syntaxTreeNode->type == N_DEF) { // local definition
		handleLocalDefinition(syntaxTreeNode, symbolTable);
	}
	// TODO: other node types

	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, symbolTable);
		child = child->nextSibling;
	}
}

