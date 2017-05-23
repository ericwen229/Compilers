#ifndef __IR_H__
#define __IR_H__

#include <stdio.h>

#include "def.h"
#include "trie.h"
#include "syntaxtree.h"
#include "symboltable.h"

typedef struct IROperand {
	enum {
		IRO_CONST, IRO_VAR, IRO_TEMP,
		IRO_GETADDR, IRO_SETADDR,
		IRO_FUNC, IRO_LABEL
	} type;
	bool isTemp;
	union {
		int constValue;
		struct {
			int varId;
			TrieNode* varNode;
		};
		int tempId;
		char* funcName;
		int labelId;
	};
} IROperand;

typedef enum RelOp {
	IR_LT, IR_LE, IR_EQ, IR_GE, IR_GT
} RelOp;

typedef struct IRCode {
	enum {
		IR_LABEL, IR_FUNC,
		IR_ASSIGN, IR_ADD, IR_MINUS, IR_STAR, IR_DIV,
		IR_GOTO, IR_COND, IR_RETURN, IR_DEC, IR_ARG, IR_CALL, IR_PARAM,
		IR_READ, IR_WRITE
	} type;
	union {
		struct {
			IROperand* op;
			IROperand* dest;
		} singleOp;
		struct {
			IROperand* left;
			IROperand* right;
			IROperand* dest;
		} binOp;
		struct {
			IROperand* left;
			IROperand* right;
			RelOp relOp;
			IROperand* dest;
		} condOp;
		struct {
			IROperand* op;
			int size;
		} decOp;
	};
	struct IRCode* prev;
	struct IRCode* next;
} IRCode;

IROperand* createConstOperand(int constValue);
IROperand* createVarOperand(int varId, TrieNode* varNode);
IROperand* createTempOperand(int tempId);
IROperand* createGetAddrOperand(bool isTemp, int varId, int tempId);
IROperand* createSetAddrOperand(bool isTemp, int varId, int tempId);
IROperand* createFuncOperand(char* funcName);
IROperand* createLabelOperand(int labelId);

IRCode* createLabel(int labelId);
IRCode* createFunction(char* funcName);
IRCode* createAssign(IROperand* src, IROperand* dest);
IRCode* createAdd(IROperand* left, IROperand* right, IROperand* dest);
IRCode* createMinus(IROperand* left, IROperand* right, IROperand* dest);
IRCode* createStar(IROperand* left, IROperand* right, IROperand* dest);
IRCode* createDiv(IROperand* left, IROperand* right, IROperand* dest);
IRCode* createGoto(int labelId);
IRCode* createCond(IROperand* left, IROperand* right, RelOp relOp, IROperand* dest);
IRCode* createReturn(IROperand* op);
IRCode* createDec(IROperand* var, int size);
IRCode* createArg(IROperand* arg);
IRCode* createCall(char* funcName, IROperand* dest);
IRCode* createParam(IROperand* param);
IRCode* createRead(IROperand* dest);
IRCode* createWrite(IROperand* op);

IRCode* concat(IRCode* code1, IRCode* code2);

int generateTempId();
int generateLabelId();
IRCode* generateSampleCode();
IRCode* translateProgram(SyntaxTreeNode* program, SymbolTable symbolTable, SymbolTable functionTable);
void printIRCode(IRCode* code, FILE* out);

#endif

