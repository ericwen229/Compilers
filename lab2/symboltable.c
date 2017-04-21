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

void printSymbolTable(SymbolTable table) {
	printTrie(table);
}

