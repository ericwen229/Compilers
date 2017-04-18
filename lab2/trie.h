#ifndef __TRIE_H__
#define __TRIE_H__

#include "def.h"

#define BRANCH_NUM 63

typedef struct TrieNode {
	struct TrieNode* child[BRANCH_NUM];
	bool isEnd;
	void* item;
} TrieNode;

TrieNode* initTrieNode();
void* queryTrie(TrieNode* node, char* str);
void insertTrie(TrieNode* trie, char* str, void* item);
void printTrie(TrieNode* trie);

#endif

