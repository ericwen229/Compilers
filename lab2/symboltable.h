#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "def.h"
#include "trie.h"
#include "syntaxtree.h"

typedef TrieNode* SymbolTable;

typedef enum SymbolTableItemType {
	S_BASIC, S_ARRAY, S_STRUCTDEF, S_STRUCT, S_FUNCTION
} SymbolTableItemType;

typedef union SymbolType {
	BasicType basicType;
} SymbolType;

typedef struct SymbolTableItem {
	SymbolTableItemType itemType;
	SymbolType type;
} SymbolTableItem;

SymbolTable initSymbolTable();
TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableItem* item);
void printSymbolTable(SymbolTable table);

#endif

