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

		SymbolTableType* insertType = initSymbolTableType();
		insertType->typeType = S_STRUCTDEF;
		insertType->type.structType.firstField = handleStructDefList(tagNode->nextSibling->nextSibling, symbolTable);
		TrieNode* structDefNode = insertSymbol(symbolTable, structName, insertType);

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

void handleVarDec(SymbolTableType* type, SyntaxTreeNode* varDecNode, bool isStruct, StructField* currField, bool isParam, FuncParam* currParam) {
	SyntaxTreeNode* child = varDecNode->firstChild;
	if (child->type == N_ID) {
		child->attr.id->type = type;
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
		handleVarDec(array, child, isStruct, currField, isParam, currParam);
	}
}

void handleDec(SymbolTableType* type, SyntaxTreeNode* decNode, bool isStruct, StructField* currField) {
	SyntaxTreeNode* varDecNode = decNode->firstChild;
	handleVarDec(type, varDecNode, isStruct, currField, false, NULL);
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
	handleVarDec(type, varDecNode, false, NULL, false, NULL);
	if (varDecNode->nextSibling != NULL) {
		handleExtDecList(copySymbolTableType(type), varDecNode->nextSibling->nextSibling);
	}
}

FuncParam* handleParamDec(SyntaxTreeNode* paramDecNode, SymbolTable symbolTable) {
	SymbolTableType* type = handleSpecifier(paramDecNode->firstChild, symbolTable);
	FuncParam* currParam = (FuncParam*)malloc(sizeof(FuncParam));
	currParam->nextParam = NULL;
	handleVarDec(type, paramDecNode->firstChild->nextSibling, false, NULL, true, currParam);
	return currParam;
}

FuncParam* handleVarList(SyntaxTreeNode* varListNode, SymbolTable symbolTable) {
	FuncParam* currParam = handleParamDec(varListNode->firstChild, symbolTable);
	if (varListNode->firstChild->nextSibling != NULL) {
		currParam->nextParam = handleVarList(varListNode->firstChild->nextSibling->nextSibling, symbolTable);
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
		funcType->type.funcType.firstParam = handleVarList(nameNode->nextSibling->nextSibling, symbolTable);
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
			// TODO: what to return
		}
		else if (child->type == N_FLOAT) {
			// TODO: what to return
		}
	}
	else if (childNum == 2) {
		// TODO
	}
	else if (childNum == 3) {
		// TODO
	}
	else { // childNum == 4
		// TODO
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

