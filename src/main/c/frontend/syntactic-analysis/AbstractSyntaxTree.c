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
		free(header->professor);
		free(header->date);
		free(header->instructions);
		free(header);
	}
}

void destroyMultiplechoiceOption(MultiplechoiceOption * option) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (option != NULL) {
		MultiplechoiceOption * next = option->next;
		free(option->value);
		free(option);
		option = next;
	}
}

void destroyMultiplechoiceNode(MultiplechoiceNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		destroyMultiplechoiceOption(node->options);
		free(node);
	}
}

void destroyFillblanksNode(FillblanksNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		destroyStringList(node->answers);
		free(node);
	}
}

void destroyOpenquestionNode(OpenquestionNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->question);
		free(node->answer);
		free(node);
	}
}

void destroyTrueorfalseNode(TrueorfalseNode * node) {
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

void destroyChoosefromNode(ChoosefromNode * node) {
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
			case EXERCISE_MULTIPLECHOICE: destroyMultiplechoiceNode(exercise->multiplechoice); break;
			case EXERCISE_FILLBLANKS: destroyFillblanksNode(exercise->fillblanks); break;
			case EXERCISE_OPENQUESTION: destroyOpenquestionNode(exercise->openquestion); break;
			case EXERCISE_CHOOSEFROM: destroyChoosefromNode(exercise->choosefrom); break;
			case EXERCISE_MATCH: destroyMatchNode(exercise->match); break;
			case EXERCISE_TRUEORFALSE: destroyTrueorfalseNode(exercise->trueorfalse); break;
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

void destroyImageNode(ImageNode * node) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (node != NULL) {
		free(node->path);
		free(node->caption);
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
			case ITEM_IMAGE: destroyImageNode(item->image); break;
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
