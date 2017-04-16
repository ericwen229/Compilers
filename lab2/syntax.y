
%{

	#include <stdio.h>
	#include <string.h>

	#include "def.h"
	#include "syntaxtree.h"

	typedef SyntaxTreeNode* pNode;

	#define YYSTYPE pNode

	#include "lex.yy.c"

	extern bool gError;
	extern SyntaxTreeNode* gTree;
	
	static int lastErrLineNo = -1;

	int yyerror(char* msg) {
		if (yylineno == lastErrLineNo) {
			return 0;
		}
		if (strlen(yytext) == 0) {
			printf("Error type B at Line %d: Unexpected end of file\n", yylineno - 1);
		}
		else {
			printf("Error type B at Line %d: Unexpected token \"%s\"\n", yylineno, yytext);
		}
		lastErrLineNo = yylineno;
		gError = true;
		return 0;
	}
	
%}

%token SEMI COMMA
%token ASSIGNOP
%token RELOP
%token PLUS MINUS STAR DIV
%token AND OR NOT
%token DOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE
%token INT
%token FLOAT
%token ID

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT

%destructor { if ($$ != NULL) freeTree($$); } <>

%%

/* High-level Definitions */
Program : ExtDefList {
	$$ = node(PROGRAM);
	appendChild($$, $1);
	gTree = $$;
	$$ = NULL;
}
	;
ExtDefList : {
	$$ = node(EXTDEFLIST);
}
	| ExtDef ExtDefList {
	$$ = node(EXTDEFLIST);
	appendChild($$, $1);
	appendChild($$, $2);
}
	;
ExtDef : Specifier ExtDecList SEMI {
	$$ = node(EXTDEF);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Specifier SEMI {
	$$ = node(EXTDEF);
	appendChild($$, $1);
	appendChild($$, $2);
}
	| Specifier FunDec CompSt {
	$$ = node(EXTDEF);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Specifier FunDec SEMI {
	$$ = node(EXTDEF);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Specifier FunDec error DefList StmtList RC {
	yyerrok;
	$$ = node(EXTDEF);
	freeTree($1);
	freeTree($2);
	freeTree($4);
	freeTree($5);
	freeTree($6);
}
	| error CompSt {
	yyerrok;
	$$ = node(EXTDEF);
	freeTree($2);
}
	| error SEMI {
	yyerrok;
	$$ = node(EXTDEF);
	freeTree($2);
}
	| error RC {
	yyerrok;
	$$ = node(EXTDEF);
	freeTree($2);
}
	;
ExtDecList : VarDec {
	$$ = node(EXTDECLIST);
	appendChild($$, $1);
}
	| VarDec COMMA ExtDecList {
	$$ = node(EXTDECLIST);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	;

/* Specifiers */
Specifier : TYPE {
	$$ = node(SPECIFIER);
	appendChild($$, $1);
}
	| StructSpecifier {
	$$ = node(SPECIFIER);
	appendChild($$, $1);
}
	;
StructSpecifier : STRUCT OptTag LC StructDefList RC {
	$$ = node(STRUCTSPECIFIER);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
	appendChild($$, $5);
}
	| STRUCT Tag {
	$$ = node(STRUCTSPECIFIER);
	appendChild($$, $1);
	appendChild($$, $2);
}
	;
OptTag : {
	$$ = node(OPTTAG);
}
	| ID {
	$$ = node(OPTTAG);
	appendChild($$, $1);
}
	;
Tag : ID {
	$$ = node(TAG);
	appendChild($$, $1);	
}
	;

/* Declarators */
VarDec : ID {
	$$ = node(VARDEC);
	appendChild($$, $1);
}
	| VarDec LB INT RB {
	$$ = node(VARDEC);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
}
	;
FunDec : ID LP VarList RP {
	$$ = node(FUNDEC);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
}
	| ID LP RP {
	$$ = node(FUNDEC);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	;
VarList : ParamDec COMMA VarList {
	$$ = node(VARLIST);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| ParamDec {
	$$ = node(VARLIST);
	appendChild($$, $1);
}
	;
ParamDec : Specifier VarDec {
	$$ = node(PARAMDEC);
	appendChild($$, $1);
	appendChild($$, $2);
}
	;

/* Statements */
CompSt : LC DefList StmtList RC {
	$$ = node(COMPST);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
}
	;
StmtList : {
	$$ = node(STMTLIST);
}
	| Stmt StmtList {
	$$ = node(STMTLIST);
	appendChild($$, $1);
	appendChild($$, $2);
}
	;
Stmt : Exp SEMI {
	$$ = node(STMT);
	appendChild($$, $1);
	appendChild($$, $2);
}
	| CompSt {
	$$ = node(STMT);
	appendChild($$, $1);
}
	| RETURN Exp SEMI {
	$$ = node(STMT);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
	$$ = node(STMT);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
	appendChild($$, $5);
}
	| IF LP Exp RP Stmt ELSE Stmt {
	$$ = node(STMT);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
	appendChild($$, $5);
	appendChild($$, $6);
	appendChild($$, $7);
}
	| WHILE LP Exp RP Stmt {
	$$ = node(STMT);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
	appendChild($$, $5);
}
	| error Stmt {
	yyerrok;
	$$ = node(STMT);
	freeTree($2);
}
	| error SEMI {
	yyerrok;
	$$ = node(STMT);
	freeTree($2);
}
	| error RC {
	yyerrok;
	$$ = node(STMT);
	YYBACKUP(RC, $2);
}
	;

/* Local Definitions */
DefList : {
	$$ = node(DEFLIST);
}
	| Def DefList {
	$$ = node(DEFLIST);
	appendChild($$, $1);
	appendChild($$, $2);
}
	;
Def : Specifier DecList SEMI {
	$$ = node(DEF);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Specifier error Def {
	yyerrok;
	$$ = node(DEF);
	freeTree($1);
	freeTree($3);
}
	| Specifier error Stmt {
	yyerrok;
	$$ = node(DEF);
	freeTree($1);
	freeTree($3);
}
	| Specifier error SEMI {
	yyerrok;
	$$ = node(DEF);
	freeTree($1);
	freeTree($3);
}
	| Specifier error RC {
	yyerrok;
	$$ = node(DEF);
	freeTree($1);
	YYBACKUP(RC, $3);
}
DecList : Dec {
	$$ = node(DECLIST);
	appendChild($$, $1);
}
	| Dec COMMA DecList {
	$$ = node(DECLIST);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	;
Dec : VarDec {
	$$ = node(DEC);
	appendChild($$, $1);	
}
	| VarDec ASSIGNOP Exp {
	$$ = node(DEC);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	;

StructDefList : {
	$$ = node(DEFLIST);
}
	| StructDef StructDefList {
	$$ = node(DEFLIST);
	appendChild($$, $1);
	appendChild($$, $2);
}
	;
StructDef : Specifier DecList SEMI {
	$$ = node(DEF);		  
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| error SEMI {
	yyerrok;
	$$ = node(DEF);
	freeTree($2);
}
	| error RC {
	yyerrok;
	$$ = node(DEF);
	YYBACKUP(RC, $2);
}
	;

/* Expressions */
Exp : Exp ASSIGNOP Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp AND Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp OR Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp RELOP Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);

}
	| Exp PLUS Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp MINUS Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp STAR Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp DIV Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| LP Exp RP {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| MINUS Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
}
	| NOT Exp {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
}
	| ID LP Args RP {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
}
	| ID LP RP {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| Exp LB Exp RB {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
	appendChild($$, $4);
}
	| Exp DOT ID {
	$$ = node(EXP);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);
}
	| ID {
	$$ = node(EXP);
	appendChild($$, $1);
}
	| INT {
	$$ = node(EXP);
	appendChild($$, $1);
}
	| FLOAT {
	$$ = node(EXP);
	appendChild($$, $1);
}
	;
Args : Exp COMMA Args {
	$$ = node(ARGS);
	appendChild($$, $1);
	appendChild($$, $2);
	appendChild($$, $3);	 
}
	| Exp {
	$$ = node(ARGS);
	appendChild($$, $1);
}
	;

%%

