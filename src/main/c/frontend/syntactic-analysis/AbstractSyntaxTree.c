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
		// TODO
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
