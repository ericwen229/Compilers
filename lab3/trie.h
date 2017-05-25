#ifndef __TRIE_H__
#define __TRIE_H__

#include "def.h"

#define BRANCH_NUM 63

typedef struct TrieNode {
	struct TrieNode* parent;
	int parentIndex;
	struct TrieNode* child[BRANCH_NUM];
	bool isEnd;
	void* type;
} TrieNode;

typedef void (*TrieOp)(TrieNode*);

TrieNode* initTrieNode(TrieNode* parent, int parentIndex);
void* queryTrie(TrieNode* node, char* str);
TrieNode* insertTrie(TrieNode* trie, char* str, void* item);
char* retrieveStr(TrieNode* node);
void traverseTrie(TrieNode* trie, TrieOp beforeOp, TrieOp afterOp);

#endif

