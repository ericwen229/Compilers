#ifndef __DEF_H__
#define __DEF_H__

#define NDEBUG

#define log_a(type)
#define log(type)

#ifndef NDEBUG
	
#undef log_a
#undef log

#define log_a(type) printf(type " %s\n", yytext) /* log with attribute */
#define log(type) printf(type "\n") /* log without attribute */

#endif

#define node_a(TYPE) createTerminalNode(N_ ## TYPE, attr, yylineno)
#define node(TYPE) createNode(N_ ## TYPE)

typedef int bool;

#define true 1
#define false 0

#endif

