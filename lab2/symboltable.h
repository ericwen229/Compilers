#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "def.h"
#include "trie.h"
#include "syntaxtree.h"
#include "idtable.h"

typedef TrieNode* SymbolTable;

typedef enum SymbolTableItemType {
	BASIC, ARRAY, STRUCTDEF, STRUCT, FUNCTION
} SymbolTableItemType;

typedef enum BasicType {
	INT, FLOAT
} BasicType;

typedef union SymbolType {
	BasicType basicType;
} SymbolType;

typedef struct SymbolTableItem {
	SymbolTableItemType itemType;
	SymbolType type;
} SymbolTableItem;

SymbolTable initSymbolTable();
void insertSymbol(SymbolTable table, char* symbol, SymbolTableItem* item);
void printSymbolTable(SymbolTable table);
void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, idTable* idTable, SymbolTable symbolTable);

#endif

