#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "trie.h"
#include "symboltable.h"

TrieNode* initTrieNode(TrieNode* parent, int parentIndex) {
	TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
	node->parent = parent;
	node->parentIndex = parentIndex;
	int i = 0;
	for (i = 0; i < BRANCH_NUM; ++ i) {
		node->child[i] = NULL;
	}
	node->isEnd = false;
	node->type = NULL;
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
		return currNode->type;
	}
	else {
		return NULL;
	}
}

TrieNode* insertTrie(TrieNode* node, char* str, void* type) {
	int i = 0;
	TrieNode* currNode = node;
	for (i = 0; i < strlen(str); ++ i) {
		int nextChildIndex = ch2Index(str[i]);
		if (currNode->child[nextChildIndex] == NULL) {
			currNode->child[nextChildIndex] = initTrieNode(currNode, nextChildIndex);
		}
		currNode = currNode->child[nextChildIndex];
	}
	currNode->isEnd = true;
	currNode->type = type;
	return currNode;
}

int getDepth(TrieNode* node) {
	//if (node == NULL) {
	//	printf("ERROR");
	//	return 0;
	//}
	if (node->parent == NULL) {
		return 0;
	}
	else {
		return 1 + getDepth(node->parent);
	}
}

char* retrieveStr(TrieNode* node) {
	int length = getDepth(node);
	char* str = (char*)malloc((length + 1) * sizeof(char));
	str[length] = '\0';
	for (int i = length - 1; i >= 0; -- i) {
		str[i] = index2Ch(node->parentIndex);
		node = node->parent;
	}
	return str;
}

void traverseTrie(TrieNode* trie, TrieOp beforeOp, TrieOp afterOp) {
	if (trie == NULL) {
		return;
	}

	if (beforeOp != NULL) {
		(*beforeOp)(trie);
	}

	int i = 0;
	for (i = 0; i < BRANCH_NUM; ++ i) {
		traverseTrie(trie->child[i], beforeOp, afterOp);
	}

	if (afterOp != NULL) {
		(*afterOp)(trie);
	}

}

