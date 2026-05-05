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

Program * ProgramSemanticAction(Header * header) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->header = header;
	program->items = NULL;
	_compilerState->abstractSyntaxtTree = program;
	return program;
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
