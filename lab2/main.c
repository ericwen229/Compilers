#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "syntaxtree.h"
#include "idtable.h"
#include "symboltable.h"
#include "semantic.h"

extern int yydebug;

bool gError = false;

// IdTable* gIdTable = NULL;
SymbolTable gSymbolTable = NULL;
SyntaxTreeNode* gTree = NULL;

void yyrestart(FILE*);
int yyparse(void);

int main(int argc, char* argv[]) {
	if (argc <= 1) return 1;
	FILE *f = fopen(argv[1], "r");
	if (!f) {
		perror(argv[1]);
		return 1;	
	}
	// gIdTable = createIdTable(128);
	gSymbolTable = initSymbolTable();
	yyrestart(f);
	yyparse();
	if (!gError) {
		printSymbolTable(gSymbolTable);
		semanticAnalysis(gTree, gSymbolTable);
	}
	freeTree(gTree);
	// freeTable(gIdTable);
	return 0;
}
