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

/* ── Exercises ── */

typedef struct McOption McOption;
typedef struct McNode McNode;
typedef struct BlanksNode BlanksNode;
typedef struct DirectNode DirectNode;
typedef struct TorfNode TorfNode;
typedef struct MatchPair MatchPair;
typedef struct MatchNode MatchNode;
typedef struct ChartCell ChartCell;
typedef struct ChartNode ChartNode;
typedef struct ChooseFromNode ChooseFromNode;

struct McOption {
	char * value;
	int is_correct;
	McOption * next;
};

struct McNode {
	char * question;
	McOption * options;
	int score;
};

struct BlanksNode {
	char * question;
	StringList * answers;
	int score;
};

struct DirectNode {
	char * question;
	int lines;
	char * answer;
	int score;
};

struct TorfNode {
	char * question;
	int justify;
	int answer;
	int score;
};

struct MatchPair {
	char * left;
	char * right;
	MatchPair * next;
};

struct MatchNode {
	char * question;
	MatchPair * pairs;
	int score;
};

struct ChartCell {
	int row;
	int col;
	char * value;
	ChartCell * next;
};

struct ChartNode {
	char * task;
	int dim_rows;
	int dim_cols;
	ChartCell * cells;
	StringList * answer;
	int score;
};

struct ChooseFromNode {
	char * task;
	StringList * options;
	char * text;
	IntList * answer;
	int score;
	int shuffle;
};

struct Program {
	Header * header;
	Item * items;
};

void destroyStringList(StringList * list);
void destroyIntList(IntList * list);
void destroyHeader(Header * header);
void destroyMcOption(McOption * option);
void destroyMcNode(McNode * node);
void destroyBlanksNode(BlanksNode * node);
void destroyDirectNode(DirectNode * node);
void destroyTorfNode(TorfNode * node);
void destroyMatchPair(MatchPair * pair);
void destroyMatchNode(MatchNode * node);
void destroyChartCell(ChartCell * cell);
void destroyChartNode(ChartNode * node);
void destroyChooseFromNode(ChooseFromNode * node);
void destroyProgram(Program * program);

#endif
