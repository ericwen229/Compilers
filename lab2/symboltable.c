#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symboltable.h"

SymbolTable initSymbolTable() {
	return initTrieNode(NULL, -1);
}

TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* type) {
	return insertTrie(table, symbol, type);
}

SymbolTableType* initSymbolTableType() {
	SymbolTableType* newType = (SymbolTableType*)malloc(sizeof(SymbolTableType));
	newType->typeType = S_DEFAULT;
	return newType;
}

SymbolTableType* copySymbolTableType(SymbolTableType* type) {
	if (type->typeType == S_DEFAULT) {
		printf("[[WHY IS DEFAULT BEING COPIED!]]\n");
		return NULL;
	}
	else if (type->typeType == S_BASIC || type->typeType == S_STRUCT) {
		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = type->typeType;
		if (newType->typeType == S_BASIC) {
			newType->type = type->type;
		}
		else {
			newType->type.structName = (char*)malloc(sizeof(char) * (strlen(type->type.structName) + 1));
			strcpy(newType->type.structName, type->type.structName);
		}
		return newType;
	}
	else {
		printf("[[WHY IS ARRAY/STRUCTDEF/FUNCTION BEING COPIED!]]\n");
		return NULL;
	}
}

void freeStructField(StructField* field) {
	if (field == NULL) return;
	free(field->fieldName);
	freeStructField(field->nextField);
	free(field);
}

void freeFuncParam(FuncParam* param) {
	if (param == NULL) return;
	freeFuncParam(param->nextParam);
	free(param);
}

void freeSymbolTableType(SymbolTableType* type) {
	if (type == NULL) {
		printf("NULL TYPE BEING FREED\n");
		return;
	}
	switch (type->typeType) {
	case S_BASIC: free(type); break;
	case S_ARRAY: freeSymbolTableType(type->type.arrayType.elementType); free(type); break;
	case S_STRUCT: free(type->type.structName); free(type); break;
	case S_STRUCTDEF: freeStructField(type->type.structType.firstField); free(type); break;
	case S_FUNCTION: freeSymbolTableType(type->type.funcType.returnType); freeFuncParam(type->type.funcType.firstParam); free(type); break;
	default: printf("??? BEING FREED\n"); break;
	}
}

void printSymbolTable(SymbolTable table) {
	printTrie(table);
}

void freeSymbolTable(SymbolTable table) {
	freeTrie(table);
}

