#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ir.h"

extern bool gError;

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

IROperand* copyOperand(IROperand* operand) {
	IROperand* newOperand = createOperand();
	(*newOperand) = (*operand);
	return newOperand;
}

void freeOperand(IROperand* operand) {
	if (operand == NULL) return;
	if (operand->type == IRO_FUNC) free(operand->funcName);
	free(operand);
}

RelOp convertRelOp(RelopType relOp) {
	switch (relOp) {
	case LT:
		return IR_LT;
		break;
	case GT:
		return IR_GT;
		break;
	case LE:
		return IR_LE;
		break;
	case GE:
		return IR_GE;
		break;
	case EQ:
		return IR_EQ;
		break;
	case NE:
		return IR_NE;
		break;
	default:
		printf("[unknown relop]\n");
		gError = true;
		return IR_NE;
	}
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

IRCode* translateCond(SyntaxTreeNode* exp, int trueLabel, int falseLabel, SymbolTable symbolTable, SymbolTable functionTable);

IRCode* translateExpLeft(SyntaxTreeNode* exp, IROperand* src, SymbolTable symbolTable, SymbolTable functionTable) {
	if (exp->type != N_EXP) {
		printf("EXP MISMATCH!\n");
		if (src != NULL) freeOperand(src);
		gError = true;
		return NULL;
	}
	if (exp->firstChild->type == N_ID) {
		// ID
		TrieNode* varNode = exp->firstChild->attr.id;
		int varId = ((SymbolTableType*)varNode->type)->id;
		if (src == NULL) return NULL;
		return createAssign(src, createVarOperand(varId, varNode));
	}
	else if (exp->firstChild->nextSibling->type == N_LB) {
		// Exp LB Exp RB
		// TODO: array
		return NULL;
	}
	else {
		// Exp DOT ID
		printf("CANNOT TRANSLATE STRUCTURES!\n");
		if (src != NULL) freeOperand(src);
		gError = true;
		return NULL;
	}
}

IRCode* translateExp(SyntaxTreeNode* exp, IROperand* place, SymbolTable symbolTable, SymbolTable functionTable);

IRCode* translateArgs(SyntaxTreeNode* args, ArgList** argList, SymbolTable symbolTable, SymbolTable functionTable) {
	if (args->type != N_ARGS) {
		printf("ARGS MISMATCH!\n");
		gError = true;
		return NULL;
	}
	if (args->firstChild->nextSibling == NULL) {
		// Exp
		int tempId = generateTempId();
		ArgList* arg = createArgList(createTempOperand(tempId));
		if ((*argList) == NULL) {
			(*argList) = arg;
		}
		else {
			arg->next = (*argList);
			(*argList) = arg;
		}
		return translateExp(args->firstChild, createTempOperand(tempId), symbolTable, functionTable);
	}
	else {
		// Exp COMMA Args
		int tempId = generateTempId();
		ArgList* arg = createArgList(createTempOperand(tempId));
		if ((*argList) == NULL) {
			(*argList) = arg;
		}
		else {
			arg->next = (*argList);
			(*argList) = arg;
		}
		return concat(translateExp(args->firstChild, createTempOperand(tempId), symbolTable, functionTable),
				translateArgs(args->firstChild->nextSibling->nextSibling, argList, symbolTable, functionTable));
	}
}

IRCode* translateExp(SyntaxTreeNode* exp, IROperand* place, SymbolTable symbolTable, SymbolTable functionTable) {
	if (exp->type != N_EXP) {
		printf("EXP MISMATCH!\n");
		if (place != NULL) freeOperand(place);
		gError = true;
		return NULL;
	}
	int childNum = numOfChild(exp);
	if (childNum == 1) {
		if (exp->firstChild->type == N_INT) {
			// INT
			if (place == NULL) return NULL;
			else return createAssign(createConstOperand(exp->firstChild->attr.intValue), place);
		}
		else if (exp->firstChild->type == N_ID) {
			// ID
			TrieNode* varNode = exp->firstChild->attr.id;
			int varId = ((SymbolTableType*)varNode->type)->id;
			if (place == NULL) return NULL;
			else return createAssign(createVarOperand(varId, varNode), place);
		}
		else {
			// FLOAT
			printf("CANNOT TRANSLATE FLOATING NUMBERS!\n");
			if (place != NULL) freeOperand(place);
			gError = true;
			return NULL;
		}
	}
	else if (childNum == 2) {
		if (exp->firstChild->type == N_MINUS) {
			// MINUS Exp
			if (place == NULL) return translateExp(exp->firstChild->nextSibling, NULL, symbolTable, functionTable);
			else {
				int tempId = generateTempId();
				return concat(translateExp(exp->firstChild->nextSibling, createTempOperand(tempId), symbolTable, functionTable),
						createMinus(createConstOperand(0), createTempOperand(tempId), place));
			}
		}
		else {
			// NOT Exp
			if (place == NULL) return translateExp(exp->firstChild->nextSibling, NULL, symbolTable, functionTable);	
			int label1 = generateLabelId();
			int label2 = generateLabelId();
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, createAssign(createConstOperand(0), place));
			finalCode = concat(finalCode, translateCond(exp, label1, label2, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label1));
			finalCode = concat(finalCode, createAssign(createConstOperand(1), copyOperand(place)));
			finalCode = concat(finalCode, createLabel(label2));
			return finalCode;
		}
	}
	else if (childNum == 3) {
		if (exp->firstChild->type == N_ID) {
			// ID LP RP
			int tempId = generateTempId();
			char* funcName = retrieveStr(exp->firstChild->attr.id);
			IRCode* finalCode = NULL;
			if (strcmp(funcName, "read") == 0) {
				finalCode = concat(finalCode, createRead(createTempOperand(tempId)));
			}
			else {
				finalCode = concat(finalCode, createCall(funcName, createTempOperand(tempId)));
			}
			free(funcName);
			if (place != NULL) {
				finalCode = concat(finalCode, createAssign(createTempOperand(tempId), place));
			}
			return finalCode;
		}
		else if (exp->firstChild->type == N_LP) {
			// LP Exp RP
			return translateExp(exp->firstChild->nextSibling, place, symbolTable, functionTable);
		}
		else if (exp->firstChild->nextSibling->type == N_DOT) {
			// Exp DOT ID
			printf("CANNOT TRANSLATE STRUCTURES!\n");
			if (place != NULL) freeOperand(place);
			gError = true;
			return NULL;
		}
		else {
			SyntaxTreeNode* firstChild = exp->firstChild;
			SyntaxTreeNode* secondChild = firstChild->nextSibling;
			SyntaxTreeNode* thirdChild = secondChild->nextSibling;
			if (secondChild->type == N_ASSIGNOP) {
				// Exp ASSIGNOP Exp
				int tempId = generateTempId();
				IRCode* finalCode = NULL;
				finalCode = concat(finalCode, translateExp(thirdChild, createTempOperand(tempId), symbolTable, functionTable));
				finalCode = concat(finalCode, translateExpLeft(firstChild, createTempOperand(tempId), symbolTable, functionTable));
				if (place != NULL) {
					finalCode = concat(finalCode, createAssign(createTempOperand(tempId), place));
				}
				return finalCode;
			}
			else if (secondChild->type == N_RELOP) {
				// Exp RELOP Exp
				if (place == NULL) {
					return concat(translateExp(firstChild, NULL, symbolTable, functionTable),
							translateExp(thirdChild, NULL, symbolTable, functionTable));
				}
				int label1 = generateLabelId();
				int label2 = generateLabelId();
				IRCode* finalCode = NULL;
				finalCode = concat(finalCode, createAssign(createConstOperand(0), place));
				finalCode = concat(finalCode, translateCond(exp, label1, label2, symbolTable, functionTable));
				finalCode = concat(finalCode, createLabel(label1));
				finalCode = concat(finalCode, createAssign(createConstOperand(1), copyOperand(place)));
				finalCode = concat(finalCode, createLabel(label2));
				return finalCode;
			}
			else if (secondChild->type == N_AND || secondChild->type == N_OR) {
				// Exp AND/OR Exp
				if (place == NULL) {
					return concat(translateExp(firstChild, NULL, symbolTable, functionTable),
							translateExp(thirdChild, NULL, symbolTable, functionTable));
				}
				int label1 = generateLabelId();
				int label2 = generateLabelId();
				IRCode* finalCode = NULL;
				finalCode = concat(finalCode, createAssign(createConstOperand(0), place));
				finalCode = concat(finalCode, translateCond(exp, label1, label2, symbolTable, functionTable));
				finalCode = concat(finalCode, createLabel(label1));
				finalCode = concat(finalCode, createAssign(createConstOperand(1), copyOperand(place)));
				finalCode = concat(finalCode, createLabel(label2));
				return finalCode;
			}
			else {
				// Exp PLUS/MINUS/STAR/DIV Exp
				if (place == NULL) {
					return concat(translateExp(firstChild, NULL, symbolTable, functionTable),
							translateExp(thirdChild, NULL, symbolTable, functionTable));
				}
				int temp1 = generateTempId();
				int temp2 = generateTempId();
				IRCode* finalCode = NULL;
				finalCode = concat(finalCode, translateExp(firstChild, createTempOperand(temp1), symbolTable, functionTable));
				finalCode = concat(finalCode, translateExp(firstChild->nextSibling->nextSibling, createTempOperand(temp2), symbolTable, functionTable));
				switch (secondChild->type) {
				case N_PLUS:
					finalCode = concat(finalCode, createAdd(createTempOperand(temp1), createTempOperand(temp2), place));
					break;
				case N_MINUS:
					finalCode = concat(finalCode, createMinus(createTempOperand(temp1), createTempOperand(temp2), place));
					break;
				case N_STAR:
					finalCode = concat(finalCode, createStar(createTempOperand(temp1), createTempOperand(temp2), place));
					break;
				default:
					finalCode = concat(finalCode, createDiv(createTempOperand(temp1), createTempOperand(temp2), place));
					break;
				}
				return finalCode;
			}
		}
	}
	else { // childNum == 4
		if (exp->firstChild->type == N_ID) {
			// ID LP ARGS RP
			char* funcName = retrieveStr(exp->firstChild->attr.id);
			ArgList* argList = NULL;
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, translateArgs(exp->firstChild->nextSibling->nextSibling, &argList, symbolTable, functionTable));
			if (strcmp(funcName, "write") == 0) {
				finalCode = concat(finalCode, createWrite(copyOperand(argList->arg)));
				freeArgList(argList);
				free(funcName);
			}
			else {
				ArgList* currArg = argList;
				while (currArg != NULL) {
					finalCode = concat(finalCode, createArg(copyOperand(currArg->arg)));
					currArg = currArg->next;
				}
				freeArgList(argList);

				if (place != NULL) {
					finalCode = concat(finalCode, createCall(funcName, place));
				}
				else {
					int tempId = generateTempId();
					finalCode = concat(finalCode, createCall(funcName, createTempOperand(tempId)));
				}
			}
			return finalCode;
		}
		else {
			// Exp LB Exp RB
			// TODO: array
			return NULL;
		}
	}
}
	
IRCode* translateCond(SyntaxTreeNode* exp, int trueLabel, int falseLabel, SymbolTable symbolTable, SymbolTable functionTable) {
	if (exp->type != N_EXP) {
		printf("COND MISMATCH!\n");
		gError = true;
		return NULL;
	}
	int childNum = numOfChild(exp);
	if (childNum == 2 && exp->firstChild->type == N_NOT) {
		return translateCond(exp->firstChild->nextSibling, falseLabel, trueLabel, symbolTable, functionTable);
	}
	else if (childNum == 3) {
		SyntaxTreeNode* secondChild = exp->firstChild->nextSibling;
		if (secondChild->type == N_RELOP) {
			int temp1 = generateTempId();
			int temp2 = generateTempId();
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, translateExp(exp->firstChild, createTempOperand(temp1), symbolTable, functionTable));
			finalCode = concat(finalCode, translateExp(secondChild->nextSibling, createTempOperand(temp2), symbolTable, functionTable));
			finalCode = concat(finalCode, createCond(createTempOperand(temp1), createTempOperand(temp2), convertRelOp(secondChild->attr.relopType), createLabelOperand(trueLabel)));
			finalCode = concat(finalCode, createGoto(falseLabel));
			return finalCode;
		}
		else if (secondChild->type == N_AND) {
			int label1 = generateLabelId();
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, translateCond(exp->firstChild, label1, falseLabel, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label1));
			finalCode = concat(finalCode, translateCond(secondChild->nextSibling, trueLabel, falseLabel, symbolTable, functionTable));
			return finalCode;
		}
		else if (secondChild->type == N_OR) {
			int label1 = generateLabelId();
			IRCode* finalCode = NULL;
			finalCode = concat(finalCode, translateCond(exp->firstChild, trueLabel, label1, symbolTable, functionTable));
			finalCode = concat(finalCode, createLabel(label1));
			finalCode = concat(finalCode, translateCond(secondChild->nextSibling, trueLabel, falseLabel, symbolTable, functionTable));
			return finalCode;
		}
	}
	int tempId = generateTempId();
	IRCode* finalCode = NULL;
	finalCode = concat(finalCode, translateExp(exp, createTempOperand(tempId), symbolTable, functionTable));
	finalCode = concat(finalCode, createCond(createTempOperand(tempId), createConstOperand(0), IR_NE, createLabelOperand(trueLabel)));
	finalCode = concat(finalCode, createGoto(falseLabel));
	return finalCode;
}

IRCode* translateCompSt(SyntaxTreeNode* compSt, SymbolTable symbolTable, SymbolTable functionTable);

IRCode* translateStmt(SyntaxTreeNode* stmt, SymbolTable symbolTable, SymbolTable functionTable) {
	if (stmt->type != N_STMT) {
		printf("STMT MISMATCH!\n");
		gError = true;
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
	if (defList->type != N_DEFLIST) {
		printf("DEFLIST MISMATCH!\n");
		gError = true;
		return NULL;
	}
	// TODO: array
	return NULL;
}

IRCode* translateStmtList(SyntaxTreeNode* stmtList, SymbolTable symbolTable, SymbolTable functionTable) {
	if (stmtList->type != N_STMTLIST) {
		printf("STMTLIST MISMATCH!\n");
		gError = true;
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
		gError = true;
		return NULL;
	}
	return concat(translateDefList(compSt->firstChild->nextSibling, symbolTable, functionTable),
			translateStmtList(compSt->firstChild->nextSibling->nextSibling, symbolTable, functionTable));
}

IRCode* translateVarDec(SyntaxTreeNode* varDec, SymbolTable symbolTable, SymbolTable functionTable) {
	if (varDec->type != N_VARDEC) {
		printf("VARDEC MISMATCH!\n");
		gError = true;
		return NULL;
	}
	if (varDec->firstChild->type == N_ID) {
		TrieNode* varNode = varDec->firstChild->attr.id;
		int varId = ((SymbolTableType*)varDec->firstChild->attr.id->type)->id;
		return createParam(createVarOperand(varId, varNode));
	}
	else {
		return translateVarDec(varDec->firstChild, symbolTable, functionTable);
	}
}

IRCode* translateParamDec(SyntaxTreeNode* paramDec, SymbolTable symbolTable, SymbolTable functionTable) {
	if (paramDec->type != N_PARAMDEC) {
		printf("PARAMDEC MISMATCH!\n");
		gError = true;
		return NULL;
	}
	return translateVarDec(paramDec->firstChild->nextSibling, symbolTable, functionTable);
}

IRCode* translateVarList(SyntaxTreeNode* varList, SymbolTable symbolTable, SymbolTable functionTable) {
	if (varList->type != N_VARLIST) {
		printf("VARLIST MISMATCH!\n");
		gError = true;
		return NULL;
	}
	IRCode* finalCode = NULL;
	finalCode = concat(finalCode, translateParamDec(varList->firstChild, symbolTable, functionTable));
	if (varList->firstChild->nextSibling != NULL) {
		finalCode = concat(finalCode, translateVarList(varList->firstChild->nextSibling->nextSibling, symbolTable, functionTable));
	}
	return finalCode;
}

IRCode* translateFunDec(SyntaxTreeNode* funDec, SymbolTable symbolTable, SymbolTable functionTable) {
	if (funDec->type != N_FUNDEC) {
		printf("FUNDEC MISMATCH!\n");
		gError = true;
		return NULL;
	}
	char* funcName = retrieveStr(funDec->firstChild->attr.id);
	IRCode* finalCode = NULL;
	finalCode = concat(finalCode, createFunction(funcName));
	if (funDec->firstChild->nextSibling->nextSibling->nextSibling != NULL) {
		finalCode = concat(finalCode, translateVarList(funDec->firstChild->nextSibling->nextSibling, symbolTable, functionTable));
	}
	return finalCode;
}

IRCode* translateExtDef(SyntaxTreeNode* extDef, SymbolTable symbolTable, SymbolTable functionTable) {
	if (extDef->type != N_EXTDEF) {
		printf("EXTDEF MISMATCH!\n");
		gError = true;
		return NULL;
	}
	if (extDef->firstChild->nextSibling->type == N_FUNDEC) {
		// Specifier FunDec Compst
		return concat(translateFunDec(extDef->firstChild->nextSibling, symbolTable, functionTable),
				translateCompSt(extDef->firstChild->nextSibling->nextSibling, symbolTable, functionTable));
	}
	else if (extDef->firstChild->nextSibling->type == N_SEMI) {
		// Specifier SEMI
		printf("CANNOT TRANSLATE TYPE DECLARATIONS!\n");
		gError = true;
		return NULL;
	}
	else {
		// Specifier ExtDecList SEMI
		printf("CANNOT TRANSLATE GLOBAL VARIABLES!");
		gError = true;
		return NULL;
	}
}

IRCode* translateExtDefList(SyntaxTreeNode* extDefList, SymbolTable symbolTable, SymbolTable functionTable) {
	if (extDefList->type != N_EXTDEFLIST) {
		printf("EXTDEFLIST MISMATCH!\n");
		gError = true;
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
		gError = true;
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
		gError = true;
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
		case IR_NE:
			fprintf(out, " != ");
			break;
		default:
			fprintf(out, " [unknown relop] ");
			gError = true;
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
		gError = true;
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

void _freeIRCode(IRCode* code) {
	if (code == NULL) return;
	switch (code->type) {
	case IR_LABEL:
		freeOperand(code->singleOp.op);
		break;
	case IR_FUNC:
		freeOperand(code->singleOp.op);
		break;
	case IR_ASSIGN:
		freeOperand(code->singleOp.dest);
		freeOperand(code->singleOp.op);
		break;
	case IR_ADD:
		freeOperand(code->binOp.dest);
		freeOperand(code->binOp.left);
		freeOperand(code->binOp.right);
		break;
	case IR_MINUS:
		freeOperand(code->binOp.dest);
		freeOperand(code->binOp.left);
		freeOperand(code->binOp.right);
		break;
	case IR_STAR:
		freeOperand(code->binOp.dest);
		freeOperand(code->binOp.left);
		freeOperand(code->binOp.right);
		break;
	case IR_DIV:
		freeOperand(code->binOp.dest);
		freeOperand(code->binOp.left);
		freeOperand(code->binOp.right);
		break;
	case IR_GOTO:
		freeOperand(code->singleOp.op);
		break;
	case IR_COND:
		freeOperand(code->condOp.left);
		freeOperand(code->condOp.right);
		freeOperand(code->condOp.dest);
		break;
	case IR_RETURN:
		freeOperand(code->singleOp.op);
		break;
	case IR_DEC:
		freeOperand(code->decOp.op);
		break;
	case IR_ARG:
		freeOperand(code->singleOp.op);
		break;
	case IR_CALL:
		freeOperand(code->singleOp.dest);
		freeOperand(code->singleOp.op);
		break;
	case IR_PARAM:
		freeOperand(code->singleOp.op);
		break;
	case IR_READ:
		freeOperand(code->singleOp.dest);
		break;
	case IR_WRITE:
		freeOperand(code->singleOp.op);
		break;
	default:
		gError = true;
		break;
	}
}

void freeIRCode(IRCode* code) {
	if (code == NULL) return;
	IRCode* head = code;

	IRCode* curr = head;
	IRCode* next = curr->next;
	_freeIRCode(curr);

	while (next != head) {
		curr = next;
		next = curr->next;
		_freeIRCode(curr);
	}
}

ArgList* createArgList(IROperand* arg) {
	ArgList* newArg = (ArgList*)malloc(sizeof(ArgList));
	newArg->arg = arg;
	newArg->next = NULL;
	return newArg;
}

void freeArgList(ArgList* argList) {
	if (argList == NULL) return;
	ArgList* curr = argList;
	ArgList* next = curr->next;
	freeOperand(curr->arg);
	free(curr);
	while (next != NULL) {
		curr = next;
		next = curr->next;
		freeOperand(curr->arg);
		free(curr);
	}
}

