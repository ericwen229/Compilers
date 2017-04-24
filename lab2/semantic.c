#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "semantic.h"

StructField* handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable, bool isStruct, StructField* currField);

StructField* handleStructDefList(SyntaxTreeNode* defListNode, SymbolTable symbolTable) {
	if (defListNode->firstChild == NULL) {
		return NULL;
	}
	StructField* currField = (StructField*)malloc(sizeof(StructField));
	StructField* tailField = handleDef(defListNode->firstChild, symbolTable, true, currField);
	tailField->nextField = handleStructDefList(defListNode->firstChild->nextSibling, symbolTable);
	return currField;
}

SymbolTableType* handleStructSpecifier(SyntaxTreeNode* structSpecifierNode, SymbolTable symbolTable) {
	SyntaxTreeNode* tagNode = structSpecifierNode->firstChild->nextSibling;
	if (tagNode->type == N_TAG) {
		// current node is StructSpecifier
		// production is STRUCT Tag
		// TODO: check definition
		char* structName = retrieveStr(tagNode->firstChild->attr.id);
		SymbolTableType* structDef = querySymbol(symbolTable, structName);
		if (structDef == NULL || structDef->typeType != S_STRUCTDEF) {
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
				printf("Error type 3 at Line %d: Redefinition of identifier \"%s\".\n", child->lineno, idName);
			}
			else {
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

void handleDec(SymbolTableType* type, SyntaxTreeNode* decNode, bool isStruct, StructField* currField) {
	SyntaxTreeNode* varDecNode = decNode->firstChild;
	handleVarDec(type, varDecNode, isStruct, currField, false, NULL, false);
	if (isStruct && varDecNode->nextSibling != NULL) {
		printf("Error type 15 at Line %d: Trying to initialize struct field.\n", varDecNode->nextSibling->nextSibling->lineno);
	}
}

StructField* handleDecList(SymbolTableType* type, SyntaxTreeNode* decListNode, bool isStruct, StructField* currField) {
	SyntaxTreeNode* decNode = decListNode->firstChild;
	handleDec(copySymbolTableType(type), decListNode->firstChild, isStruct, currField);
	if (decNode->nextSibling != NULL) {
		StructField* nextField = NULL;
		if (isStruct) {
			nextField = (StructField*)malloc(sizeof(StructField));
			currField->nextField = nextField;
			nextField->nextField = NULL;
		}
		return handleDecList(type, decNode->nextSibling->nextSibling, isStruct, nextField);
	}
	else {
		return currField;
	}
}

StructField* handleDef(SyntaxTreeNode* defNode, SymbolTable symbolTable, bool isStruct, StructField* currField) {
	SymbolTableType* type = handleSpecifier(defNode->firstChild, symbolTable);
	StructField* field = handleDecList(type, defNode->firstChild->nextSibling, isStruct, currField);
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

void handleFunDec(SymbolTableType* returnType, SyntaxTreeNode* funDecNode, SymbolTable symbolTable, bool isDefinition) {
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

	if (funDecNode->firstChild->attr.id->type == NULL) {
		funDecNode->firstChild->attr.id->type = funcType;
	}
	else if (((SymbolTableType*)(funDecNode->firstChild->attr.id->type))->type.funcType.isDefined) { // has been defined
		if (isDefinition) {
			char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
			printf("Error type 4 at Line %d: Redefinition of function \"%s\".\n", funDecNode->lineno, funcName);
			free(funcName);
		}
		else {
			SymbolTableType* defineType = funDecNode->firstChild->attr.id->type;
			if (!compareSymbolTableType(defineType, funcType)) {
				char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
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
				printf("Error type 19 at Line %d: Function \"%s\" definition mismatch function declaration.\n", funDecNode->lineno, funcName);
				free(funcName);
			}
		}
		else {
			if (!same) {
				char* funcName = retrieveStr(funDecNode->firstChild->attr.id);
				printf("Error type 19 at Line %d: Function \"%s\" declaration mismatch function declaration.\n", funDecNode->lineno, funcName);
				free(funcName);
			}
		}
		freeSymbolTableType(funcType);
	}
} 

void handleExtDef(SyntaxTreeNode* extDefNode, SymbolTable symbolTable) {
	SymbolTableType* type = handleSpecifier(extDefNode->firstChild, symbolTable);
	SyntaxTreeNode* secondChild = extDefNode->firstChild->nextSibling;
	if (secondChild->type == N_SEMI) {
	}
	else if (secondChild->type == N_EXTDECLIST) {
		handleExtDecList(copySymbolTableType(type), secondChild);
	}
	else { // Specifier FunDec CompSt
		if (secondChild->nextSibling->type == N_COMPST) { // function definition
			handleFunDec(type, secondChild, symbolTable, true);
			semanticAnalysis(secondChild->nextSibling, symbolTable);
		}
		else { // function declaration
			handleFunDec(type, secondChild, symbolTable, false);
		}
	}
	freeSymbolTableType(type);
}

int numOfChild(SyntaxTreeNode *node) {
	SyntaxTreeNode* child = node->firstChild;
	int count = 0;
	while (child != NULL) {
		++count;
		child = child->nextSibling;
	}
	return count;
}

SymbolTableType* handleExp(SyntaxTreeNode* expNode, SymbolTable symbolTable);

bool handleArgs(SyntaxTreeNode* argsNode, FuncParam* param, SymbolTable symbolTable) {
	if (argsNode == NULL && param == NULL) {
		return true;
	}
	else if (argsNode == NULL || param == NULL) {
		return false;
	}
	SymbolTableType* currParamType = param->paramType;
	SymbolTableType* currArgType = handleExp(argsNode->firstChild, symbolTable);
	if (currArgType == NULL) {
		// TODO: whether to return false
		return true;
	}
	if (!compareSymbolTableType(currParamType, currArgType)) {
		freeSymbolTableType(currArgType);
		return false;
	}
	else {
		freeSymbolTableType(currArgType);
		if (argsNode->firstChild->nextSibling == NULL) {
			return handleArgs(NULL, param->nextParam, symbolTable);
		}
		else {
			return handleArgs(argsNode->firstChild->nextSibling->nextSibling, param->nextParam, symbolTable);
		}
	}
}

SymbolTableType* handleExp(SyntaxTreeNode* expNode, SymbolTable symbolTable) {
	int childNum = numOfChild(expNode);
	if (childNum == 1) {
		SyntaxTreeNode* child = expNode->firstChild;
		if (child->type == N_ID) {
			TrieNode* idNode = child->attr.id;
			if (idNode->type == NULL) { // undefined identifier
				char* idStr = retrieveStr(idNode);
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
		// TODO: check type
		return handleExp(expNode->firstChild->nextSibling, symbolTable);
	}
	else if (childNum == 3) {
		SyntaxTreeNode* firstChild = expNode->firstChild;
		if (firstChild->type == N_ID) { // ID LP RP
			TrieNode* funcNode = firstChild->attr.id;
			if (funcNode->type == NULL) { // undefined function
				char* funcName = retrieveStr(funcNode);
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", firstChild->lineno, funcName);
				free(funcName);
				return NULL;
			}
			else if (((SymbolTableType*)(funcNode->type))->typeType != S_FUNCTION) {
				char* idName = retrieveStr(funcNode);
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", firstChild->lineno, idName);
				free(idName);
				return NULL;
			}
			SymbolTableType* funcType = funcNode->type;
			return copySymbolTableType(funcType->type.funcType.returnType);
		}
		else if (firstChild->type == N_LP) {
			return handleExp(firstChild->nextSibling, symbolTable);
		}
		else if (firstChild->nextSibling->type == N_DOT) { // Exp DOT ID
			SymbolTableType *type = handleExp(firstChild, symbolTable);
			if (type == NULL) {
				return NULL;
			}
			else if (type->typeType != S_STRUCT) {
				SyntaxTreeNode* fieldNode = firstChild->nextSibling->nextSibling;
				printf("Error type 13 at Line %d: Illegal use of '.'.\n", fieldNode->lineno);
				freeSymbolTableType(type);
				return NULL;
			}
			else {
				SyntaxTreeNode* fieldNode = firstChild->nextSibling->nextSibling;
				char* fieldName = retrieveStr(fieldNode->attr.id);
				SymbolTableType* structDef = querySymbol(symbolTable, type->type.structName);
				SymbolTableType* fieldType = queryField(structDef->type.structType.firstField, fieldName);
				if (fieldType == NULL) {
					printf("Error type 14 at Line %d: Undefined field \"%s\".\n", fieldNode->lineno, fieldName);
				}
				free(fieldName);
				return fieldType;
			}
		}
		// TODO: more productions
	}
	else { // childNum == 4
		SyntaxTreeNode* firstChild = expNode->firstChild;
		if (firstChild->type == N_ID) { // ID LP ARGS RP
			TrieNode* idNode = firstChild->attr.id;
			if (idNode->type == NULL) { // undefined function
				char* idStr = retrieveStr(idNode);
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n", firstChild->lineno, idStr);
				free(idStr);
				return NULL;
			}
			else if (((SymbolTableType*)(idNode->type))->typeType != S_FUNCTION) {
				char* idName = retrieveStr(idNode);
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", firstChild->lineno, idName);
				free(idName);
				return NULL;
			}
			SymbolTableType* funcType = idNode->type;
			if (handleArgs(firstChild->nextSibling->nextSibling, funcType->type.funcType.firstParam, symbolTable)) {
				return copySymbolTableType(funcType->type.funcType.returnType);
			}
			else {
				printf("Error type 9 at Line %d: Function paramaters mismatch.\n", firstChild->nextSibling->nextSibling->lineno);
				return NULL;
			}
		}
		else { // Exp LB Exp RB
			SymbolTableType* expType = handleExp(firstChild, symbolTable);
			if (expType == NULL) return NULL;
			else if (expType->typeType != S_ARRAY) {
				char* idStr = retrieveStr(firstChild->firstChild->attr.id);
				printf("Error type 10 at Line %d: \"%s\" is not an array.\n", firstChild->lineno, idStr);
				free(idStr);
				return NULL;
			}
			SymbolTableType* indexType = handleExp(firstChild->nextSibling->nextSibling, symbolTable);
			if (indexType == NULL) {
				freeSymbolTableType(expType);
				return NULL;
			}
			else if (indexType->typeType != S_BASIC || indexType->type.basicType != T_INT) {
				printf("Error type 10 at Line %d: Index is not integer.\n", firstChild->nextSibling->nextSibling->lineno);
				freeSymbolTableType(expType);
				return NULL;
			}
			return copySymbolTableType(expType->type.arrayType.elementType);
		}
	}
	return NULL;
}

void handleStmt(SyntaxTreeNode* stmtNode, SymbolTable symbolTable) {
	if (stmtNode->firstChild->type == N_EXP) {
		SymbolTableType* type = handleExp(stmtNode->firstChild, symbolTable);
		if (type != NULL) freeSymbolTableType(type);
	}
	else if (stmtNode->firstChild->type == N_COMPST) {
		semanticAnalysis(stmtNode->firstChild, symbolTable);
	}
	else if (stmtNode->firstChild->type == N_RETURN) {
		// TODO
	}
	else if (stmtNode->firstChild->type == N_IF) {
		// TODO
	}
	else { // WHILE LOOP
		// TODO
	}
}

void semanticAnalysis(SyntaxTreeNode* syntaxTreeNode, SymbolTable symbolTable) {
	if (syntaxTreeNode->type == N_EXTDEF) { // external definition
		handleExtDef(syntaxTreeNode, symbolTable);
		return;
	}
	else if (syntaxTreeNode->type == N_DEF) { // local definition
		handleDef(syntaxTreeNode, symbolTable, false, NULL);
		return;
	}
	else if (syntaxTreeNode->type == N_STMT) {
		handleStmt(syntaxTreeNode, symbolTable);
		return;
	}
	// TODO: other node types

	SyntaxTreeNode* child = syntaxTreeNode->firstChild;
	while (child != NULL) {
		semanticAnalysis(child, symbolTable);
		child = child->nextSibling;
	}
}

