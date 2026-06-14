#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

static void _duplicatePropertyError(const char * block, const char * property) {
	logError(_logger, "%s: duplicate '%s' property.", block, property);
	_compilerState->duplicateProperty = true;
}

/* PUBLIC FUNCTIONS */

/* ── program ── */

Program * ProgramSemanticAction(Item * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->header = NULL;
	program->items = items;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Program * ProgramWithHeaderSemanticAction(Header * header, Item * items) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->header = header;
	program->items = items;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

/* ── items ── */

Item * TextItemSemanticAction(char * text) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Item * item = calloc(1, sizeof(Item));
	item->type = ITEM_TEXT;
	item->text = text;
	item->next = NULL;
	return item;
}

Item * AppendItemSemanticAction(Item * list, Item * item) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (list == NULL) {
		return item;
	}
	Item * current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = item;
	return list;
}

/* ── header ── */

HeaderProperty * HeaderPropertySemanticAction(HeaderPropertyType type, char * stringValue, int intValue) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	HeaderProperty * prop = calloc(1, sizeof(HeaderProperty));
	prop->type = type;
	prop->stringValue = stringValue;
	prop->intValue = intValue;
	return prop;
}

static void _applyHeaderProperty(Header * header, HeaderProperty * prop) {
	switch (prop->type) {
		case HEADER_PROP_TITLE:
			if (header->title != NULL) _duplicatePropertyError("header", "title");
			header->title = prop->stringValue; break;
		case HEADER_PROP_SUBJECT:
			if (header->subject != NULL) _duplicatePropertyError("header", "subject");
			header->subject = prop->stringValue; break;
		case HEADER_PROP_PROFESSOR:
			if (header->professor != NULL) _duplicatePropertyError("header", "professor");
			header->professor = prop->stringValue; break;
		case HEADER_PROP_DATE:
			if (header->date != NULL) _duplicatePropertyError("header", "date");
			header->date = prop->stringValue; break;
		case HEADER_PROP_DURATION:
			if (header->duration != -1) _duplicatePropertyError("header", "duration");
			header->duration = prop->intValue; break;
		case HEADER_PROP_SCORE_GRID:
			if (header->score_grid != -1) _duplicatePropertyError("header", "score_grid");
			header->score_grid = prop->intValue; break;
		case HEADER_PROP_ANSWER_SHEET:
			if (header->answer_sheet != -1) _duplicatePropertyError("header", "answer_sheet");
			header->answer_sheet = prop->intValue; break;
		case HEADER_PROP_STUDENT_NAME:
			if (header->student_name != -1) _duplicatePropertyError("header", "student_name");
			header->student_name = prop->intValue; break;
		case HEADER_PROP_STUDENT_ID:
			if (header->student_id != -1) _duplicatePropertyError("header", "student_id");
			header->student_id = prop->intValue; break;
		case HEADER_PROP_COURSE:
			if (header->course != -1) _duplicatePropertyError("header", "course");
			header->course = prop->intValue; break;
		case HEADER_PROP_INSTRUCTIONS: {
			StringList * node = calloc(1, sizeof(StringList));
			node->value = prop->stringValue;
			node->next = NULL;
			if (header->instructions == NULL) {
				header->instructions = node;
			} else {
				StringList * current = header->instructions;
				while (current->next != NULL) current = current->next;
				current->next = node;
			}
			break;
		}
	}
	free(prop);
}

Header * CreateHeaderSemanticAction(HeaderProperty * firstProp) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Header * header = calloc(1, sizeof(Header));
	header->duration = -1;
	header->score_grid = -1;
	header->answer_sheet = -1;
	header->student_name = -1;
	header->student_id = -1;
	header->course = -1;
	_applyHeaderProperty(header, firstProp);
	return header;
}

Header * AddHeaderPropertySemanticAction(Header * header, HeaderProperty * prop) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	_applyHeaderProperty(header, prop);
	return header;
}

Item * ExerciseItemSemanticAction(ExerciseType type, void * node) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Exercise * exercise = calloc(1, sizeof(Exercise));
	exercise->type = type;
	exercise->next = NULL;
	switch (type) {
		case EXERCISE_MULTIPLECHOICE: exercise->multiplechoice = (MultiplechoiceNode *) node; break;
		case EXERCISE_FILLBLANKS: exercise->fillblanks = (FillblanksNode *) node; break;
		case EXERCISE_OPENQUESTION: exercise->openquestion = (OpenquestionNode *) node; break;
		case EXERCISE_CHOOSEFROM: exercise->choosefrom = (ChoosefromNode *) node; break;
		case EXERCISE_MATCH: exercise->match = (MatchNode *) node; break;
		case EXERCISE_TRUEORFALSE: exercise->trueorfalse = (TrueorfalseNode *) node; break;
		case EXERCISE_CHART: exercise->chart = (ChartNode *) node; break;
		case EXERCISE_SET: exercise->set = (SetNode *) node; break;
	}
	Item * item = calloc(1, sizeof(Item));
	item->type = ITEM_EXERCISE;
	item->exercise = exercise;
	item->next = NULL;
	return item;
}

/* ── mc ── */

MultiplechoiceNode * CreateMultiplechoiceNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MultiplechoiceNode * node = calloc(1, sizeof(MultiplechoiceNode));
	node->score = -1;
	return node;
}

MultiplechoiceNode * MultiplechoiceSetQuestionSemanticAction(MultiplechoiceNode * node, char * question) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->question != NULL) {
		_duplicatePropertyError("multiplechoice", "question");
	}
	node->question = question;
	return node;
}

MultiplechoiceNode * MultiplechoiceAddOptionSemanticAction(MultiplechoiceNode * node, char * value, int isCorrect) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MultiplechoiceOption * option = calloc(1, sizeof(MultiplechoiceOption));
	option->value = value;
	option->is_correct = isCorrect;
	option->next = NULL;
	/* append to end of list */
	if (node->options == NULL) {
		node->options = option;
	} else {
		MultiplechoiceOption * current = node->options;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = option;
	}
	return node;
}

MultiplechoiceNode * MultiplechoiceAddIntOptionSemanticAction(MultiplechoiceNode * node, int value, int isCorrect) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	char buf[32];
	snprintf(buf, sizeof(buf), "%d", value);
	return MultiplechoiceAddOptionSemanticAction(node, strdup(buf), isCorrect);
}

MultiplechoiceNode * MultiplechoiceSetScoreSemanticAction(MultiplechoiceNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("multiplechoice", "score");
	}
	node->score = score;
	return node;
}

/* ── torf ── */

TrueorfalseNode * CreateTrueorfalseNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TrueorfalseNode * node = calloc(1, sizeof(TrueorfalseNode));
	node->justify = -1;
	node->answer = -1;
	node->score = -1;
	return node;
}

TrueorfalseNode * TrueorfalseSetQuestionSemanticAction(TrueorfalseNode * node, char * question) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->question != NULL) {
		_duplicatePropertyError("trueorfalse", "question");
	}
	node->question = question;
	return node;
}

TrueorfalseNode * TrueorfalseSetJustifySemanticAction(TrueorfalseNode * node, int justify) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->justify != -1) {
		_duplicatePropertyError("trueorfalse", "justify");
	}
	node->justify = justify;
	return node;
}

TrueorfalseNode * TrueorfalseSetAnswerSemanticAction(TrueorfalseNode * node, int answer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->answer != -1) {
		_duplicatePropertyError("trueorfalse", "answer");
	}
	node->answer = answer;
	return node;
}

TrueorfalseNode * TrueorfalseSetScoreSemanticAction(TrueorfalseNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("trueorfalse", "score");
	}
	node->score = score;
	return node;
}

/* ── direct ── */

OpenquestionNode * CreateOpenquestionNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	OpenquestionNode * node = calloc(1, sizeof(OpenquestionNode));
	node->lines = -1;
	node->score = -1;
	return node;
}

OpenquestionNode * OpenquestionSetQuestionSemanticAction(OpenquestionNode * node, char * question) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->question != NULL) {
		_duplicatePropertyError("openquestion", "question");
	}
	node->question = question;
	return node;
}

OpenquestionNode * OpenquestionSetLinesSemanticAction(OpenquestionNode * node, int lines) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->lines != -1) {
		_duplicatePropertyError("openquestion", "lines");
	}
	node->lines = lines;
	return node;
}

OpenquestionNode * OpenquestionSetAnswerSemanticAction(OpenquestionNode * node, char * answer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->answer != NULL) {
		_duplicatePropertyError("openquestion", "answer");
	}
	node->answer = answer;
	return node;
}

OpenquestionNode * OpenquestionSetScoreSemanticAction(OpenquestionNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("openquestion", "score");
	}
	node->score = score;
	return node;
}

/* ── blanks ── */

FillblanksNode * CreateFillblanksNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FillblanksNode * node = calloc(1, sizeof(FillblanksNode));
	node->score = -1;
	return node;
}

FillblanksNode * FillblanksSetQuestionSemanticAction(FillblanksNode * node, char * question) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->question != NULL) {
		_duplicatePropertyError("fillblanks", "question");
	}
	node->question = question;
	return node;
}

FillblanksNode * FillblanksAddAnswerSemanticAction(FillblanksNode * node, char * answer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StringList * entry = calloc(1, sizeof(StringList));
	entry->value = answer;
	entry->next = NULL;
	if (node->answers == NULL) {
		node->answers = entry;
	} else {
		StringList * current = node->answers;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = entry;
	}
	return node;
}

FillblanksNode * FillblanksSetScoreSemanticAction(FillblanksNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("fillblanks", "score");
	}
	node->score = score;
	return node;
}

/* ── lists ── */

StringList * CreateStringListSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StringList * node = calloc(1, sizeof(StringList));
	node->value = value;
	node->next = NULL;
	return node;
}

StringList * AppendStringListSemanticAction(StringList * list, char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StringList * node = calloc(1, sizeof(StringList));
	node->value = value;
	node->next = NULL;
	StringList * current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = node;
	return list;
}

IntList * CreateIntListSemanticAction(int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	IntList * node = calloc(1, sizeof(IntList));
	node->value = value;
	node->next = NULL;
	return node;
}

IntList * AppendIntListSemanticAction(IntList * list, int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	IntList * node = calloc(1, sizeof(IntList));
	node->value = value;
	node->next = NULL;
	IntList * current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = node;
	return list;
}

/* ── chooseFrom ── */

ChoosefromNode * CreateChoosefromNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ChoosefromNode * node = calloc(1, sizeof(ChoosefromNode));
	node->score = -1;
	node->shuffle = -1;
	return node;
}

ChoosefromNode * ChoosefromSetTaskSemanticAction(ChoosefromNode * node, char * task) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->task != NULL) {
		_duplicatePropertyError("choosefrom", "task");
	}
	node->task = task;
	return node;
}

ChoosefromNode * ChoosefromSetOptionsSemanticAction(ChoosefromNode * node, StringList * options) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->options != NULL) {
		_duplicatePropertyError("choosefrom", "options");
	}
	node->options = options;
	return node;
}

ChoosefromNode * ChoosefromSetTextSemanticAction(ChoosefromNode * node, char * text) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->text != NULL) {
		_duplicatePropertyError("choosefrom", "text");
	}
	node->text = text;
	return node;
}

ChoosefromNode * ChoosefromSetAnswerSemanticAction(ChoosefromNode * node, IntList * answer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->answer != NULL) {
		_duplicatePropertyError("choosefrom", "answer");
	}
	node->answer = answer;
	return node;
}

ChoosefromNode * ChoosefromSetScoreSemanticAction(ChoosefromNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("choosefrom", "score");
	}
	node->score = score;
	return node;
}

ChoosefromNode * ChoosefromSetShuffleSemanticAction(ChoosefromNode * node, int shuffle) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->shuffle != -1) {
		_duplicatePropertyError("choosefrom", "shuffle");
	}
	node->shuffle = shuffle;
	return node;
}

/* ── match ── */

MatchNode * CreateMatchNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MatchNode * node = calloc(1, sizeof(MatchNode));
	node->score = -1;
	return node;
}

MatchNode * MatchSetQuestionSemanticAction(MatchNode * node, char * question) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->question != NULL) {
		_duplicatePropertyError("match", "question");
	}
	node->question = question;
	return node;
}

MatchNode * MatchAddPairSemanticAction(MatchNode * node, char * left, char * right) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MatchPair * pair = calloc(1, sizeof(MatchPair));
	pair->left = left;
	pair->right = right;
	pair->next = NULL;
	if (node->pairs == NULL) {
		node->pairs = pair;
	} else {
		MatchPair * current = node->pairs;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = pair;
	}
	return node;
}

MatchNode * MatchSetScoreSemanticAction(MatchNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("match", "score");
	}
	node->score = score;
	return node;
}

/* ── chart ── */

ChartNode * CreateChartNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ChartNode * node = calloc(1, sizeof(ChartNode));
	node->score = -1;
	return node;
}

ChartNode * ChartSetTaskSemanticAction(ChartNode * node, char * task) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->task != NULL) {
		_duplicatePropertyError("chart", "task");
	}
	node->task = task;
	return node;
}

ChartNode * ChartSetDimSemanticAction(ChartNode * node, int rows, int cols) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->dim_rows != 0) {
		_duplicatePropertyError("chart", "dim");
	}
	node->dim_rows = rows;
	node->dim_cols = cols;
	return node;
}

ChartNode * ChartAddCellSemanticAction(ChartNode * node, int row, int col, char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ChartCell * cell = calloc(1, sizeof(ChartCell));
	cell->row = row;
	cell->col = col;
	cell->value = value;
	cell->next = NULL;
	if (node->cells == NULL) {
		node->cells = cell;
	} else {
		ChartCell * current = node->cells;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = cell;
	}
	return node;
}

ChartNode * ChartSetAnswerSemanticAction(ChartNode * node, StringList * answer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->answer != NULL) {
		_duplicatePropertyError("chart", "answer");
	}
	node->answer = answer;
	return node;
}

ChartNode * ChartSetScoreSemanticAction(ChartNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("chart", "score");
	}
	node->score = score;
	return node;
}

/* ── set ── */

SetNode * CreateSetNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SetNode * node = calloc(1, sizeof(SetNode));
	node->shuffle = -1;
	node->score = -1;
	return node;
}

SetNode * SetSetTextSemanticAction(SetNode * node, char * text) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->text != NULL) {
		_duplicatePropertyError("set", "text");
	}
	node->text = text;
	return node;
}

SetNode * SetSetScoreSemanticAction(SetNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->score != -1) {
		_duplicatePropertyError("set", "score");
	}
	node->score = score;
	return node;
}

SetNode * SetSetShuffleSemanticAction(SetNode * node, int shuffle) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->shuffle != -1) {
		_duplicatePropertyError("set", "shuffle");
	}
	node->shuffle = shuffle;
	return node;
}

SetNode * SetAddExerciseSemanticAction(SetNode * node, ExerciseType type, void * exerciseNode) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	Exercise * exercise = calloc(1, sizeof(Exercise));
	exercise->type = type;
	exercise->next = NULL;

	switch (type) {
		case EXERCISE_MULTIPLECHOICE:
			exercise->multiplechoice = (MultiplechoiceNode *) exerciseNode;
			break;
		case EXERCISE_FILLBLANKS:
			exercise->fillblanks = (FillblanksNode *) exerciseNode;
			break;
		case EXERCISE_OPENQUESTION:
			exercise->openquestion = (OpenquestionNode *) exerciseNode;
			break;
		case EXERCISE_CHOOSEFROM:
			exercise->choosefrom = (ChoosefromNode *) exerciseNode;
			break;
		case EXERCISE_MATCH:
			exercise->match = (MatchNode *) exerciseNode;
			break;
		case EXERCISE_TRUEORFALSE:
			exercise->trueorfalse = (TrueorfalseNode *) exerciseNode;
			break;
		case EXERCISE_CHART:
			exercise->chart = (ChartNode *) exerciseNode;
			break;
		default:
			break;
	}

	if (node->exercises == NULL) {
		node->exercises = exercise;
	} else {
		Exercise * current = node->exercises;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = exercise;
	}

	return node;
}

/* ── section ── */

Item * SectionItemSemanticAction(SectionNode * section) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	Item * item = calloc(1, sizeof(Item));
	item->type = ITEM_SECTION;
	item->section = section;
	item->next = NULL;

	return item;
}

SectionNode * CreateSectionNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	SectionNode * node = calloc(1, sizeof(SectionNode));
	node->select = -1;

	return node;
}

SectionNode * SectionSetNameSemanticAction(SectionNode * node, char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	node->name = name;

	return node;
}

SectionNode * SectionSetSelectSemanticAction(SectionNode * node, int select) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	node->select = select;

	return node;
}

SectionNode * SectionAddItemSemanticAction(SectionNode * node, Item * item) {
	_logSyntacticAnalyzerAction(__FUNCTION__);

	if (node->items == NULL) {
		node->items = item;
	} else {
		Item * current = node->items;

		while (current->next != NULL) {
			current = current->next;
		}

		current->next = item;
	}

	return node;
}

/* ── image ── */

Item * ImageItemSemanticAction(ImageNode * image) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Item * item = calloc(1, sizeof(Item));
	item->type = ITEM_IMAGE;
	item->image = image;
	item->next = NULL;
	return item;
}

Item * ChartItemSemanticAction(ChartNode * chart) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Item * item = calloc(1, sizeof(Item));
	item->type = ITEM_CHART;
	item->chart = chart;
	item->next = NULL;
	return item;
}

ImageNode * CreateImageNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ImageNode * node = calloc(1, sizeof(ImageNode));
	node->width = -1;
	return node;
}

ImageNode * ImageSetPathSemanticAction(ImageNode * node, char * path) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->path != NULL) {
		_duplicatePropertyError("image", "path");
	}
	node->path = path;
	return node;
}

ImageNode * ImageSetCaptionSemanticAction(ImageNode * node, char * caption) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->caption != NULL) {
		_duplicatePropertyError("image", "caption");
	}
	node->caption = caption;
	return node;
}

ImageNode * ImageSetWidthSemanticAction(ImageNode * node, int width) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	if (node->width != -1) {
		_duplicatePropertyError("image", "width");
	}
	node->width = width;
	return node;
}
