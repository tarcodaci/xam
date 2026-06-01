#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include <stdbool.h>

/** Initialize module's internal state. */
ModuleDestructor initializeSemanticAnalyzerModule();

typedef struct SemanticError SemanticError;

struct SemanticError {
	char * message;
	SemanticError * next;
};

/**
 * Validates the AST. Returns NULL if valid, or a linked list of errors.
 */
SemanticError * validateProgram(Program * program);

void destroySemanticErrors(SemanticError * errors);

#endif
