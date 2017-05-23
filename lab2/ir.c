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

IROperand* createVarOperand(int varId, TrieNode* varNode) {
	IROperand* varOperand = createOperand();
	varOperand->type = IRO_VAR;
	varOperand->varId = varId;
	varOperand->varNode = varNode;
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

// ==========

IRCode* generateSampleCode() {
	IRCode* finalCode = NULL;
	// FUNCTION main :
	finalCode = concat(finalCode, createFunction("main"));
	// READ t1
	finalCode = concat(finalCode, createRead(createTempOperand(1)));
	// v1 := t1
	finalCode = concat(finalCode, createAssign(createTempOperand(1), createVarOperand(1, NULL)));
	// t2 := #0
	finalCode = concat(finalCode, createAssign(createConstOperand(0), createTempOperand(2)));
	// IF v1 > t2 GOTO label1
	finalCode = concat(finalCode, createCond(createVarOperand(1, NULL), createTempOperand(2), IR_GT, createLabelOperand(1)));
	// IF v1 < t2 GOTO label2
	finalCode = concat(finalCode, createCond(createVarOperand(1, NULL), createTempOperand(2), IR_LT, createLabelOperand(1)));
	// WRITE t2
	finalCode = concat(finalCode, createWrite(createTempOperand(2)));
	// GOTO label3
	finalCode = concat(finalCode, createGoto(3));
	// LABEL label1 :
	finalCode = concat(finalCode, createLabel(1));
	// t3 := #1
	finalCode = concat(finalCode, createAssign(createConstOperand(1), createTempOperand(3)));
	// WRITE t3
	finalCode = concat(finalCode, createWrite(createTempOperand(3)));
	// GOTO label3
	finalCode = concat(finalCode, createGoto(3));
	// LABEL label2 :
	finalCode = concat(finalCode, createLabel(2));
	// t6 := #-1
	finalCode = concat(finalCode, createAssign(createConstOperand(-1), createTempOperand(6)));
	// WRITE t6
	finalCode = concat(finalCode, createWrite(createTempOperand(6)));
	// LABEL label3 :
	finalCode = concat(finalCode, createLabel(3));
	// RETURN t2
	finalCode = concat(finalCode, createReturn(createTempOperand(2)));
	return finalCode;
}

// ==========

IRCode* translateExp(SyntaxTreeNode* exp, IROperand* place, SymbolTable symbolTable, SymbolTable functionTable) {
	// TODO
	return NULL;
}
	
IRCode* translateCond(SyntaxTreeNode* exp, int trueLabel, int falseLabel, SymbolTable symbolTable, SymbolTable functionTable) {
	// TODO
	return NULL;
}

IRCode* translateCompSt(SyntaxTreeNode* compSt, SymbolTable symbolTable, SymbolTable functionTable);

IRCode* translateStmt(SyntaxTreeNode* stmt, SymbolTable symbolTable, SymbolTable functionTable) {
	if (stmt->type != N_STMT) {
		printf("STMT MISMATCH!\n");
		return NULL;
	}
	SyntaxTreeNode* firstChild = stmt->firstChild;
	if (firstChild->type == N_EXP) {
		// EXP SEMI
		return translateExp(firstChild, NULL, symbolTable, functionTable);
	}
	else if (firstChild->type == N_COMPST) {
		// COMPST
		return translateCompSt(firstChild, symbolTable, functionTable);
	}
	else if (firstChild->type == N_RETURN) {
		// RETURN EXP
		int tempId = generateTempId();
		return concat(translateExp(firstChild->nextSibling, createTempOperand(tempId), symbolTable, functionTable),
				createReturn(createTempOperand(tempId)));
	}
	else if (firstChild->type == N_IF) {
		// IF LP Exp RP Stmt
		// IF LP Exp RP Stmt ELSE Stmt
		SyntaxTreeNode* exp = firstChild->nextSibling->nextSibling;
		SyntaxTreeNode* trueStmt = exp->nextSibling->nextSibling;
		if (trueStmt->nextSibling == NULL) {
			// IF LP Exp RP Stmt
			int label1 = generateLabelId();
			int label2 = generateLabelId();
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, translateCond(exp, label1, label2, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label1));
			finalCode = concat(finalCode, translateStmt(trueStmt, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label2));
			return finalCode;
		}
		else {
			// IF LP Exp RP Stmt ELSE Stmt
			SyntaxTreeNode* falseStmt = trueStmt->nextSibling->nextSibling;
			int label1 = generateLabelId();
			int label2 = generateLabelId();
			int label3 = generateLabelId();
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, translateCond(exp, label1, label2, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label1));
			finalCode = concat(finalCode, translateStmt(trueStmt, symbolTable, functionTable));
			finalCode = concat(finalCode, createGoto(label3));
			finalCode = concat(finalCode, createLabel(label2));
			finalCode = concat(finalCode, translateStmt(falseStmt, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label3));
			return finalCode;
		}
	}
	else {
		// WHILE LP Exp RP Stmt
		SyntaxTreeNode* exp = firstChild->nextSibling->nextSibling;
		SyntaxTreeNode* stmt = exp->nextSibling->nextSibling;
		int label1 = generateLabelId();
		int label2 = generateLabelId();
		int label3 = generateLabelId();
		IRCode* finalCode = NULL;
		finalCode = concat(finalCode, createLabel(label1));
		finalCode = concat(finalCode, translateCond(exp, label2, label3, symbolTable, functionTable));
		finalCode = concat(finalCode, createLabel(label2));
		finalCode = concat(finalCode, translateStmt(stmt, symbolTable, functionTable));
		finalCode = concat(finalCode, createGoto(1));
		finalCode = concat(finalCode, createLabel(label3));
		return finalCode;
	}
}

IRCode* translateDefList(SyntaxTreeNode* defList, SymbolTable symbolTable, SymbolTable functionTable) {
	// TODO
	if (defList->type != N_DEFLIST) {
		printf("DEFLIST MISMATCH!\n");
		return NULL;
	}
	return NULL;
}

IRCode* translateStmtList(SyntaxTreeNode* stmtList, SymbolTable symbolTable, SymbolTable functionTable) {
	if (stmtList->type != N_STMTLIST) {
		printf("STMTLIST MISMATCH!\n");
		return NULL;
	}
	if (stmtList->firstChild == NULL) {
		return NULL;
	}
	else {
		return concat(translateStmt(stmtList->firstChild, symbolTable, functionTable), translateStmtList(stmtList->firstChild->nextSibling, symbolTable, functionTable));
	}
}

IRCode* translateCompSt(SyntaxTreeNode* compSt, SymbolTable symbolTable, SymbolTable functionTable) {
	if (compSt->type != N_COMPST) {
		printf("COMPST MISMATCH!\n");
		return NULL;
	}
	return concat(translateDefList(compSt->firstChild->nextSibling, symbolTable, functionTable),
			translateStmtList(compSt->firstChild->nextSibling->nextSibling, symbolTable, functionTable));
}

IRCode* translateExtDef(SyntaxTreeNode* extDef, SymbolTable symbolTable, SymbolTable functionTable) {
	if (extDef->type != N_EXTDEF) {
		printf("EXTDEF MISMATCH!\n");
		return NULL;
	}
	// Specifier FunDec Compst
	char* funcName = retrieveStr(extDef->firstChild->nextSibling->firstChild->attr.id);
	return concat(createFunction(funcName), translateCompSt(extDef->firstChild->nextSibling->nextSibling, symbolTable, functionTable));
}

IRCode* translateExtDefList(SyntaxTreeNode* extDefList, SymbolTable symbolTable, SymbolTable functionTable) {
	if (extDefList->type != N_EXTDEFLIST) {
		printf("EXTDEFLIST MISMATCH!\n");
		return NULL;
	}
	if (extDefList->firstChild == NULL) {
		return NULL;
	}
	else {
		return concat(translateExtDef(extDefList->firstChild, symbolTable, functionTable),
				translateExtDefList(extDefList->firstChild->nextSibling, symbolTable, functionTable));
	}
}

IRCode* translateProgram(SyntaxTreeNode* program, SymbolTable symbolTable, SymbolTable functionTable) {
	if (program->type != N_PROGRAM) {
		printf("PROGRAM MISMATCH!\n");
		return NULL;
	}
	return translateExtDefList(program->firstChild, symbolTable, functionTable);
}

// ==========

int generateTempId() {
	static int id = -1;
	++ id;
	return id;
}

int generateLabelId() {
	static int id = -1;
	++ id;
	return id;
}

void printOperand(IROperand* op, FILE* out) {
	if (op == NULL) return;
	switch (op->type) {
	case IRO_CONST:
		fprintf(out, "#%d", op->constValue);
		break;
	case IRO_VAR:
		fprintf(out, "v%d", op->varId);
		break;
	case IRO_TEMP:
		fprintf(out, "t%d", op->tempId);
		break;
	case IRO_GETADDR:
		if (op->isTemp) fprintf(out, "&t%d", op->tempId);
		else fprintf(out, "&v%d", op->varId);
		break;
	case IRO_SETADDR:
		if (op->isTemp) fprintf(out, "*t%d", op->tempId);
		else fprintf(out, "*v%d", op->varId);
		break;
	case IRO_FUNC:
		fprintf(out, "%s", op->funcName);
		break;
	case IRO_LABEL:
		fprintf(out, "label%d", op->labelId);
		break;
	default:
		fprintf(out, "[unknown operand]");
		break;
	}
}

void _printIRCode(IRCode* code, FILE* out) {
	if (code == NULL) return;
	switch (code->type) {
	case IR_LABEL:
		fprintf(out, "LABEL ");
		printOperand(code->singleOp.op, out);
		fprintf(out, " :\n");
		break;
	case IR_FUNC:
		fprintf(out, "FUNCTION ");
		printOperand(code->singleOp.op, out);
		fprintf(out, " :\n");
		break;
	case IR_ASSIGN:
		printOperand(code->singleOp.dest, out);
		fprintf(out, " := ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	case IR_ADD:
		printOperand(code->binOp.dest, out);
		fprintf(out, " := ");
		printOperand(code->binOp.left, out);
		fprintf(out, " + ");
		printOperand(code->binOp.right, out);
		fprintf(out, "\n");
		break;
	case IR_MINUS:
		printOperand(code->binOp.dest, out);
		fprintf(out, " := ");
		printOperand(code->binOp.left, out);
		fprintf(out, " - ");
		printOperand(code->binOp.right, out);
		fprintf(out, "\n");
		break;
	case IR_STAR:
		printOperand(code->binOp.dest, out);
		fprintf(out, " := ");
		printOperand(code->binOp.left, out);
		fprintf(out, " * ");
		printOperand(code->binOp.right, out);
		fprintf(out, "\n");
		break;
	case IR_DIV:
		printOperand(code->binOp.dest, out);
		fprintf(out, " := ");
		printOperand(code->binOp.left, out);
		fprintf(out, " / ");
		printOperand(code->binOp.right, out);
		fprintf(out, "\n");
		break;
	case IR_GOTO:
		fprintf(out, "GOTO ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	case IR_COND:
		fprintf(out, "IF ");
		printOperand(code->condOp.left, out);
		switch (code->condOp.relOp) {
		case IR_LT:
			fprintf(out, " < ");
			break;
		case IR_LE:
			fprintf(out, " <= ");
			break;
		case IR_EQ:
			fprintf(out, " == ");
			break;
		case IR_GE:
			fprintf(out, " >= ");
			break;
		case IR_GT:
			fprintf(out, " > ");
			break;
		default:
			fprintf(out, " [unknown relop] ");
			break;
		}
		printOperand(code->condOp.right, out);
		fprintf(out, " GOTO ");
		printOperand(code->condOp.dest, out);
		fprintf(out, "\n");
		break;
	case IR_RETURN:
		fprintf(out, "RETURN ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	case IR_DEC:
		fprintf(out, "DEC ");
		printOperand(code->decOp.op, out);
		fprintf(out, " %d\n", code->decOp.size);
		break;
	case IR_ARG:
		fprintf(out, "ARG ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	case IR_CALL:
		printOperand(code->singleOp.dest, out);
		fprintf(out, " := CALL ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	case IR_PARAM:
		fprintf(out, "PARAM ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	case IR_READ:
		fprintf(out, "READ ");
		printOperand(code->singleOp.dest, out);
		fprintf(out, "\n");
		break;
	case IR_WRITE:
		fprintf(out, "WRITE ");
		printOperand(code->singleOp.op, out);
		fprintf(out, "\n");
		break;
	default:
		fprintf(out, "[unknown code]\n");
		break;
	}
}

void printIRCode(IRCode* code, FILE* out) {
	if (code == NULL) return;
	IRCode* head = code;
	_printIRCode(head, out);
	code = code->next;
	while (code != head) {
		_printIRCode(code, out);
		code = code->next;
	}
}
