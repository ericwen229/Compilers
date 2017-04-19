#include <stdlib.h>
#include <stdio.h>

#include "semantic.h"

SymbolTableType* retrieveType(SyntaxTreeNode* specifierNode) {
	specifierNode = specifierNode->firstChild;
	SymbolTableType* newItem = (SymbolTableType*)malloc(sizeof(SymbolTableType));
	if (specifierNode->type == N_TYPE) { // simple type
		newItem->typeType = S_BASIC;
		newItem->type.basicType = specifierNode->attr.typeType;
		return newItem;
	}
	else { // struct specifier
		// TODO: parse struct specifier
		// add struct type to symbol table
		// if no tag specified then specify one starting with 0
		// the variable holds the name of structure definition
		return NULL;
	}
}

void handleVarDec(SymbolTableType* type, SyntaxTreeNode* varDecNode) {
	SyntaxTreeNode* child = varDecNode->firstChild;
	if (child->type == N_ID) {
		child->attr.id->type = type;
	}
	else {
		SymbolTableType* array = (SymbolTableType*)malloc(sizeof(SymbolTableType));
		array->typeType = S_ARRAY;
		array->type.arrayType.len = child->nextSibling->nextSibling->attr.intValue;
		array->type.arrayType.elementType = type;
		handleVarDec(array, child);
	}
}

void handleDec(SymbolTableType* type, SyntaxTreeNode* decNode) {
	SyntaxTreeNode* varDecNode = decNode->firstChild;
	handleVarDec(type, varDecNode);
}

void handleDecList(SymbolTableType* type, SyntaxTreeNode* decListNode) {
	SyntaxTreeNode* decNode = decListNode->firstChild;
	handleDec(type, decListNode->firstChild);
	if (decListNode->firstChild->nextSibling != NULL) {
		handleDecList(type, decListNode->firstChild->nextSibling->nextSibling);
	}
}

void handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable) {
	SymbolTableType* type = retrieveType(defNode->firstChild);
	handleDecList(type, defNode->firstChild->nextSibling);
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		// TODO: external definition
	}
	else if (syntaxTreeNode->type == N_DEF) { // local definition
		handleDef(syntaxTreeNode, symbolTable);
	}
	// TODO: other node types

	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, symbolTable);
		child = child->nextSibling;
	}
}

