#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semantic.h"

SymbolTableType* handleStructSpecifier(SyntaxTreeNode* structSpecifierNode, SymbolTable symbolTable) {
	SyntaxTreeNode* tagNode = structSpecifierNode->firstChild->nextSibling;
	if (tagNode->type == N_TAG) {
		// current node is StructSpecifier
		// production is STRUCT Tag
		SymbolTableType* newType = (SymbolTableType*)malloc(sizeof(SymbolTableType));
		newType->typeType = S_STRUCT;
		newType->type.structName = retrieveStr(tagNode->firstChild->attr.id);
		return newType;
	}
	else {
		// current node is StructSpecifier
		// production is STRUCT OptTag LC DefList RC
		char* structName = NULL;
		if (tagNode->firstChild == NULL) {
			// anonymous struct
			static int anonymousCount = 0;
			char nameBuf[9];
			sprintf(nameBuf, "%x", anonymousCount);
			++ anonymousCount;

			structName = (char*)malloc((2 + strlen(nameBuf)) * sizeof(char));
			structName[0] = '0';
			strcpy(structName + 1, nameBuf);
		}
		else {
			structName = retrieveStr(tagNode->firstChild->attr.id);
		}

		// TODO: insert into symbol table

		SymbolTableType* newType = (SymbolTableType*)malloc(sizeof(SymbolTableType));
		newType->typeType = S_STRUCT;
		newType->type.structName = structName;
	}
}

SymbolTableType* handleSpecifier(SyntaxTreeNode* specifierNode, SymbolTable symbolTable) {
	specifierNode = specifierNode->firstChild;
	if (specifierNode->type == N_TYPE) { // simple type
		SymbolTableType* newType = (SymbolTableType*)malloc(sizeof(SymbolTableType));
		newType->typeType = S_BASIC;
		newType->type.basicType = specifierNode->attr.typeType;
		return newType;
	}
	else { // struct specifier
		return handleStructSpecifier(specifierNode, symbolTable);
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
	SymbolTableType* type = handleSpecifier(defNode->firstChild, symbolTable);
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

