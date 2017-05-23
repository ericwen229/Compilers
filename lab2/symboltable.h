#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "def.h"
#include "trie.h"
#include "syntaxtree.h"

typedef TrieNode* SymbolTable;

typedef enum SymbolTableTypeType {
	S_DEFAULT, S_BASIC, S_ARRAY, S_STRUCTDEF, S_STRUCT, S_FUNCTION
} SymbolTableTypeType;

struct SymbolTableType;

typedef struct ArrayType {
	int len;
	struct SymbolTableType* elementType;
} ArrayType;

typedef struct StructField {
	char* fieldName;
	struct SymbolTableType* fieldType;
	struct StructField* nextField;
} StructField;

typedef struct StructType {
	StructField* firstField;
} StructType;

typedef struct FuncParam {
	struct SymbolTableType* paramType;
	struct FuncParam* nextParam;
} FuncParam;

typedef struct FuncType {
	bool isDefined;
	int lineno;
	struct SymbolTableType* returnType;
	FuncParam* firstParam;
} FuncType;

typedef union SymbolType {
	BasicType basicType;
	ArrayType arrayType;
	StructType structType;
	char* structName;
	FuncType funcType;
} SymbolType;

typedef struct SymbolTableType {
	SymbolTableTypeType typeType;
	SymbolType type;
	int id;
} SymbolTableType;

SymbolTable initSymbolTable();
SymbolTable initFunctionTable();
TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* type);
SymbolTableType* querySymbol(SymbolTable table, char* symbol);
SymbolTableType* queryField(StructField* field, char* fieldName);
SymbolTableType* initSymbolTableType();
SymbolTableType* copySymbolTableType(SymbolTableType* type);
bool compareSymbolTableType(SymbolTableType* typeA, SymbolTableType* typeB);
void freeSymbolTableType(SymbolTableType* type);
void printSymbolTable(SymbolTable table);
void assignId(SymbolTable table);
void checkUndefinedFunc(SymbolTable table);
void freeSymbolTable(SymbolTable table);

#endif

