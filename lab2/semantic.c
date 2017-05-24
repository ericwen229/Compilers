#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semantic.h"

extern bool gError;

static SymbolTableType* gCurrReturnType;

SymbolTableType* handleExp(SyntaxTreeNode* expNode, SymbolTable symbolTable, SymbolTable functionTable);
StructField* handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable, SymbolTable functionTable, bool isStruct, StructField* currField);

StructField* handleStructDefList(SyntaxTreeNode* defListNode, SymbolTable symbolTable) {
	if (defListNode->firstChild == NULL) {
		return NULL;
	}
	StructField* currField = (StructField*)malloc(sizeof(StructField));
	StructField* tailField = handleDef(defListNode->firstChild, symbolTable, NULL, true, currField);
	tailField->nextField = handleStructDefList(defListNode->firstChild->nextSibling, symbolTable);
	return currField;
}

SymbolTableType* handleStructSpecifier(SyntaxTreeNode* structSpecifierNode, SymbolTable symbolTable) {
	SyntaxTreeNode* tagNode = structSpecifierNode->firstChild->nextSibling;
	if (tagNode->type == N_TAG) {
		// current node is StructSpecifier
		// production is STRUCT Tag
		char* structName = retrieveStr(tagNode->firstChild->attr.id);
		SymbolTableType* structDef = querySymbol(symbolTable, structName);
		if (structDef == NULL || structDef->typeType != S_STRUCTDEF) {
			gError = true;
			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", tagNode->lineno, structName);
		}
		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = S_STRUCT;
		newType->type.structName = retrieveStr(tagNode->firstChild->attr.id);
		return newType;
	}
	else {
		// current node is StructSpecifier
		// production is STRUCT OptTag LC DefList RC
		char* structName = NULL;
		if (tagNode->firstChild == NULL) {
			// anonymous struct
			static int anonymousCount = 0;
			char nameBuf[9];
			sprintf(nameBuf, "%x", anonymousCount);
			++ anonymousCount;

			structName = (char*)malloc((2 + strlen(nameBuf)) * sizeof(char));
			structName[0] = '0';
			strcpy(structName + 1, nameBuf);
		}
		else {
			structName = retrieveStr(tagNode->firstChild->attr.id);
		}

		if (querySymbol(symbolTable, structName) != NULL) {
			gError = true;
			printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", tagNode->lineno, structName);
		}
		else {
			SymbolTableType* insertType = initSymbolTableType();
			insertType->typeType = S_STRUCTDEF;
			insertType->type.structType.firstField = handleStructDefList(tagNode->nextSibling->nextSibling, symbolTable);
			insertSymbol(symbolTable, structName, insertType);
		}

		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = S_STRUCT;
		newType->type.structName = structName;
		return newType;
	}
}

SymbolTableType* handleSpecifier(SyntaxTreeNode* specifierNode, SymbolTable symbolTable) {
	specifierNode = specifierNode->firstChild;
	if (specifierNode->type == N_TYPE) { // simple type
		SymbolTableType* newType = initSymbolTableType();
		newType->typeType = S_BASIC;
		newType->type.basicType = specifierNode->attr.typeType;
		return newType;
	}
	else { // struct specifier
		return handleStructSpecifier(specifierNode, symbolTable);
	}
}

void handleVarDec(SymbolTableType* type, SyntaxTreeNode* varDecNode, bool isStruct, StructField* currField, bool isParam, FuncParam* currParam, bool isDefinition) {
	SyntaxTreeNode* child = varDecNode->firstChild;
	if (child->type == N_ID) {
		if (isParam && !isDefinition) {
			currParam->paramType = type;
			return;
		}
		if (child->attr.id->type != NULL) {
			char* idName = retrieveStr(child->attr.id);
			if (!isStruct) {
				gError = true;
				printf("Error type 3 at Line %d: Redefinition of identifier \"%s\".\n", child->lineno, idName);
			}
			else {
				gError = true;
				printf("Error type 15 at Line %d: Redefinition of field \"%s\".\n", child->lineno, idName);
			}
			free(idName);
			freeSymbolTableType(type);
			return;
		}
		else {
			child->attr.id->type = type;
		}

		if (isStruct) {
			currField->fieldName = retrieveStr(child->attr.id);
			currField->fieldType = type;
		}
		else if (isParam) {
			currParam->paramType = type;
		}
	}
	else {
		SymbolTableType* array = initSymbolTableType();
		array->typeType = S_ARRAY;
		array->type.arrayType.len = child->nextSibling->nextSibling->attr.intValue;
		array->type.arrayType.elementType = type;
		handleVarDec(array, child, isStruct, currField, isParam, currParam, isDefinition);
	}
}

void handleDec(SymbolTableType* type, SyntaxTreeNode* decNode, SymbolTable symbolTable, SymbolTable functionTable, bool isStruct, StructField* currField) {
	SyntaxTreeNode* varDecNode = decNode->firstChild;
	handleVarDec(type, varDecNode, isStruct, currField, false, NULL, false);
	if (varDecNode->nextSibling != NULL) {
		if (isStruct) {
			gError = true;
			printf("Error type 15 at Line %d: Trying to initialize struct field.\n", varDecNode->nextSibling->nextSibling->lineno);
			return;
		}
		SyntaxTreeNode* expNode = varDecNode->nextSibling->nextSibling;
		SymbolTableType* expType = handleExp(expNode, symbolTable, functionTable);
		if (!compareSymbolTableType(type, expType)) {
			gError = true;
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", expNode->lineno);
		}
		if (expType != NULL) {
			freeSymbolTableType(expType);
		}
	}
}

StructField* handleDecList(SymbolTableType* type, SyntaxTreeNode* decListNode, SymbolTable symbolTable, SymbolTable functionTable, bool isStruct, StructField* currField) {
	SyntaxTreeNode* decNode = decListNode->firstChild;
	handleDec(copySymbolTableType(type), decListNode->firstChild, symbolTable, functionTable, isStruct, currField);
	if (decNode->nextSibling != NULL) {
		StructField* nextField = NULL;
		if (isStruct) {
			nextField = (StructField*)malloc(sizeof(StructField));
			currField->nextField = nextField;
			nextField->nextField = NULL;
		}
		return handleDecList(type, decNode->nextSibling->nextSibling, symbolTable, functionTable, isStruct, nextField);
	}
	else {
		return currField;
	}
}

StructField* handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable, SymbolTable functionTable, bool isStruct, StructField* currField) {
	SymbolTableType* type = handleSpecifier(defNode->firstChild, symbolTable);
	StructField* field = handleDecList(type, defNode->firstChild->nextSibling, symbolTable, functionTable, isStruct, currField);
	freeSymbolTableType(type);
	return field;
}

void handleExtDecList(SymbolTableType* type, SyntaxTreeNode* extDecListNode) {
	SyntaxTreeNode* varDecNode = extDecListNode->firstChild;
	handleVarDec(type, varDecNode, false, NULL, false, NULL, false);
	if (varDecNode->nextSibling != NULL) {
		handleExtDecList(copySymbolTableType(type), varDecNode->nextSibling->nextSibling);
	}
}

FuncParam* handleParamDec(SyntaxTreeNode* paramDecNode, SymbolTable symbolTable, bool isDefinition) {
	SymbolTableType* type = handleSpecifier(paramDecNode->firstChild, symbolTable);
	FuncParam* currParam = (FuncParam*)malloc(sizeof(FuncParam));
	currParam->nextParam = NULL;
	handleVarDec(type, paramDecNode->firstChild->nextSibling, false, NULL, true, currParam, isDefinition);
	return currParam;
}

FuncParam* handleVarList(SyntaxTreeNode* varListNode, SymbolTable symbolTable, bool isDefinition) {
	FuncParam* currParam = handleParamDec(varListNode->firstChild, symbolTable, isDefinition);
	if (varListNode->firstChild->nextSibling != NULL) {
		currParam->nextParam = handleVarList(varListNode->firstChild->nextSibling->nextSibling, symbolTable, isDefinition);
	}
	else {
		currParam->nextParam = NULL;
	}
	return currParam;
}

void handleFunDec(SymbolTableType* returnType, SyntaxTreeNode* funDecNode, SymbolTable symbolTable, SymbolTable functionTable, bool isDefinition) {
	SyntaxTreeNode* nameNode = funDecNode->firstChild;
	SymbolTableType* funcType = initSymbolTableType();
	funcType->typeType = S_FUNCTION;
	funcType->type.funcType.isDefined = isDefinition;
	funcType->type.funcType.lineno = funDecNode->lineno;
	funcType->type.funcType.returnType = copySymbolTableType(returnType);
	if (nameNode->nextSibling->nextSibling->type == N_RP) {
		funcType->type.funcType.firstParam = NULL;
	}
	else {
		funcType->type.funcType.firstParam = handleVarList(nameNode->nextSibling->nextSibling, symbolTable, isDefinition);
	}

	// transfer funtion from symbol table to function table
	char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
	SymbolTableType* originalType = querySymbol(functionTable, funcName);
	funDecNode->firstChild->attr.id = insertSymbol(functionTable, funcName, originalType);
	free(funcName);

	if (funDecNode->firstChild->attr.id->type == NULL) {
		funDecNode->firstChild->attr.id->type = funcType;
	}
	else if (((SymbolTableType*)(funDecNode->firstChild->attr.id->type))->typeType != S_FUNCTION) {
		char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
		gError = true;
		printf("Error type 3 at Line %d: Redefinition of identifier \"%s\".\n", funDecNode->firstChild->lineno, funcName);
		free(funcName);
		freeSymbolTableType(funcType);
	}
	else if (((SymbolTableType*)(funDecNode->firstChild->attr.id->type))->type.funcType.isDefined) { // has been defined
		if (isDefinition) {
			char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
			gError = true;
			printf("Error type 4 at Line %d: Redefinition of function \"%s\".\n", funDecNode->lineno, funcName);
			free(funcName);
		}
		else {
			SymbolTableType* defineType = funDecNode->firstChild->attr.id->type;
			if (!compareSymbolTableType(defineType, funcType)) {
				char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
				gError = true;
				printf("Error type 19 at Line %d: Function \"%s\" declaration mismatch function definition.\n", funDecNode->lineno, funcName);
				free(funcName);
			}
		}
		freeSymbolTableType(funcType);
	}
	else { // has been declared
		SymbolTableType* declareType = funDecNode->firstChild->attr.id->type;
		bool same = compareSymbolTableType(declareType, funcType);
		if (isDefinition) {
			if (same) {
				declareType->type.funcType.isDefined = true;
			}
			else {
				char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
				gError = true;
				printf("Error type 19 at Line %d: Function \"%s\" definition mismatch function declaration.\n", funDecNode->lineno, funcName);
				free(funcName);
			}
		}
		else {
			if (!same) {
				char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
				gError = true;
				printf("Error type 19 at Line %d: Function \"%s\" declaration mismatch function declaration.\n", funDecNode->lineno, funcName);
				free(funcName);
			}
		}
		freeSymbolTableType(funcType);
	}
} 

void handleExtDef(SyntaxTreeNode* extDefNode, SymbolTable symbolTable, SymbolTable functionTable) {
	SymbolTableType* type = handleSpecifier(extDefNode->firstChild, symbolTable);
	SyntaxTreeNode* secondChild = extDefNode->firstChild->nextSibling;
	if (secondChild->type == N_SEMI) {
	}
	else if (secondChild->type == N_EXTDECLIST) {
		handleExtDecList(copySymbolTableType(type), secondChild);
	}
	else { // Specifier FunDec CompSt
		if (secondChild->nextSibling->type == N_COMPST) { // function definition
			gCurrReturnType = type;
			handleFunDec(type, secondChild, symbolTable, functionTable, true);
			semanticAnalysis(secondChild->nextSibling, symbolTable, functionTable);
		}
		else { // function declaration
			handleFunDec(type, secondChild, symbolTable, functionTable, false);
		}
	}
	freeSymbolTableType(type);
}

bool handleArgs(SyntaxTreeNode* argsNode, FuncParam* param, SymbolTable symbolTable, SymbolTable functionTable) {
	if (argsNode == NULL && param == NULL) {
		return true;
	}
	else if (argsNode == NULL || param == NULL) {
		return false;
	}
	SymbolTableType* currParamType = param->paramType;
	SymbolTableType* currArgType = handleExp(argsNode->firstChild, symbolTable, functionTable);
	if (currArgType == NULL) {
		return false;
	}
	if (!compareSymbolTableType(currParamType, currArgType)) {
		freeSymbolTableType(currArgType);
		return false;
	}
	else {
		freeSymbolTableType(currArgType);
		if (argsNode->firstChild->nextSibling == NULL) {
			return handleArgs(NULL, param->nextParam, symbolTable, functionTable);
		}
		else {
			return handleArgs(argsNode->firstChild->nextSibling->nextSibling, param->nextParam, symbolTable, functionTable);
		}
	}
}

bool isLeftValue(SyntaxTreeNode* expNode) {
	int childNum = numOfChild(expNode);
	if ((childNum == 1 && expNode->firstChild->type == N_ID) ||
			(childNum == 3 && expNode->firstChild->nextSibling->type == N_DOT) ||
			(childNum == 4 && expNode->firstChild->nextSibling->type == N_LB)) {
		return true;
	}
	else {
		return false;
	}
}

SymbolTableType* handleExp(SyntaxTreeNode* expNode, SymbolTable symbolTable, SymbolTable functionTable) {
	int childNum = numOfChild(expNode);
	if (childNum == 1) {
		SyntaxTreeNode* child = expNode->firstChild;
		if (child->type == N_ID) {
			TrieNode* idNode = child->attr.id;
			if (idNode->type == NULL) { // undefined identifier
				char* idStr = retrieveStr(idNode);
				gError = true;
				printf("Error type 1 at Line %d: Undefined identifier \"%s\".\n", child->lineno, idStr);
				free(idStr);
				return NULL;
			}
			return copySymbolTableType(idNode->type);
		}
		else if (child->type == N_INT) {
			SymbolTableType* newType = initSymbolTableType();
			newType->typeType = S_BASIC;
			newType->type.basicType = T_INT;
			return newType;
		}
		else if (child->type == N_FLOAT) {
			SymbolTableType* newType = initSymbolTableType();
			newType->typeType = S_BASIC;
			newType->type.basicType = T_FLOAT;
			return newType;
		}
	}
	else if (childNum == 2) {
		SyntaxTreeNode* firstChild = expNode->firstChild;
		SymbolTableType* expType = handleExp(expNode->firstChild->nextSibling, symbolTable, functionTable);
		if (firstChild->type == N_MINUS) {
			if (expType == NULL || expType->typeType != S_BASIC) {
				gError = true;
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", firstChild->nextSibling->lineno);
				if (expType != NULL) freeSymbolTableType(expType);
				return NULL;
			}
			else {
				return expType;
			}
		}
		else { // N_NOT
			if (expType == NULL || expType->typeType != S_BASIC || expType->type.basicType != N_INT) {
				gError = true;
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", firstChild->nextSibling->lineno);
				if (expType != NULL) freeSymbolTableType(expType);
				return NULL;
			}
			else {
				return expType;
			}
		}
	}
	else if (childNum == 3) {
		SyntaxTreeNode* firstChild = expNode->firstChild;
		if (firstChild->type == N_ID) { // ID LP RP
			TrieNode* funcNode = firstChild->attr.id;
			char* funcName = retrieveStr(funcNode);
			SymbolTableType* type = querySymbol(functionTable, funcName);
			if (type == NULL) { // undefined function
				char* funcName = retrieveStr(funcNode);
				gError = true;
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", firstChild->lineno, funcName);
				free(funcName);
				return NULL;
			}
			else if (type->typeType != S_FUNCTION) {
				char* idName = retrieveStr(funcNode);
				gError = true;
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", firstChild->lineno, idName);
				free(idName);
				return NULL;
			}
			return copySymbolTableType(type->type.funcType.returnType);
		}
		else if (firstChild->type == N_LP) {
			return handleExp(firstChild->nextSibling, symbolTable, functionTable);
		}
		else if (firstChild->nextSibling->type == N_DOT) { // Exp DOT ID
			SymbolTableType *type = handleExp(firstChild, symbolTable, functionTable);
			if (type == NULL || type->typeType != S_STRUCT) {
				SyntaxTreeNode* fieldNode = firstChild->nextSibling->nextSibling;
				gError = true;
				printf("Error type 13 at Line %d: Illegal use of '.'.\n", fieldNode->lineno);
				if (type != NULL) freeSymbolTableType(type);
				return NULL;
			}
			else {
				SyntaxTreeNode* fieldNode = firstChild->nextSibling->nextSibling;
				char* fieldName = retrieveStr(fieldNode->attr.id);
				SymbolTableType* structDef = querySymbol(symbolTable, type->type.structName);
				SymbolTableType* fieldType = queryField(structDef->type.structType.firstField, fieldName);
				if (fieldType == NULL) {
					gError = true;
					printf("Error type 14 at Line %d: Undefined field \"%s\".\n", fieldNode->lineno, fieldName);
				}
				free(fieldName);
				return fieldType;
			}
		}
		else {
			SyntaxTreeNode* firstChild = expNode->firstChild;
			SyntaxTreeNode* secondChild = firstChild->nextSibling;
			SyntaxTreeNode* thirdChild = secondChild->nextSibling;
			if (secondChild->type == N_ASSIGNOP) {
				if (!isLeftValue(firstChild)) {
					gError = true;
					printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", firstChild->lineno);
					return NULL;
				}
				SymbolTableType* leftType = handleExp(firstChild, symbolTable, functionTable);
				SymbolTableType* rightType = handleExp(thirdChild, symbolTable, functionTable);
				if (leftType == NULL || rightType == NULL) {
					gError = true;
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", secondChild->lineno);
					if (leftType != NULL) freeSymbolTableType(leftType);
					if (rightType != NULL) freeSymbolTableType(rightType);
					return NULL;
				}
				SymbolTableType* leftRealType = (leftType->typeType == S_STRUCT)? querySymbol(symbolTable, leftType->type.structName): leftType;
				SymbolTableType* rightRealType = (rightType->typeType == S_STRUCT)? querySymbol(symbolTable, rightType->type.structName): rightType;
				if (!compareSymbolTableType(leftRealType, rightRealType)) {
					gError = true;
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", secondChild->lineno);
					freeSymbolTableType(leftType);
					freeSymbolTableType(rightType);
					return NULL;
				}
				else {
					freeSymbolTableType(rightType);
					return leftType;
				}
			}
			else if (secondChild->type == N_RELOP) {
				SymbolTableType* leftType = handleExp(firstChild, symbolTable, functionTable);
				SymbolTableType* rightType = handleExp(thirdChild, symbolTable, functionTable);
				if (leftType == NULL || leftType->typeType != S_BASIC ||
						rightType == NULL || rightType->typeType != S_BASIC) {
					gError = true;
					printf("Error type 7 at Line %d: Type mismatched for operands.\n", secondChild->lineno);
					if (leftType != NULL) freeSymbolTableType(leftType);
					if (rightType != NULL) freeSymbolTableType(rightType);
					return NULL;
				}
				freeSymbolTableType(leftType);
				freeSymbolTableType(rightType);
				SymbolTableType* newType = initSymbolTableType();
				newType->typeType = S_BASIC;
				newType->type.basicType = N_INT;
				return newType;
			}
			else if (secondChild->type == N_AND || secondChild->type == N_OR) {
				SymbolTableType* leftType = handleExp(firstChild, symbolTable, functionTable);
				SymbolTableType* rightType = handleExp(thirdChild, symbolTable, functionTable);
				if (leftType == NULL || leftType->typeType != S_BASIC || leftType->type.basicType != T_INT ||
						rightType == NULL || rightType->typeType != S_BASIC || rightType->type.basicType != T_INT) {
					gError = true;
					printf("Error type 7 at Line %d: Type mismatched for operands.\n", secondChild->lineno);
					if (leftType != NULL) freeSymbolTableType(leftType);
					if (rightType != NULL) freeSymbolTableType(rightType);
					return NULL;
				}
				freeSymbolTableType(rightType);
				return leftType;
			}
			else { // PLUS MINUS STAR DIV
				SymbolTableType* leftType = handleExp(firstChild, symbolTable, functionTable);
				SymbolTableType* rightType = handleExp(thirdChild, symbolTable, functionTable);
				if (leftType == NULL || leftType->typeType != S_BASIC ||
						rightType == NULL || rightType->typeType != S_BASIC ||
						!compareSymbolTableType(leftType, rightType)) {
					gError = true;
					printf("Error type 7 at Line %d: Type mismatched for operands.\n", secondChild->lineno);
					if (leftType != NULL) freeSymbolTableType(leftType);
					if (rightType != NULL) freeSymbolTableType(rightType);
					return NULL;
				}
				freeSymbolTableType(rightType);
				return leftType;
			}
		}
	}
	else { // childNum == 4
		SyntaxTreeNode* firstChild = expNode->firstChild;
		if (firstChild->type == N_ID) { // ID LP ARGS RP
			TrieNode* idNode = firstChild->attr.id;
			char* funcName = retrieveStr(idNode);
			SymbolTableType* type = querySymbol(functionTable, funcName);
			free(funcName);
			if (type == NULL) { // undefined function
				char* idStr = retrieveStr(idNode);
				gError = true;
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", firstChild->lineno, idStr);
				free(idStr);
				return NULL;
			}
			else if (type->typeType != S_FUNCTION) {
				char* idName = retrieveStr(idNode);
				gError = true;
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", firstChild->lineno, idName);
				free(idName);
				return NULL;
			}
			if (handleArgs(firstChild->nextSibling->nextSibling, type->type.funcType.firstParam, symbolTable, functionTable)) {
				return copySymbolTableType(type->type.funcType.returnType);
			}
			else {
				gError = true;
				printf("Error type 9 at Line %d: Function paramaters mismatch.\n", firstChild->nextSibling->nextSibling->lineno);
				return NULL;
			}
		}
		else { // Exp LB Exp RB
			SymbolTableType* expType = handleExp(firstChild, symbolTable, functionTable);
			if (expType == NULL || expType->typeType != S_ARRAY) {
				gError = true;
				printf("Error type 10 at Line %d: Illegal use of \"[]\".\n", firstChild->lineno);
				if (expType != NULL) freeSymbolTableType(expType);
				return NULL;
			}
			SymbolTableType* indexType = handleExp(firstChild->nextSibling->nextSibling, symbolTable, functionTable);
			if (indexType == NULL) {
				freeSymbolTableType(expType);
				return NULL;
			}
			else if (indexType->typeType != S_BASIC || indexType->type.basicType != T_INT) {
				gError = true;
				printf("Error type 12 at Line %d: Index is not integer.\n", firstChild->nextSibling->nextSibling->lineno);
				freeSymbolTableType(expType);
				freeSymbolTableType(indexType);
				return NULL;
			}
			freeSymbolTableType(indexType);
			return copySymbolTableType(expType->type.arrayType.elementType);
		}
	}
	return NULL;
}

void handleStmt(SyntaxTreeNode* stmtNode, SymbolTable symbolTable, SymbolTable functionTable) {
	if (stmtNode->firstChild->type == N_EXP) {
		SymbolTableType* type = handleExp(stmtNode->firstChild, symbolTable, functionTable);
		if (type != NULL) freeSymbolTableType(type);
	}
	else if (stmtNode->firstChild->type == N_COMPST) {
		semanticAnalysis(stmtNode->firstChild, symbolTable, functionTable);
	}
	else if (stmtNode->firstChild->type == N_RETURN) {
		SymbolTableType* returnType = handleExp(stmtNode->firstChild->nextSibling, symbolTable, functionTable);
		if (!compareSymbolTableType(returnType, gCurrReturnType)) {
			gError = true;
			printf("Error type 8 at Line %d: Type mismatched for return.\n", stmtNode->firstChild->nextSibling->lineno);
		}
		if (returnType != NULL) freeSymbolTableType(returnType);
	}
	else if (stmtNode->firstChild->type == N_IF) {
		SymbolTableType* expType = handleExp(stmtNode->firstChild->nextSibling->nextSibling, symbolTable, functionTable);
		if (expType != NULL) freeSymbolTableType(expType);
		SyntaxTreeNode* firstStmt = stmtNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling;
		handleStmt(firstStmt, symbolTable, functionTable);
		if (firstStmt->nextSibling != NULL) {
			handleStmt(firstStmt->nextSibling->nextSibling, symbolTable, functionTable);
		}
	}
	else { // WHILE LOOP
		SymbolTableType* expType = handleExp(stmtNode->firstChild->nextSibling->nextSibling, symbolTable, functionTable);
		if (expType != NULL) freeSymbolTableType(expType);
		handleStmt(stmtNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, symbolTable, functionTable);
	}
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable, SymbolTable functionTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		handleExtDef(syntaxTreeNode, symbolTable, functionTable);
		return;
	}
	else if (syntaxTreeNode->type == N_DEF) { // local definition
		handleDef(syntaxTreeNode, symbolTable, functionTable, false, NULL);
		return;
	}
	else if (syntaxTreeNode->type == N_STMT) {
		handleStmt(syntaxTreeNode, symbolTable, functionTable);
		return;
	}

	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, symbolTable, functionTable);
		child = child->nextSibling;
	}
}

