#include "symboltable.h"

SymbolTable initSymbolTable() {
	return initTrieNode();
}

TrieNode* insertSymbol(SymbolTable table, char* symbol, SymbolTableType* item) {
	return insertTrie(table, symbol, item);
}

void printSymbolTable(SymbolTable table) {
	printTrie(table);
}

