#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/* ── Calculator types (to be removed) ── */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

void destroyConstant(Constant * constant);
void destroyExpression(Expression * expression);
void destroyFactor(Factor * factor);

/* ── xam AST ── */

typedef struct StringList StringList;
typedef struct IntList IntList;
typedef struct Header Header;
typedef struct Item Item;
typedef struct Program Program;

struct StringList {
	char * value;
	StringList * next;
};

struct IntList {
	int value;
	IntList * next;
};

struct Header {
	char * title;
	char * subject;
	char * date;
	int duration;
	int score_grid;
	int answer_sheet;
	int student_name;
	int student_id;
	int course;
	char * instructions;
};

struct Program {
	Header * header;
	Item * items;
};

void destroyStringList(StringList * list);
void destroyIntList(IntList * list);
void destroyHeader(Header * header);
void destroyProgram(Program * program);

#endif
