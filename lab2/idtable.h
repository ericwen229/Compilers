#ifndef __IDTABLE_H__
#define __IDTABLE_H__

typedef struct IdTableCell {
	char* name;
} IdTableCell;

typedef struct IdTable {
	int size;
	int capacity;
	IdTableCell** cells;
} IdTable;

IdTable* createIdTable(int capacity);
int insertId(IdTable* table, char* name);
IdTableCell* getCell(IdTable* table, int index);
void freeTable(IdTable* table);

#endif

