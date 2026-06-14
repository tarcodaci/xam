#include "SemanticAnalyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void _shutdownSemanticAnalyzerModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: SemanticAnalyzer...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeSemanticAnalyzerModule() {
	_logger = createLogger("SemanticAnalyzer");
	return _shutdownSemanticAnalyzerModule;
}

/* PRIVATE FUNCTIONS */

static SemanticError * _createError(const char * message) {
	SemanticError * error = calloc(1, sizeof(SemanticError));
	error->message = strdup(message);
	error->next = NULL;
	return error;
}

static SemanticError * _appendError(SemanticError * list, SemanticError * error) {
	if (error == NULL) return list;
	if (list == NULL) return error;
	SemanticError * current = list;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = error;
	return list;
}

static int _countOptions(MultiplechoiceOption * options) {
	int count = 0;
	while (options != NULL) {
		count++;
		options = options->next;
	}
	return count;
}

static int _countStringList(StringList * list) {
	int count = 0;
	while (list != NULL) {
		count++;
		list = list->next;
	}
	return count;
}

static int _countIntList(IntList * list) {
	int count = 0;
	while (list != NULL) {
		count++;
		list = list->next;
	}
	return count;
}

/* Counts groups of consecutive underscores (each group = one blank). */
static int _countUnderscores(const char * str) {
	int count = 0;
	int inGroup = 0;
	if (str == NULL) return 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] == '_') {
			if (!inGroup) {
				count++;
				inGroup = 1;
			}
		} else {
			inGroup = 0;
		}
	}
	return count;
}

static SemanticError * _validateExercise(Exercise * exercise) {
	switch (exercise->type) {
		case EXERCISE_MULTIPLECHOICE:
			if (exercise->multiplechoice->question == NULL) {
				return _createError("multiplechoice: 'question' property is mandatory.");
			}
			if (_countOptions(exercise->multiplechoice->options) < 2) {
				return _createError("multiplechoice: must have at least 2 options.");
			}
			return NULL;
		case EXERCISE_FILLBLANKS:
			if (exercise->fillblanks->question == NULL) {
				return _createError("fillblanks: 'question' property is mandatory.");
			}
			if (_countUnderscores(exercise->fillblanks->question) == 0) {
				return _createError("fillblanks: 'question' must contain at least one '_'.");
			}
			return NULL;
		case EXERCISE_CHOOSEFROM: {
			int optionCount = _countStringList(exercise->choosefrom->options);
			int blanks = exercise->choosefrom->text != NULL ? _countUnderscores(exercise->choosefrom->text) : 0;
			if (optionCount < 2) {
				return _createError("choosefrom: must have at least 2 options.");
			}
			if (blanks > optionCount) {
				return _createError("choosefrom: must have at least as many options as blanks in 'text'.");
			}
			if (exercise->choosefrom->answer != NULL) {
				int answerCount = _countIntList(exercise->choosefrom->answer);
				if (answerCount != blanks) {
					return _createError("choosefrom: answer count must equal the number of blanks in 'text'.");
				}
			}
			return NULL;
		}
		case EXERCISE_TRUEORFALSE:
			if (exercise->trueorfalse->question == NULL) {
				return _createError("trueorfalse: 'question' property is mandatory.");
			}
			return NULL;
		case EXERCISE_OPENQUESTION:
			if (exercise->openquestion->question == NULL) {
				return _createError("openquestion: 'question' property is mandatory.");
			}
			return NULL;
		case EXERCISE_MATCH: {
			int pairCount = 0;
			MatchPair * p = exercise->match->pairs;
			while (p != NULL) { pairCount++; p = p->next; }
			if (pairCount < 2) {
				return _createError("match: must have at least 2 pairs.");
			}
			return NULL;
		}
		case EXERCISE_CHART: {
			ChartNode * chart = exercise->chart;
			ChartCell * cell = chart->cells;
			while (cell != NULL) {
				if (cell->row < 1 || cell->row > chart->dim_rows ||
				    cell->col < 1 || cell->col > chart->dim_cols) {
					return _createError("fillchart: cell position exceeds declared dimensions.");
				}
				cell = cell->next;
			}
			return NULL;
		}
		default:
			return NULL;
	}
}

static SemanticError * _validateItems(Item * items) {
	SemanticError * errors = NULL;
	while (items != NULL) {
		if (items->type == ITEM_EXERCISE) {
			errors = _appendError(errors, _validateExercise(items->exercise));
		} else if (items->type == ITEM_SECTION) {
			errors = _appendError(errors, _validateItems(items->section->items));
		}
		items = items->next;
	}
	return errors;
}

static int _getExerciseScore(Exercise * exercise) {
	switch (exercise->type) {
		case EXERCISE_MULTIPLECHOICE: return exercise->multiplechoice->score;
		case EXERCISE_FILLBLANKS: return exercise->fillblanks->score;
		case EXERCISE_OPENQUESTION: return exercise->openquestion->score;
		case EXERCISE_TRUEORFALSE: return exercise->trueorfalse->score;
		case EXERCISE_MATCH: return exercise->match->score;
		case EXERCISE_CHOOSEFROM: return exercise->choosefrom->score;
		case EXERCISE_CHART: return exercise->chart->score;
		case EXERCISE_SET: return exercise->set->score;
		default: return -1;
	}
}

static SemanticError * _validateScoreGrid(Item * items) {
	SemanticError * errors = NULL;
	while (items != NULL) {
		if (items->type == ITEM_EXERCISE) {
			if (_getExerciseScore(items->exercise) == -1) {
				errors = _appendError(errors, _createError("score_grid: every exercise must declare 'score'."));
			}
		}
		items = items->next;
	}
	return errors;
}

static int _hasCorrectOption(MultiplechoiceOption * options) {
	while (options != NULL) {
		if (options->is_correct) return 1;
		options = options->next;
	}
	return 0;
}

static SemanticError * _validateAnswerSheet(Item * items) {
	SemanticError * errors = NULL;
	while (items != NULL) {
		if (items->type == ITEM_EXERCISE) {
			Exercise * ex = items->exercise;
			switch (ex->type) {
				case EXERCISE_MULTIPLECHOICE:
					if (!_hasCorrectOption(ex->multiplechoice->options)) {
						errors = _appendError(errors, _createError("answer_sheet: multiplechoice must mark at least one correct option with '#'."));
					}
					break;
				case EXERCISE_FILLBLANKS:
					if (ex->fillblanks->answers == NULL) {
						errors = _appendError(errors, _createError("answer_sheet: fillblanks must declare 'answer'."));
					}
					break;
				case EXERCISE_TRUEORFALSE:
					if (ex->trueorfalse->answer == -1) {
						errors = _appendError(errors, _createError("answer_sheet: trueorfalse must declare 'answer'."));
					}
					break;
				default:
					break;
			}
		}
		items = items->next;
	}
	return errors;
}

/* PUBLIC FUNCTIONS */

SemanticError * validateProgram(Program * program) {
	logDebugging(_logger, "Validating program...");
	SemanticError * errors = NULL;

	if (program->header != NULL && program->header->title == NULL) {
		errors = _appendError(errors, _createError("header: 'title' property is mandatory."));
	}

	errors = _appendError(errors, _validateItems(program->items));

	/* A program must have at least one exercise. */
	int hasExercise = 0;
	Item * item = program->items;
	while (item != NULL) {
		if (item->type == ITEM_EXERCISE) { hasExercise = 1; break; }
		if (item->type == ITEM_SECTION) { hasExercise = 1; break; }
		item = item->next;
	}
	if (!hasExercise) {
		errors = _appendError(errors, _createError("program: must contain at least one exercise."));
	}

	if (program->header != NULL && program->header->score_grid == 1) {
		errors = _appendError(errors, _validateScoreGrid(program->items));
		/* Score total must be 10 or 100. */
		int total = 0;
		Item * it = program->items;
		while (it != NULL) {
			if (it->type == ITEM_EXERCISE) {
				int s = _getExerciseScore(it->exercise);
				if (s > 0) total += s;
			}
			it = it->next;
		}
		if (total != 10 && total != 100) {
			errors = _appendError(errors, _createError("score_grid: total score must be 10 or 100."));
		}
	}

	if (program->header != NULL && program->header->answer_sheet == 1) {
		errors = _appendError(errors, _validateAnswerSheet(program->items));
	}

	return errors;
}

void destroySemanticErrors(SemanticError * errors) {
	while (errors != NULL) {
		SemanticError * next = errors->next;
		free(errors->message);
		free(errors);
		errors = next;
	}
}
