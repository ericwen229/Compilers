#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semantic.h"

StructField* handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable, bool isStruct, StructField* currField);

StructField* handleStructDefList(SyntaxTreeNode* defListNode, SymbolTable symbolTable) {
	if (defListNode->firstChild == NULL) {
		return NULL;
	}
	StructField* currField = (StructField*)malloc(sizeof(StructField));
	StructField* tailField = handleDef(defListNode->firstChild, symbolTable, true, currField);
	tailField->nextField = handleStructDefList(defListNode->firstChild->nextSibling, symbolTable);
	return currField;
}

SymbolTableType* handleStructSpecifier(SyntaxTreeNode* structSpecifierNode, SymbolTable symbolTable) {
	SyntaxTreeNode* tagNode = structSpecifierNode->firstChild->nextSibling;
	if (tagNode->type == N_TAG) {
		// current node is StructSpecifier
		// production is STRUCT Tag
		SymbolTableType* newType = initSymbolTableType();
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

		SymbolTableType* insertType = initSymbolTableType();
		insertType->typeType = S_STRUCTDEF;
		insertType->type.structType.firstField = handleStructDefList(tagNode->nextSibling->nextSibling, symbolTable);
		TrieNode* structDefNode = insertSymbol(symbolTable, structName, insertType);

		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = S_STRUCT;
		newType->type.structName = structName;
		return newType;
	}
}

SymbolTableType* handleSpecifier(SyntaxTreeNode* specifierNode, SymbolTable symbolTable) {
	specifierNode = specifierNode->firstChild;
	if (specifierNode->type == N_TYPE) { // simple type
		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = S_BASIC;
		newType->type.basicType = specifierNode->attr.typeType;
		return newType;
	}
	else { // struct specifier
		return handleStructSpecifier(specifierNode, symbolTable);
	}
}

void handleVarDec(SymbolTableType* type, SyntaxTreeNode* varDecNode, bool isStruct, StructField* currField) {
	SyntaxTreeNode* child = varDecNode->firstChild;
	if (child->type == N_ID) {
		child->attr.id->type = type;
		if (isStruct) {
			currField->fieldName = retrieveStr(child->attr.id);
			currField->fieldType = type;
		}
	}
	else {
		SymbolTableType* array = initSymbolTableType();
		array->typeType = S_ARRAY;
		array->type.arrayType.len = child->nextSibling->nextSibling->attr.intValue;
		array->type.arrayType.elementType = type;
		handleVarDec(array, child, isStruct, currField);
	}
}

void handleDec(SymbolTableType* type, SyntaxTreeNode* decNode, bool isStruct, StructField* currField) {
	SyntaxTreeNode* varDecNode = decNode->firstChild;
	handleVarDec(type, varDecNode, isStruct, currField);
}

StructField* handleDecList(SymbolTableType* type, SyntaxTreeNode* decListNode, bool isStruct, StructField* currField) {
	SyntaxTreeNode* decNode = decListNode->firstChild;
	handleDec(copySymbolTableType(type), decListNode->firstChild, isStruct, currField);
	if (decNode->nextSibling != NULL) {
		StructField* nextField = NULL;
		if (isStruct) {
			nextField = (StructField*)malloc(sizeof(StructField));
			currField->nextField = nextField;
			nextField->nextField = NULL;
		}
		return handleDecList(type, decNode->nextSibling->nextSibling, isStruct, nextField);
	}
	else {
		return currField;
	}
}

StructField* handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable, bool isStruct, StructField* currField) {
	SymbolTableType* type = handleSpecifier(defNode->firstChild, symbolTable);
	StructField* field = handleDecList(type, defNode->firstChild->nextSibling, isStruct, currField);
	freeSymbolTableType(type);
	return field;
}

void handleExtDecList(SymbolTableType* type, SyntaxTreeNode* extDecListNode) {
	SyntaxTreeNode* varDecNode = extDecListNode->firstChild;
	handleVarDec(type, varDecNode, false, NULL);
	if (varDecNode->nextSibling != NULL) {
		handleExtDecList(type, varDecNode->nextSibling->nextSibling);
	}
}

void handleExtDef(SyntaxTreeNode* extDefNode, SymbolTable symbolTable) {
	SymbolTableType* type = handleSpecifier(extDefNode->firstChild, symbolTable);
	SyntaxTreeNode* secondChild = extDefNode->firstChild->nextSibling;
	if (secondChild->type == N_SEMI) {
		free(type);
	}
	else if (secondChild->type == N_EXTDECLIST) {
		handleExtDecList(type, secondChild);
	}
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		handleExtDef(syntaxTreeNode, symbolTable);
		return;
	}
	//else if (syntaxTreeNode->type == N_DEF) { // local definition
	//	handleDef(syntaxTreeNode, symbolTable, false, NULL);
	//	return;
	//}
	// TODO: other node types

	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, symbolTable);
		child = child->nextSibling;
	}
}

