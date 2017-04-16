#include <stdlib.h>
#include <string.h>

#include "trie.h"

TrieNode* initNode() {
	TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
	int i = 0;
	for (i = 0; i < BRANCH_NUM; ++ i) {
		node->child[i] = NULL;
	}
	node->isEnd = false;
	node->item = NULL;
}

int childIndex(char ch) {
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

void* query(TrieNode* node, char* str) {
	int i = 0;
	TrieNode* currNode = node;
	for (i = 0; i < strlen(str); ++ i) {
		int nextChildIndex = childIndex(str[i]);
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

void insert(TrieNode* node, char* str, void* item) {
	int i = 0;
	TrieNode* currNode = node;
	for (i = 0; i < strlen(str); ++ i) {
		int nextChildIndex = childIndex(str[i]);
		if (currNode->child[nextChildIndex] == NULL) {
			currNode->child[nextChildIndex] = initNode();
		}
		currNode = currNode->child[nextChildIndex];
	}
	currNode->isEnd = true;
	currNode->item = item;
}

