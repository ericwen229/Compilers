#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "def.h"

typedef enum SymbolTableItemType {
	BASIC, ARRAY, STRUCTDEF, STRUCT, FUNCTION
} SymbolTableItemType;

typedef enum BasicType {
	INT, FLOAT
} BasicType;

typedef union SymbolType {
	BasicType basicType;
} SymbolType;

typedef struct SymbolTableItem {
	SymbolTableItemType itemType;
	SymbolType type;
	char* name;
} SymbolTableItem;

#endif

