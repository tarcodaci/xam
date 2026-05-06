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

/* PUBLIC FUNCTIONS */

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
		case HEADER_PROP_TITLE: header->title = prop->stringValue; break;
		case HEADER_PROP_SUBJECT: header->subject = prop->stringValue; break;
		case HEADER_PROP_DATE: header->date = prop->stringValue; break;
		case HEADER_PROP_DURATION: header->duration = prop->intValue; break;
		case HEADER_PROP_SCORE_GRID: header->score_grid = prop->intValue; break;
		case HEADER_PROP_ANSWER_SHEET: header->answer_sheet = prop->intValue; break;
		case HEADER_PROP_STUDENT_NAME: header->student_name = prop->intValue; break;
		case HEADER_PROP_STUDENT_ID: header->student_id = prop->intValue; break;
		case HEADER_PROP_COURSE: header->course = prop->intValue; break;
		case HEADER_PROP_INSTRUCTIONS: header->instructions = prop->stringValue; break;
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
		case EXERCISE_MC: exercise->mc = (McNode *) node; break;
		case EXERCISE_BLANKS: exercise->blanks = (BlanksNode *) node; break;
		case EXERCISE_DIRECT: exercise->direct = (DirectNode *) node; break;
		case EXERCISE_CHOOSE_FROM: exercise->chooseFrom = (ChooseFromNode *) node; break;
		case EXERCISE_MATCH: exercise->match = (MatchNode *) node; break;
		case EXERCISE_TORF: exercise->torf = (TorfNode *) node; break;
		case EXERCISE_CHART: exercise->chart = (ChartNode *) node; break;
		case EXERCISE_SET: exercise->set = (SetNode *) node; break;
	}
	Item * item = calloc(1, sizeof(Item));
	item->type = ITEM_EXERCISE;
	item->exercise = exercise;
	item->next = NULL;
	return item;
}

McNode * CreateMcNodeSemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	McNode * node = calloc(1, sizeof(McNode));
	node->score = -1;
	return node;
}

McNode * McSetQuestionSemanticAction(McNode * node, char * question) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	node->question = question;
	return node;
}

McNode * McAddOptionSemanticAction(McNode * node, char * value, int isCorrect) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	McOption * option = calloc(1, sizeof(McOption));
	option->value = value;
	option->is_correct = isCorrect;
	option->next = NULL;
	/* append to end of list */
	if (node->options == NULL) {
		node->options = option;
	} else {
		McOption * current = node->options;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = option;
	}
	return node;
}

McNode * McAddIntOptionSemanticAction(McNode * node, int value, int isCorrect) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	char buf[32];
	snprintf(buf, sizeof(buf), "%d", value);
	return McAddOptionSemanticAction(node, strdup(buf), isCorrect);
}

McNode * McSetScoreSemanticAction(McNode * node, int score) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	node->score = score;
	return node;
}
