#include <stdlib.h>
#include <stdio.h>

#include "irtree.h"

IRTreeNode* createIRTreeNode() {
	IRTreeNode* newNode = (IRTreeNode*)malloc(sizeof(IRTreeNode));
	newNode->attr = NULL;
	newNode->firstChild = NULL;
	newNode->nextSibling = NULL;
	return newNode;
}

IRTreeNode* initIRTree() {
	IRTreeNode* rootNode = createIRTreeNode();
	rootNode->type = I_ROOT;
	return rootNode;
}

