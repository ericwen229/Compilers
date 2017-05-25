#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symboltable.h"

SymbolTable initSymbolTable() {
	return initTrieNode(NULL, -1);
}

SymbolTableType* initIntType() {
	SymbolTableType* intType = initSymbolTableType();
	intType->typeType = S_BASIC;
	intType->type.basicType = T_INT;
	return intType;
}

SymbolTableType* initReadType() {
	SymbolTableType* readType = initSymbolTableType();
	readType->typeType = S_FUNCTION;
	readType->type.funcType.isDefined = true;
	readType->type.funcType.lineno = -1;
	readType->type.funcType.returnType = initIntType();
	readType->type.funcType.firstParam = NULL;
	return readType;
}

FuncParam* initParam(SymbolTableType* type, FuncParam* next) {
	FuncParam* param = (FuncParam*)malloc(sizeof(FuncParam));
	param->paramType = type;
	param->nextParam = next;
}

SymbolTableType* initWriteType() {
	SymbolTableType* writeType = initSymbolTableType();
	writeType->typeType = S_FUNCTION;
	writeType->type.funcType.isDefined = true;
	writeType->type.funcType.lineno = -1;
	writeType->type.funcType.returnType = initIntType();
	writeType->type.funcType.firstParam = initParam(initIntType(), NULL);
	return writeType;
}

SymbolTable initFunctionTable() {
	SymbolTable functionTable = initSymbolTable();
	insertSymbol(functionTable, "read", initReadType());
	insertSymbol(functionTable, "write", initWriteType());
	return functionTable;
}

TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* type) {
	return insertTrie(table, symbol, type);
}

SymbolTableType* querySymbol(SymbolTable table, char* symbol) {
	return (SymbolTableType*)queryTrie(table, symbol);
}

SymbolTableType* queryField(StructField* field, char* fieldName) {
	if (field == NULL) return NULL;
	if (strcmp(field->fieldName, fieldName) == 0) {
		return copySymbolTableType(field->fieldType);
	}
	else {
		return queryField(field->nextField, fieldName);
	}
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
	else if (type->typeType == S_ARRAY) {
		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = type->typeType;
		newType->type.arrayType.len = type->type.arrayType.len;
		newType->type.arrayType.elementType = copySymbolTableType(type->type.arrayType.elementType);
		return newType;
	}
	else {
		// TODO
		printf("[[WHY IS STRUCTDEF/FUNCTION BEING COPIED!]]\n");
		return NULL;
	}
}

bool compareSymbolTableType(SymbolTableType* typeA, SymbolTableType* typeB);

bool compareArrayType(ArrayType* typeA, ArrayType* typeB) {
	if (typeA == typeB) {
		return true;
	}
	return compareSymbolTableType(typeA->elementType, typeB->elementType);
}

bool compareStructField(StructField* fieldA, StructField* fieldB) {
	if (fieldA == fieldB) {
		return true;
	}
	if (fieldA == NULL || fieldB == NULL) {
		return false;
	}
	if (!compareSymbolTableType(fieldA->fieldType, fieldB->fieldType)) {
		return false;
	}
	return compareStructField(fieldA->nextField, fieldB->nextField);
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
		return !(typeA == NULL);
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
		return compareStructField(typeA->type.structType.firstField, typeB->type.structType.firstField);
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

void printType(void* _type) {
	if (_type == NULL) {
		printf("<<NOTYPE>>");
		return;
	}
	SymbolTableType* type = (SymbolTableType*)_type;
	if (type->typeType == S_BASIC) {
		if (type->type.basicType == T_INT) {
			printf("<<INT>>");
		}
		else {
			printf("<<FLOAT>>");
		}
	}
	else if (type->typeType == S_ARRAY) {
		printf("[%d]", type->type.arrayType.len);
		printType((void*)type->type.arrayType.elementType);
	}
	else if (type->typeType == S_STRUCT) {
		printf("<<STRUCT %s>>", type->type.structName);
	}
	else if (type->typeType == S_STRUCTDEF) {
		printf("<<STRUCTDEF>>");
		StructField* structField = type->type.structType.firstField;
		while (structField != NULL) {
			printf("[%s|", structField->fieldName);
			printType((void*)structField->fieldType);
			putchar(']');
			structField = structField->nextField;
		}
	}
	else if (type->typeType == S_FUNCTION) {
		printf("<<FUNCTION>>[[");
		printType((void*)type->type.funcType.returnType);
		printf("]]");
		FuncParam* funcParam = type->type.funcType.firstParam;
		while (funcParam != NULL) {
			putchar('[');
			printType(funcParam->paramType);
			putchar(']');
			funcParam = funcParam->nextParam;
		}
	}
	else {
		printf("<<UNKNOWN>>");
	}
	printf("[%d]", type->id);
}

void printTrieNode(TrieNode* node) {
	if (node->isEnd) {
		putchar(' ');
		char* str = retrieveStr(node);
		printf("%s", str);
		free(str);
		putchar(' ');
		printType(node->type);
		putchar('\n');
	}
}

void assignNodeId(TrieNode* node) {
	static int id = 0;
	if (node->isEnd && node->type != NULL) {
		((SymbolTableType*)node->type)->id = id;
		++ id;
	}
}

void freeTrieNodeType(TrieNode* node) {
	if (node->type != NULL) {
		freeSymbolTableType(node->type);
		node->type = NULL;
	}
}

void freeTrieNode(TrieNode* node) {
	free(node);
}

void printSymbolTable(SymbolTable table) {
	traverseTrie(table, printTrieNode, NULL);
}

void assignId(SymbolTable table) {
	traverseTrie(table, assignNodeId, NULL);
}

void checkUndefinedFuncNode(TrieNode* node) {
	SymbolTableType* type = (SymbolTableType*)node->type;
	if (type == NULL) return;
	if (type->typeType == S_FUNCTION) {
		if (!type->type.funcType.isDefined) {
			char* funcName = retrieveStr(node);
			printf("Error type 18 at Line %d: Function \"%s\" declared but not defined.\n", type->type.funcType.lineno, funcName);
			free(funcName);
		}
	}
}

void checkUndefinedFunc(SymbolTable table) {
	traverseTrie(table, checkUndefinedFuncNode, NULL);
}

void freeSymbolTable(SymbolTable table) {
	traverseTrie(table, freeTrieNodeType, freeTrieNode);
}

