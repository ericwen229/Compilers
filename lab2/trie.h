#ifndef __TRIE_H__
#define __TRIE_H__

#include "def.h"

#define BRANCH_NUM 63

typedef struct TrieNode {
	struct TrieNode* child[BRANCH_NUM];
	bool isEnd;
	void* item;
} TrieNode;

TrieNode* initNode();
void* query(TrieNode* node, char* str);
void insert(TrieNode* trie, char* str, void* item);
void print(TrieNode* trie);

#endif

