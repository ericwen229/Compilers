#include "codegen.h"

void createHeader(FILE* out) {
	char *header = ".data\n"
		"  _prompt: .asciiz \"please enter an integer: \"\n"
		"  _ret: .asciiz \"\\n\"\n"
		"  .globl main\n"
		"  .text\n";
	fprintf(out, "%s", header);
}

void createReadFunc(FILE* out) {
}

void createWriteFunc(FILE* out) {
}

void _translateIRCode(IRCode* code, FILE* out) {
}

void analyzeStackInfo(IRCode* code, TrieNode** funcStackSize, TrieNode** varStackPos) {
	TrieNode* stackSize = NULL;
	TrieNode* stackPos = NULL;

	IRCode* head = code;
	IRCode* curr = code;
	do {
		switch (curr->type) {
		}
		curr = curr->next;
	} while (curr != head);

	*funcStackSize = stackSize;
	*varStackPos = stackPos;
}

void translateIRCode(IRCode* code, FILE* out) {
	if (code == NULL) return;
	// TODO: analyze stack info
	TrieNode* funcStackSize = NULL;
	TrieNode* varStackPos = NULL;
	analyzeStackInfo(code, &funcStackSize, &varStackPos);
	createHeader(out);
	createReadFunc(out);
	createWriteFunc(out);
	IRCode* head = code;
	IRCode* curr = head;
	IRCode* next = curr->next;
	_translateIRCode(curr, out);
	while (next != head) {
		curr = next;
		next = curr->next;
		_translateIRCode(curr, out);
	}
}
