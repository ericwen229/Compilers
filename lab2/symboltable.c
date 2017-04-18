#include "symboltable.h"
#include "trie.h"

SymbolTable* initSymbolTable() {
	return initTrieNode();
}

void insertSymbol(SymbolTable* table, char* symbol, SymbolTableItem* item) {
	insertTrie(table, symbol, item);
}

void printSymbolTable(SymbolTable* table) {
	printTrie(table);
}

