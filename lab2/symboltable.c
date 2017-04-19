#include <stdlib.h>

#include "symboltable.h"

SymbolTable initSymbolTable() {
	return initTrieNode(NULL, -1);
}

TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* type) {
	return insertTrie(table, symbol, type);
}

void printSymbolTable(SymbolTable table) {
	printTrie(table);
}

