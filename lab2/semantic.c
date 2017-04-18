#include <stdlib.h>
#include <stdio.h>

#include "semantic.h"

SymbolTableType* retrieveType(SyntaxTreeNode* specifierNode) {
	specifierNode = specifierNode->firstChild;
	SymbolTableType* newItem = (SymbolTableType*)malloc(sizeof(SymbolTableType));
	if (specifierNode->type == N_TYPE) { // simple type
		newItem->itemType = S_BASIC;
		newItem->type.basicType = specifierNode->attr.typeType;
		return newItem;
	}
	else { // struct specifier
		// TODO: parse struct specifier
		return NULL;
	}
}

void addDecListType(SymbolTableType* type, SyntaxTreeNode* decListNode) {
}

void handleLocalDefinition(SyntaxTreeNode* localDefNode, SymbolTable symbolTable) {
	SymbolTableType* type = retrieveType(localDefNode->firstChild);
	addDecListType(type, localDefNode->firstChild->nextSibling);
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		// TODO: external definition
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

