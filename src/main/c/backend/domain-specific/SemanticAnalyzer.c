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

/* PUBLIC FUNCTIONS */

SemanticError * validateProgram(Program * program) {
	logDebugging(_logger, "Validating program...");
	SemanticError * errors = NULL;

	if (program->header != NULL && program->header->title == NULL) {
		errors = _appendError(errors, _createError("header: 'title' property is mandatory."));
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
