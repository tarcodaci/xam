#include "Generator.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}

/* PRIVATE FUNCTIONS */

static int _indent = 0;

static void _line(const char * format, ...) {
	for (int i = 0; i < _indent; i++) fprintf(stdout, "\t");
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

/* Continue on the current line without indentation. */
static void _append(const char * format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

static void _generatePreamble(Header * header) {
	_line("\\documentclass[11pt, a4paper, addpoints]{exam}\n");
	_line("\\usepackage[utf8]{inputenc}\n");
	_line("\\usepackage[T1]{fontenc}\n");
	_line("\\usepackage[spanish]{babel}\n");
	_line("\\usepackage[margin=2.5cm]{geometry}\n");
	_line("\\usepackage{amsmath, amssymb}\n");
	_line("\\usepackage{graphicx}\n");
	_line("\\usepackage{enumitem}\n");
	_line("\n");
	_line("\\pagestyle{headandfoot}\n");
	if (header != NULL && (header->student_name == 1 || header->student_id == 1 || header->course == 1)) {
		_line("\\header{");
		if (header->student_name == 1) _append("Apellido y Nombre:\\rule{6cm}{0.4pt} ");
		if (header->student_id == 1) _append("Legajo:\\rule{2cm}{0.4pt} ");
		if (header->course == 1) _append("Curso:\\rule{2cm}{0.4pt}");
		_append("}{}{\\thepage}\n");
	} else {
		_line("\\header{}{}{\\thepage}\n");
	}
	_line("\\footer{}{}{}\n");
	_line("\n");
	_line("\\begin{document}\n");
	_line("\n");
}

static void _generateEpilogue() {
	_line("\\end{document}\n");
}

/* PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	Program * program = compilerState->abstractSyntaxtTree;

	_generatePreamble(program->header);
	_generateEpilogue();

	logDebugging(_logger, "Generation is done.");
}
