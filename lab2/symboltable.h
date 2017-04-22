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
} SymbolTableType;

SymbolTable initSymbolTable();
TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* type);
SymbolTableType* initSymbolTableType();
SymbolTableType* copySymbolTableType(SymbolTableType* type);
void freeSymbolTableType(SymbolTableType* type);
void printSymbolTable(SymbolTable table);
void freeSymbolTable(SymbolTable table);

#endif

