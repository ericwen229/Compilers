
%option yylineno

%{

	#include <stdio.h>
	#include <string.h>

	#include "def.h"
	#include "syntax.tab.h"
	#include "syntaxtree.h"
	#include "symboltable.h"

	extern bool gError;
	extern SymbolTable gSymbolTable;

	static Attribute attr;

	int parseOct(const char* str) {
		int value;
		sscanf(str, "%o", &value);
		return value;
	}

	int parseDec(const char* str) {
		int value;
		sscanf(str, "%d", &value);
		return value;
	}

	int parseHex(const char* str) {
		int value;
		sscanf(str, "%x", &value);
		return value;
	}

	float parseFloat(const char *str) {
		float value;
		sscanf(str, "%f", &value);
		return value;
	}

%}

SEMI		;
COMMA		,

ASSIGNOP	=
RELOP		>|<|>=|<=|==|!=
PLUS		\+
MINUS		-
STAR		\*
DIV		\/
AND		&&
OR		\|\|
NOT		!
DOT		\.

TYPE		int|float

LP		\(
RP		\)
LB		\[
RB		\]
LC		\{
RC		\}

STRUCT		struct
RETURN		return
IF		if
ELSE		else
WHILE		while

WHITESPACE	[ \t\n]+

OCTINT		0[0-7]+
DECINT		0|([1-9][0-9]*)
HEXINT		0[xX][0-9a-fA-F]+

FLOAT		([0-9]+\.[0-9]*)|(\.[0-9]+)

ID		[_a-zA-Z][_a-zA-Z0-9]*

ILLEGALOCT	0[^xX][0-9a-zA-Z]+
ILLEGALHEX	0[xX][0-9a-zA-Z]+

UNKNOWN		.

%%

{SEMI}		{
	log("SEMI");
	yylval = node_a(SEMI);
	return SEMI; }
{COMMA}		{
	log("COMMA");
	yylval = node_a(COMMA);
	return COMMA; }

{ASSIGNOP}	{
	log("ASSIGNOP");
	yylval = node_a(ASSIGNOP);
	return ASSIGNOP; }
{RELOP}		{
	log_a("RELOP");
	if (yytext[0] == '>') {
		if (strlen(yytext) == 2) { // >=
			attr.relopType = GE;
		}
		else { // >
			attr.relopType == LT;
		}
	}
	else if (yytext[0] == '<') {
		if (strlen(yytext) == 2) { // <=
			attr.relopType = LE;
		}
		else { // <
			attr.relopType = LT;
		}
	}
	else if (yytext[0] == '=') { // ==
		attr.relopType = EQ;
	}
	else { // !=
		attr.relopType = NE;
	}
	yylval = node_a(RELOP);
	return RELOP; }
{PLUS}		{
	log("PLUS");
	yylval = node_a(PLUS);
	return PLUS; }
{MINUS}		{
	log("MINUS");
	yylval = node_a(MINUS);
	return MINUS; }
{STAR}		{
	log("STAR");
	yylval = node_a(STAR);
	return STAR; }
{DIV}		{
	log("DIV");
	yylval = node_a(DIV);
	return DIV; }
{AND}		{
	log("AND");
	yylval = node_a(AND);
	return AND; }
{OR}		{
	log("OR");
	yylval = node_a(OR);
	return OR; }
{NOT}		{
	log("NOT");
	yylval = node_a(NOT);
	return NOT; }
{DOT}		{
	log("DOT");
	yylval = node_a(DOT);
	return DOT; }

{TYPE}		{
	log_a("TYPE");
	if (yytext[0] == 'i') {
		attr.typeType = T_INT;
	}
	else {
		attr.typeType = T_FLOAT;	
	}
	yylval = node_a(TYPE);
	return TYPE; }

{LP}		{
	log("LP");
	yylval = node_a(LP);
	return LP; }
{RP}		{
	log("RP");
	yylval = node_a(RP);
	return RP; }
{LB}		{
	log("LB");
	yylval = node_a(LB);
	return LB; }
{RB}		{
	log("RB");
	yylval = node_a(RB);
	return RB; }
{LC}		{
	log("LC");
	yylval = node_a(LC);
	return LC; }
{RC}		{
	log("RC");
	yylval = node_a(RC);
	return RC; }

{STRUCT}	{
	log("STRUCT");
	yylval = node_a(STRUCT);
	return STRUCT; }
{RETURN}	{
	log("RETURN");
	yylval = node_a(RETURN);
	return RETURN; }
{IF}		{
	log("IF");
	yylval = node_a(IF);
	return IF; }
{ELSE}		{
	log("ELSE");
	yylval = node_a(ELSE);
	return ELSE; }
{WHILE}		{
	log("WHILE");
	yylval = node_a(WHILE);
	return WHILE; }

{WHITESPACE}	{ /* log("WHITESPACE"); */ }

{OCTINT}	{
	log_a("OCTINT");
	attr.intValue = parseOct(yytext);
	yylval = node_a(INT);
	return INT; }
{DECINT}	{
	log_a("DECINT");
	attr.intValue = parseDec(yytext);
	yylval = node_a(INT);
	return INT; }
{HEXINT}	{
	log_a("HEXINT");
	attr.intValue = parseHex(yytext);
	yylval = node_a(INT);
	return INT; }

{FLOAT}		{
	log_a("FLOAT");
	attr.floatValue = parseFloat(yytext);
	yylval = node_a(FLOAT);
	return FLOAT; }

{ID}		{
	log_a("ID");
	attr.id = insertSymbol(gSymbolTable, yytext, NULL);
	yylval = node_a(ID);
	return ID; }

{ILLEGALOCT}	{
	printf("Error type A at Line %d: Illegal octal number '%s'\n", yylineno, yytext);
	gError = true;
	yylval = node_a(INT);
	return INT; }

{ILLEGALHEX}	{
	printf("Error type A at Line %d: Illegal hexadecimal number '%s'\n", yylineno, yytext);
	gError = true;
	yylval = node_a(INT);
	return INT; }

{UNKNOWN}	{
	printf("Error type A at Line %d: Undefined character '%s'\n", yylineno, yytext);
	gError = true; }

%%

