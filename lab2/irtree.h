#ifndef __IRTREE_H__
#define __IRTREE_H__

typedef enum IRNodeType {
	I_ROOT, I_LABEL, I_FUNC, I_ASSIGN,
	I_VAR, I_TEMP, I_CONST,
	I_ADD, I_MINUS, I_STAR, I_DIV,
	I_GETADDR, I_ACCSADDR,
	I_GOTO, I_IF, I_RETURN,
	I_DEC, I_ARG, I_CALL, I_PARAM,
	I_READ, I_WRITE
} IRNodeType;

typedef union IRNodeAttr {
	char* strValue;
	int intValue;
} IRNodeAttr;

typedef struct IRTreeNode {
	IRNodeType type;
	IRNodeAttr* attr;
	struct IRTreeNode* firstChild;
	struct IRTreeNode* nextSibling;
} IRTreeNode;

#endif

