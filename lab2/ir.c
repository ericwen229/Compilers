#include <stdlib.h>
#include <stdio.h>

#include "ir.h"

IROperand* createOperand() {
	IROperand* newOperand = (IROperand*)malloc(sizeof(IROperand));
	newOperand->funcName = NULL;
	return newOperand;
}

IROperand* createConstOperand(int constValue) {
	IROperand* constOperand = createOperand();
	constOperand->type = IRO_CONST;
	constOperand->constValue = constValue;
	return constOperand;
}

IROperand* createVarOperand(int varId) {
	IROperand* varOperand = createOperand();
	varOperand->type = IRO_VAR;
	varOperand->varId = varId;
	return varOperand;
}

IROperand* createTempOperand(int tempId) {
	IROperand* tempOperand = createOperand();
	tempOperand->type = IRO_TEMP;
	tempOperand->tempId = tempId;
	return tempOperand;
}

IROperand* createGetAddrOperand(bool isTemp, int varId, int tempId) {
	IROperand* getAddrOperand = createOperand();
	getAddrOperand->type = IRO_GETADDR;
	getAddrOperand->isTemp = isTemp;
	getAddrOperand->varId = varId;
	getAddrOperand->tempId = tempId;
	return getAddrOperand;
}

IROperand* createSetAddrOperand(bool isTemp, int varId, int tempId) {
	IROperand* setAddrOperand = createOperand();
	setAddrOperand->type = IRO_SETADDR;
	setAddrOperand->isTemp = isTemp;
	setAddrOperand->varId = varId;
	setAddrOperand->tempId = tempId;
	return setAddrOperand;
}

IROperand* createFuncOperand(char *funcName) {
	IROperand* funcOperand = createOperand();
	funcOperand->type = IRO_FUNC;
	funcOperand->funcName = funcName;
	return funcOperand;
}

IROperand* createLabelOperand(int labelId) {
	IROperand* labelOperand = createOperand();
	labelOperand->type = IRO_LABEL;
	labelOperand->labelId = labelId;
	return labelOperand;
}

// ==========

IRCode* createCode() {
	IRCode* newCode = (IRCode*)malloc(sizeof(IRCode));
	newCode->singleOp.op = NULL;
	newCode->singleOp.dest = NULL;
	newCode->binOp.left = NULL;
	newCode->binOp.right = NULL;
	newCode->binOp.dest = NULL;
	newCode->condOp.left = NULL;
	newCode->condOp.right = NULL;
	newCode->condOp.dest = NULL;
	newCode->decOp.op = NULL;
	newCode->prev = newCode;
	newCode->next = newCode;
	return newCode;
}

IRCode* createLabel(int labelId) {
	IRCode* labelCode = createCode();
	labelCode->type = IR_LABEL;
	labelCode->singleOp.op = createLabelOperand(labelId);
	return labelCode;
}

IRCode* createFunction(char* funcName) {
	IRCode* funcCode = createCode();
	funcCode->type = IR_FUNC;
	funcCode->singleOp.op = createFuncOperand(funcName);
	return funcCode;
}

IRCode* createAssign(IROperand* src, IROperand* dest) {
	IRCode* assignCode = createCode();
	assignCode->type = IR_ASSIGN;
	assignCode->singleOp.op = src;
	assignCode->singleOp.dest = dest;
	return assignCode;
}

IRCode* createAdd(IROperand* left, IROperand* right, IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_ADD;
	code->binOp.left = left;
	code->binOp.right = right;
	code->binOp.dest = dest;
	return code;
}

IRCode* createMinus(IROperand* left, IROperand* right, IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_MINUS;
	code->binOp.left = left;
	code->binOp.right = right;
	code->binOp.dest = dest;
	return code;
}

IRCode* createStar(IROperand* left, IROperand* right, IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_STAR;
	code->binOp.left = left;
	code->binOp.right = right;
	code->binOp.dest = dest;
	return code;
}

IRCode* createDiv(IROperand* left, IROperand* right, IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_DIV;
	code->binOp.left = left;
	code->binOp.right = right;
	code->binOp.dest = dest;
	return code;
}

IRCode* createGoto(int labelId) {
	IRCode* code = createCode();
	code->type = IR_GOTO;
	code->singleOp.op = createLabelOperand(labelId);
	return code;
}

IRCode* createCond(IROperand* left, IROperand* right, RelOp relOp, IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_COND;
	code->condOp.left = left;
	code->condOp.right = right;
	code->condOp.relOp = relOp;
	code->condOp.dest = dest;
	return code;
}

IRCode* createReturn(IROperand* op) {
	IRCode* code = createCode();
	code->type = IR_RETURN;
	code->singleOp.op = op;
	return code;
}

IRCode* createDec(IROperand* var, int size) {
	IRCode* code = createCode();
	code->type = IR_DEC;
	code->decOp.op = var;
	code->decOp.size = size;
	return code;
}

IRCode* createArg(IROperand* arg) {
	IRCode* code = createCode();
	code->type = IR_ARG;
	code->singleOp.op = arg;
	return code;
}

IRCode* createCall(char* funcName, IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_CALL;
	code->singleOp.op = createFuncOperand(funcName);
	code->singleOp.dest = dest;
	return code;
}

IRCode* createParam(IROperand* param) {
	IRCode* code = createCode();
	code->type = IR_PARAM;
	code->singleOp.op = param;
	return code;
}

IRCode* createRead(IROperand* dest) {
	IRCode* code = createCode();
	code->type = IR_READ;
	code->singleOp.dest = dest;
	return code;
}

IRCode* createWrite(IROperand* op) {
	IRCode* code = createCode();
	code->type = IR_WRITE;
	code->singleOp.op = op;
	return code;
}

// ==========

IRCode* concat(IRCode* code1, IRCode* code2) {
	if (code1 == NULL) return code2;
	else if (code2 == NULL) return code1;
	IRCode* code1Tail = code1->prev;
	IRCode* code2Tail = code2->prev;
	code1Tail->next = code2;
	code2->prev = code1Tail;
	code2Tail->next = code1;
	code1->prev = code2Tail;
	return code1;
}

