#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/* ── xam AST ── */

typedef enum HeaderPropertyType HeaderPropertyType;

enum HeaderPropertyType {
	HEADER_PROP_TITLE,
	HEADER_PROP_SUBJECT,
	HEADER_PROP_PROFESSOR,
	HEADER_PROP_DATE,
	HEADER_PROP_DURATION,
	HEADER_PROP_SCORE_GRID,
	HEADER_PROP_ANSWER_SHEET,
	HEADER_PROP_STUDENT_NAME,
	HEADER_PROP_STUDENT_ID,
	HEADER_PROP_COURSE,
	HEADER_PROP_INSTRUCTIONS
};

typedef struct HeaderProperty HeaderProperty;

struct HeaderProperty {
	HeaderPropertyType type;
	char * stringValue;
	int intValue;
};

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
	char * professor;
	char * date;
	int duration;
	int score_grid;
	int answer_sheet;
	int student_name;
	int student_id;
	int course;
	StringList * instructions;
};

/* ── Exercises ── */

typedef struct MultiplechoiceOption MultiplechoiceOption;
typedef struct MultiplechoiceNode MultiplechoiceNode;
typedef struct FillblanksNode FillblanksNode;
typedef struct OpenquestionNode OpenquestionNode;
typedef struct TrueorfalseNode TrueorfalseNode;
typedef struct MatchPair MatchPair;
typedef struct MatchNode MatchNode;
typedef struct ChartCell ChartCell;
typedef struct ChartNode ChartNode;
typedef struct ChoosefromNode ChoosefromNode;
typedef struct SetNode SetNode;

struct MultiplechoiceOption {
	char * value;
	int is_correct;
	MultiplechoiceOption * next;
};

struct MultiplechoiceNode {
	char * question;
	MultiplechoiceOption * options;
	int score;
};

struct FillblanksNode {
	char * question;
	StringList * answers;
	int score;
};

struct OpenquestionNode {
	char * question;
	int lines;
	char * answer;
	int score;
};

struct TrueorfalseNode {
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

struct ChoosefromNode {
	char * task;
	StringList * options;
	char * text;
	IntList * answer;
	int score;
	int shuffle;
};

typedef enum ExerciseType ExerciseType;
typedef struct Exercise Exercise;
typedef struct SetNode SetNode;
typedef struct SectionNode SectionNode;

enum ExerciseType {
	EXERCISE_MULTIPLECHOICE,
	EXERCISE_FILLBLANKS,
	EXERCISE_OPENQUESTION,
	EXERCISE_CHOOSEFROM,
	EXERCISE_MATCH,
	EXERCISE_TRUEORFALSE,
	EXERCISE_CHART,
	EXERCISE_SET
};

struct Exercise {
	ExerciseType type;
	union {
		MultiplechoiceNode * multiplechoice;
		FillblanksNode * fillblanks;
		OpenquestionNode * openquestion;
		ChoosefromNode * choosefrom;
		MatchNode * match;
		TrueorfalseNode * trueorfalse;
		ChartNode * chart;
		SetNode * set;
	};
	Exercise * next;
};

struct SetNode {
	char * text;
	int shuffle;
	int score;
	Exercise * exercises;
};

typedef struct ImageNode ImageNode;

struct ImageNode {
	char * path;
	char * caption;
	int width;
};

typedef enum ItemType ItemType;

enum ItemType {
	ITEM_EXERCISE,
	ITEM_SECTION,
	ITEM_TEXT,
	ITEM_IMAGE
};

struct Item {
	ItemType type;
	union {
		Exercise * exercise;
		SectionNode * section;
		char * text;
		ImageNode * image;
	};
	Item * next;
};

struct SectionNode {
	char * name;
	int select;
	Item * items;
};

struct Program {
	Header * header;
	Item * items;
};

void destroyStringList(StringList * list);
void destroyIntList(IntList * list);
void destroyHeader(Header * header);
void destroyMultiplechoiceOption(MultiplechoiceOption * option);
void destroyMultiplechoiceNode(MultiplechoiceNode * node);
void destroyFillblanksNode(FillblanksNode * node);
void destroyOpenquestionNode(OpenquestionNode * node);
void destroyTrueorfalseNode(TrueorfalseNode * node);
void destroyMatchPair(MatchPair * pair);
void destroyMatchNode(MatchNode * node);
void destroyChartCell(ChartCell * cell);
void destroyChartNode(ChartNode * node);
void destroyChoosefromNode(ChoosefromNode * node);
void destroyExercise(Exercise * exercise);
void destroySetNode(SetNode * node);
void destroyImageNode(ImageNode * node);
void destroyItem(Item * item);
void destroySectionNode(SectionNode * node);
void destroyProgram(Program * program);

#endif
