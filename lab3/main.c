#include <stdio.h>
#include <stdlib.h>

#include "def.h"
#include "syntaxtree.h"
#include "symboltable.h"
#include "semantic.h"
#include "ir.h"

extern int yydebug;

bool gError = false;

SymbolTable gSymbolTable = NULL;
SymbolTable gFunctionTable = NULL;
SyntaxTreeNode* gTree = NULL;
IRCode* gIRCode = NULL;

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
	gFunctionTable = initFunctionTable();
	yyrestart(f);
	yyparse();
	if (gError) {
		return 0;
	}
	semanticAnalysis(gTree, gSymbolTable, gFunctionTable);
	checkUndefinedFunc(gFunctionTable);
	assignId(gSymbolTable);
	assignId(gFunctionTable);
	//printSymbolTable(gSymbolTable);
	//printSymbolTable(gFunctionTable);
	if (gError) {
		return 0;
	}
	//gIRCode = generateSampleCode();
	gIRCode = translateProgram(gTree, gSymbolTable, gFunctionTable);
	printIRCode(gIRCode, stdout);
	freeIRCode(gIRCode);
	freeTree(gTree);
	freeSymbolTable(gSymbolTable);
	freeSymbolTable(gFunctionTable);
	return 0;
}
