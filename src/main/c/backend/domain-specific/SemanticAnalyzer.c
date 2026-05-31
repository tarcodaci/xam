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

static int _countUnderscores(const char * str) {
	int count = 0;
	if (str == NULL) return 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] == '_') count++;
	}
	return count;
}

static SemanticError * _validateExercise(Exercise * exercise) {
	switch (exercise->type) {
		case EXERCISE_MULTIPLECHOICE:
			if (_countOptions(exercise->multiplechoice->options) < 2) {
				return _createError("multiplechoice: must have at least 2 options.");
			}
			return NULL;
		case EXERCISE_FILLBLANKS:
			if (exercise->fillblanks->question != NULL && _countUnderscores(exercise->fillblanks->question) == 0) {
				return _createError("fillblanks: 'question' must contain at least one '_'.");
			}
			return NULL;
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

/* PUBLIC FUNCTIONS */

SemanticError * validateProgram(Program * program) {
	logDebugging(_logger, "Validating program...");
	SemanticError * errors = NULL;

	if (program->header != NULL && program->header->title == NULL) {
		errors = _appendError(errors, _createError("header: 'title' property is mandatory."));
	}

	errors = _appendError(errors, _validateItems(program->items));

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
