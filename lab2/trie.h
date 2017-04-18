#ifndef __TRIE_H__
#define __TRIE_H__

#include "def.h"

#define BRANCH_NUM 63

typedef struct TrieNode {
	struct TrieNode* child[BRANCH_NUM];
	bool isEnd;
	void* type;
} TrieNode;

TrieNode* initTrieNode();
void* queryTrie(TrieNode* node, char* str);
TrieNode* insertTrie(TrieNode* trie, char* str, void* item);
void printTrie(TrieNode* trie);

#endif

