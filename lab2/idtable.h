#ifndef __IDTABLE_H__
#define __IDTABLE_H__

typedef struct idTableCell {
	char* name;
} idTableCell;

typedef struct idTable {
	int size;
	int capacity;
	idTableCell** cells;
} idTable;

idTable* createIdTable(int capacity);
int insertId(idTable* table, char* name);
idTableCell* getCell(idTable* table, int index);
void freeTable(idTable* table);

#endif

