#include <stdlib.h>

#include "idtable.h"

idTable* createIdTable(int capacity) {
	idTable* newTable = (idTable*)malloc(sizeof(idTable));
	newTable->size = 0;
	newTable->capacity = capacity;
	newTable->cells = (idTableCell**)malloc(capacity * sizeof(idTableCell*));
}

int insertId(idTable* table, char* name) {
	int i = 0;
	table->cells[table->size] = (idTableCell*)malloc(sizeof(idTableCell));
	table->cells[table->size]->name = name;
	++ table->size;
	if (table->size == table->capacity) {
		idTableCell** newCellSpace = (idTableCell**)malloc(table->capacity * 2 * sizeof(idTableCell*));
		for (i = 0; i < table->capacity; ++ i) {
			newCellSpace[i] = table->cells[i];
		}
		free(table->cells);
		table->cells = newCellSpace;
		table->capacity *= 2;
	}
	return table->size - 1;
}

idTableCell* getCell(idTable* table, int index) {
	return table->cells[index];
}

void freeTable(idTable* table) {
	free(table->cells);
	free(table);
}

