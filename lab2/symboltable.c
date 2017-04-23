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

SymbolTableType* querySymbol(SymbolTable table, char* symbol) {
	return (SymbolTableType*)queryTrie(table, symbol);
}

SymbolTableType* initSymbolTableType() {
	SymbolTableType* newType = (SymbolTableType*)malloc(sizeof(SymbolTableType));
	newType->typeType = S_DEFAULT;
	return newType;
}

SymbolTableType* copySymbolTableType(SymbolTableType* type) {
	if (type->typeType == S_BASIC || type->typeType == S_STRUCT) {
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

bool compareArrayType(ArrayType* typeA, ArrayType* typeB) {
	return false;
}

bool compareStructType(StructType* typeA, StructType* typeB) {
	return false;
}

bool compareFuncParam(FuncParam* paramA, FuncParam* paramB) {
	if (paramA == paramB) {
		return true;
	}
	if (paramA == NULL || paramB == NULL) {
		return false;
	}
	if (!compareSymbolTableType(paramA->paramType, paramB->paramType)) {
		return false;
	}
	return compareFuncParam(paramA->nextParam, paramB->nextParam);
}

bool compareFuncType(FuncType* typeA, FuncType* typeB) {
	if (typeA == typeB) {
		return true;
	}
	if (typeA == NULL || typeB == NULL) {
		return false;
	}
	if (!compareSymbolTableType(typeA->returnType, typeB->returnType)) {
		return false;
	}
	return compareFuncParam(typeA->firstParam, typeB->firstParam);
}

bool compareSymbolTableType(SymbolTableType* typeA, SymbolTableType* typeB) {
	if (typeA == typeB) {
		return true;
	}
	if (typeA == NULL || typeB == NULL) {
		return false;
	}
	if (typeA->typeType != typeB->typeType) {
		return false;
	}
	switch (typeA->typeType) {
	case S_BASIC:
		return typeA->type.basicType == typeB->type.basicType;
	case S_ARRAY:
		return compareArrayType(&(typeA->type.arrayType), &(typeB->type.arrayType));
	case S_STRUCTDEF:
		return compareStructType(&(typeA->type.structType), &(typeB->type.structType));
	case S_STRUCT:
		return (strcmp(typeA->type.structName, typeB->type.structName) == 0);
	case S_FUNCTION:
		return compareFuncType(&(typeA->type.funcType), &(typeB->type.funcType));
	default:
		printf("??? BEING COMPARED\n");
	}
	return false;
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

