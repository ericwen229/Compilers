#ifndef __SYNTAXTREE_H__
#define __SYNTAXTREE_H__

#include "trie.h"

typedef enum NodeType {
	N_SEMI, N_COMMA, N_ASSIGNOP, N_RELOP,
	N_PLUS, N_MINUS, N_STAR, N_DIV,
	N_AND, N_OR, N_NOT, N_DOT,
	N_TYPE,
	N_LP, N_RP, N_LB, N_RB, N_LC, N_RC,
	N_STRUCT, N_RETURN, N_IF, N_ELSE, N_WHILE,
	N_INT, N_FLOAT,
	N_ID,

	N_PROGRAM, N_EXTDEFLIST, N_EXTDEF, N_EXTDECLIST,
	N_SPECIFIER, N_STRUCTSPECIFIER, N_OPTTAG, N_TAG,
	N_VARDEC, N_FUNDEC, N_VARLIST, N_PARAMDEC,
	N_COMPST, N_STMTLIST, N_STMT,
	N_DEFLIST, N_DEF, N_DECLIST, N_DEC,
	N_EXP, N_ARGS
} NodeType;

typedef enum RelopType {
	LT, GT, LE, GE, EQ, NE
} RelopType;

typedef enum BasicType {
	T_INT, T_FLOAT
} BasicType;

typedef union Attribute {
	TrieNode* id;
	int intValue;
	float floatValue;
	BasicType typeType;
	RelopType relopType;
} Attribute;

typedef struct SyntaxTreeNode {
	NodeType type;
	Attribute attr;
	struct SyntaxTreeNode* firstChild;
	struct SyntaxTreeNode* nextSibling;
	int lineno;
} SyntaxTreeNode;

SyntaxTreeNode* createTerminalNode(NodeType type, Attribute attr, int lineno);
SyntaxTreeNode* createNode(NodeType type);
void freeTree(SyntaxTreeNode* tree);
void appendChild(SyntaxTreeNode* parent, SyntaxTreeNode* child);
int numOfChild(SyntaxTreeNode *node);
// void printTree(SyntaxTreeNode* tree, int level);

#endif

