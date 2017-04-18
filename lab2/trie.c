#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "trie.h"

TrieNode* initTrieNode() {
	TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
	int i = 0;
	for (i = 0; i < BRANCH_NUM; ++ i) {
		node->child[i] = NULL;
	}
	node->isEnd = false;
	node->item = NULL;
}

int ch2Index(char ch) {
	if (ch == '_') {
		return 0;
	}
	else if (ch >= 'a' && ch <= 'z') {
		return 1 + (ch - 'a');
	}
	else if (ch >= 'A' && ch <= 'Z') {
		return 27 + (ch - 'A');
	}
	else if (ch >= '0' && ch <= '9') {
		return 53 + (ch - '0');
	}
	else {
		return -1;
	}
}

char index2Ch(int index) {
	if (index == 0) {
		return '_';
	}
	else if (index >= 1 && index < 27) {
		return (char)('a' + (index - 1));
	}
	else if (index >= 27 && index < 53) {
		return (char)('A' + (index - 27));
	}
	else if (index >= 53 && index < BRANCH_NUM) {
		return (char)('0' + (index - 53));
	}
	else {
		return '#';
	}
}

void* queryTrie(TrieNode* node, char* str) {
	int i = 0;
	TrieNode* currNode = node;
	for (i = 0; i < strlen(str); ++ i) {
		int nextChildIndex = ch2Index(str[i]);
		if (currNode->child[nextChildIndex] == NULL) {
			return NULL;
		}
		currNode = currNode->child[nextChildIndex];
	}
	if (currNode->isEnd) {
		return currNode->item;
	}
	else {
		return NULL;
	}
}

void insertTrie(TrieNode* node, char* str, void* item) {
	int i = 0;
	TrieNode* currNode = node;
	for (i = 0; i < strlen(str); ++ i) {
		int nextChildIndex = ch2Index(str[i]);
		if (currNode->child[nextChildIndex] == NULL) {
			currNode->child[nextChildIndex] = initTrieNode();
		}
		currNode = currNode->child[nextChildIndex];
	}
	currNode->isEnd = true;
	currNode->item = item;
}

void _printTrie(TrieNode* trie, int level) {
	int i = 0;
	for (i = 0; i < BRANCH_NUM; ++ i) {
		if (trie->child[i] != NULL) {
			int j = 0;
			for (j = 0; j < 2 * level; ++ j) {
				putchar(' ');
			}
			putchar(index2Ch(i));
			if (trie->child[i]->isEnd) {
				putchar('*');
			}
			putchar('\n');
			_printTrie(trie->child[i], level + 1);
		}
	}
}

void printTrie(TrieNode* trie) {
	_printTrie(trie, 0);
}

