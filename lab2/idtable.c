#include <stdlib.h>

#include "idtable.h"

IdTable* createIdTable(int capacity) {
	IdTable* newTable = (IdTable*)malloc(sizeof(IdTable));
	newTable->size = 0;
	newTable->capacity = capacity;
	newTable->cells = (IdTableCell**)malloc(capacity * sizeof(IdTableCell*));
}

int insertId(IdTable* table, char* name) {
	int i = 0;
	table->cells[table->size] = (IdTableCell*)malloc(sizeof(IdTableCell));
	table->cells[table->size]->name = name;
	++ table->size;
	if (table->size == table->capacity) {
		IdTableCell** newCellSpace = (IdTableCell**)malloc(table->capacity * 2 * sizeof(IdTableCell*));
		for (i = 0; i < table->capacity; ++ i) {
			newCellSpace[i] = table->cells[i];
		}
		free(table->cells);
		table->cells = newCellSpace;
		table->capacity *= 2;
	}
	return table->size - 1;
}

IdTableCell* getCell(IdTable* table, int index) {
	return table->cells[index];
}

void freeTable(IdTable* table) {
	free(table->cells);
	free(table);
}

