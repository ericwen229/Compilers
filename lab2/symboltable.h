#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "def.h"
#include "trie.h"
#include "syntaxtree.h"

typedef TrieNode* SymbolTable;

typedef enum SymbolTableTypeType {
	S_BASIC, S_ARRAY, S_STRUCTDEF, S_STRUCT, S_FUNCTION
} SymbolTableTypeType;

struct SymbolTableType;

typedef struct ArrayType {
	int len;
	struct SymbolTableType* elementType;
} ArrayType;

typedef struct StructType {
	char* fieldName;
	struct SymbolTableType* fieldType;
	struct StructType* nextField;
} StructType;

typedef union SymbolType {
	BasicType basicType;
	ArrayType arrayType;
	StructType structType;
	char* structName;
} SymbolType;

typedef struct SymbolTableType {
	SymbolTableTypeType typeType;
	SymbolType type;
} SymbolTableType;

SymbolTable initSymbolTable();
TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* item);
void printSymbolTable(SymbolTable table);

#endif

