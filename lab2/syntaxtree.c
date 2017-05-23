#include <stdlib.h>
#include <stdio.h>

#include "syntaxtree.h"

int numOfChild(SyntaxTreeNode *node) {
	SyntaxTreeNode* child = node->firstChild;
	int count = 0;
	while (child != NULL) {
		++count;
		child = child->nextSibling;
	}
	return count;
}

SyntaxTreeNode* createTerminalNode(NodeType type, Attribute attr, int lineno) {
	SyntaxTreeNode *newNode = (SyntaxTreeNode*)malloc(sizeof(SyntaxTreeNode));
	newNode->type = type;
	newNode->attr = attr;
	newNode->firstChild = NULL;
	newNode->nextSibling = NULL;
	newNode->lineno = lineno;
	return newNode;
}

SyntaxTreeNode* createNode(NodeType type) {
	SyntaxTreeNode *newNode = (SyntaxTreeNode*)malloc(sizeof(SyntaxTreeNode));
	newNode->type = type;
	newNode->firstChild = NULL;
	newNode->nextSibling = NULL;
	newNode->lineno = -1;
	return newNode;
}

void freeTree(SyntaxTreeNode* tree) {
	if (tree == NULL) {
		return;
	}

	SyntaxTreeNode* currChild = tree->firstChild;
	while (currChild != NULL) {
		SyntaxTreeNode* next = currChild->nextSibling;
		freeTree(currChild);
		currChild = next;
	}

	free(tree);
}

void appendChild(SyntaxTreeNode* parent, SyntaxTreeNode* child) {
	if (parent->firstChild == NULL) {
		parent->firstChild = child;
		parent->lineno = child->lineno;
		return;
	}

	SyntaxTreeNode* currChild = parent->firstChild;
	while (currChild->nextSibling != NULL) {
		currChild = currChild->nextSibling;	
	}
	currChild->nextSibling = child;
}

/* void printTree(SyntaxTreeNode* tree, int level) {

	static char* nodeName[] = {
		"SEMI", "COMMA", "ASSIGNOP", "RELOP",
		"PLUS", "MINUS", "STAR", "DIV",
		"AND", "OR", "NOT", "DOT",
		"TYPE",
		"LP", "RP", "LB", "RB", "LC", "RC",
		"STRUCT", "RETURN", "IF", "ELSE", "WHILE",
		"INT", "FLOAT",
		"ID",
		"Program", "ExtDefList", "ExtDef", "ExtDecList",
		"Specifier", "StructSpecifier", "OptTag", "Tag",
		"VarDec", "FunDec", "VarList", "ParamDec",
		"CompSt", "StmtList", "Stmt",
		"DefList", "Def", "DecList", "Dec",
		"Exp", "Args",
	};

	if (tree == NULL) {
		return;
	}

	int i = 0;

	if (tree->firstChild != NULL) {
		for (i = 0; i < 2 * level; ++ i) {
			putchar(' ');
		}
		printf("%s (%d)\n", nodeName[(int)tree->type], tree->lineno);
		SyntaxTreeNode* child = tree->firstChild;
		while (child != NULL) {
			printTree(child, level + 1);
			child = child->nextSibling;
		}
	}
	else if ((int)tree->type < (int)N_PROGRAM) {
		for (i = 0; i < 2 * level; ++ i) {
			putchar(' ');
		}
		printf("%s", nodeName[(int)tree->type]);
		IdTableCell* cell = NULL;
		switch (tree->type) {
			case N_ID:
				cell = getCell(gIdTable, tree->attr.id);
				printf(": %s", cell->name);
				break;
			case N_TYPE:
				switch (tree->attr.typeType) {
					case TYPEINT: printf(": int"); break;
					case TYPEFLOAT: printf(": float"); break;
					default: break;
				}
				break;
			case N_INT: printf(": %d", tree->attr.intValue); break;
			case N_FLOAT: printf(": %f", tree->attr.floatValue); break;
			case N_RELOP:
				switch (tree->attr.relopType) {
					case LT: printf(": <"); break;
					case GT: printf(": >"); break;
					case LE: printf(": <="); break;
					case GE: printf(": >="); break;
					case EQ: printf(": =="); break;
					case NE: printf(": !="); break;
					default: break;
				}
				break;
			default: break;
		}
		putchar('\n');
	}
} */

