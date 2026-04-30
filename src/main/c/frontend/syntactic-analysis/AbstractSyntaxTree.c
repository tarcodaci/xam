#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case FACTOR:
				destroyFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void destroyFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				destroyConstant(factor->constant);
				break;
			case EXPRESSION:
				destroyExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyHeader(program->header);
		destroyItem(program->items);
		free(program);
	}
}

void destroyStringList(StringList * list) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (list != NULL) {
		StringList * next = list->next;
		free(list->value);
		free(list);
		list = next;
	}
}

void destroyIntList(IntList * list) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (list != NULL) {
		IntList * next = list->next;
		free(list);
		list = next;
	}
}

void destroyHeader(Header * header) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (header != NULL) {
		free(header->title);
		free(header->subject);
		free(header->date);
		free(header->instructions);
		free(header);
	}
}

void destroyMcOption(McOption * option) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (option != NULL) {
		McOption * next = option->next;
		free(option->value);
		free(option);
		option = next;
	}
}

void destroyMcNode(McNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		destroyMcOption(node->options);
		free(node);
	}
}

void destroyBlanksNode(BlanksNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		destroyStringList(node->answers);
		free(node);
	}
}

void destroyDirectNode(DirectNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		free(node->answer);
		free(node);
	}
}

void destroyTorfNode(TorfNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		free(node);
	}
}

void destroyMatchPair(MatchPair * pair) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (pair != NULL) {
		MatchPair * next = pair->next;
		free(pair->left);
		free(pair->right);
		free(pair);
		pair = next;
	}
}

void destroyMatchNode(MatchNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		destroyMatchPair(node->pairs);
		free(node);
	}
}

void destroyChartCell(ChartCell * cell) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (cell != NULL) {
		ChartCell * next = cell->next;
		free(cell->value);
		free(cell);
		cell = next;
	}
}

void destroyChartNode(ChartNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->task);
		destroyChartCell(node->cells);
		destroyStringList(node->answer);
		free(node);
	}
}

void destroyChooseFromNode(ChooseFromNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->task);
		destroyStringList(node->options);
		free(node->text);
		destroyIntList(node->answer);
		free(node);
	}
}

void destroyExercise(Exercise * exercise) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (exercise != NULL) {
		Exercise * next = exercise->next;
		switch (exercise->type) {
			case EXERCISE_MC: destroyMcNode(exercise->mc); break;
			case EXERCISE_BLANKS: destroyBlanksNode(exercise->blanks); break;
			case EXERCISE_DIRECT: destroyDirectNode(exercise->direct); break;
			case EXERCISE_CHOOSE_FROM: destroyChooseFromNode(exercise->chooseFrom); break;
			case EXERCISE_MATCH: destroyMatchNode(exercise->match); break;
			case EXERCISE_TORF: destroyTorfNode(exercise->torf); break;
			case EXERCISE_CHART: destroyChartNode(exercise->chart); break;
			case EXERCISE_SET: destroySetNode(exercise->set); break;
		}
		free(exercise);
		exercise = next;
	}
}

void destroySetNode(SetNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->text);
		destroyExercise(node->exercises);
		free(node);
	}
}

void destroyItem(Item * item) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (item != NULL) {
		Item * next = item->next;
		switch (item->type) {
			case ITEM_EXERCISE: destroyExercise(item->exercise); break;
			case ITEM_SECTION: destroySectionNode(item->section); break;
			case ITEM_TEXT: free(item->text); break;
		}
		free(item);
		item = next;
	}
}

void destroySectionNode(SectionNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->name);
		destroyItem(node->items);
		free(node);
	}
}
