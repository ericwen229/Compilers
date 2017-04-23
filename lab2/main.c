#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "syntaxtree.h"
#include "symboltable.h"
#include "semantic.h"

extern int yydebug;

bool gError = false;

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
	gSymbolTable = initSymbolTable();
	yyrestart(f);
	yyparse();
	if (!gError) {
		semanticAnalysis(gTree, gSymbolTable);
		checkUndefinedFunc(gSymbolTable);
		printSymbolTable(gSymbolTable);
	}
	freeTree(gTree);
	freeSymbolTable(gSymbolTable);
	return 0;
}
