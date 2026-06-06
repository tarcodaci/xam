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
	_line("\\pointname{ pts}\n");
	_line("\\pointformat{}\n");
	_line("\\qformat{\\textbf{Ejercicio \\thequestion.} }\n");
	_line("\\renewcommand{\\totalformat}{Total}\n");
	_line("\\hqword{Ejercicio}\n");
	_line("\\hpword{Puntos}\n");
	_line("\\hsword{Nota}\n");
	_line("\\htword{Total}\n");
	_line("\n");
	_line("\\pagestyle{headandfoot}\n");
	if (header != NULL && (header->student_name == 1 || header->student_id == 1 || header->course == 1)) {
		_line("\\header{");
		if (header->student_name == 1) _append("Nombre y Apellido:\\hrulefill\\hspace{1em} ");
		if (header->student_id == 1) _append("Legajo:\\rule{2cm}{0.4pt} ");
		if (header->course == 1) _append("Curso:\\rule{2cm}{0.4pt}\\hspace{1em}");
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

static void _generateOpenquestion(OpenquestionNode * node) {
	if (node->score >= 0) {
		_line("\\question[%d] %s\n", node->score, node->question);
	} else {
		_line("\\question %s\n", node->question);
	}
	int lines = node->lines >= 0 ? node->lines : 3;
	if (lines > 0) {
		_line("\\fillwithlines{%d\\baselineskip}\n", lines);
	}
	_line("\n");
}

static void _generateExercise(Exercise * exercise) {
	switch (exercise->type) {
		case EXERCISE_OPENQUESTION: _generateOpenquestion(exercise->openquestion); break;
		default: break;
	}
}

static void _generateItems(Item * items) {
	_line("\\begin{questions}\n");
	_line("\n");
	_indent++;
	while (items != NULL) {
		if (items->type == ITEM_EXERCISE) {
			_generateExercise(items->exercise);
		} else if (items->type == ITEM_TEXT) {
			_indent--;
			_line("\\end{questions}\n");
			_line("\\noindent %s\n\n", items->text);
			_line("\\begin{questions}\n");
			_indent++;
		}
		items = items->next;
	}
	_indent--;
	_line("\\end{questions}\n");
	_line("\n");
}

static void _generateHeaderBlock(Header * header) {
	if (header == NULL) return;

	_line("\\begin{center}\n");
	_indent++;
	if (header->subject != NULL) {
		_line("{\\Large\\bfseries %s}\\\\[0.3em]\n", header->subject);
	}
	if (header->title != NULL) {
		_line("{\\large %s}\\\\[0.3em]\n", header->title);
	}
	if (header->professor != NULL) {
		_line("{\\normalsize %s}\\\\[0.3em]\n", header->professor);
	}
	if (header->date != NULL) {
		_line("{\\normalsize %s}\n", header->date);
	}
	_indent--;
	_line("\\end{center}\n");
	_line("\n");

	if (header->score_grid == 1) {
		_line("\\begin{center}\n");
		_indent++;
		_line("\\gradetable[h][questions]\n");
		_indent--;
		_line("\\end{center}\n");
		_line("\n");
	}

	if (header->instructions != NULL || header->duration >= 0) {
		_line("\\noindent\\textbf{IMPORTANTE:}\n");
		_line("\\begin{itemize}[leftmargin=1.5em]\n");
		_indent++;
		if (header->duration >= 0) {
			_line("\\item Duraci\\'{o}n: %d minutos.\n", header->duration);
		}
		if (header->instructions != NULL) {
			_line("\\item %s\n", header->instructions);
		}
		_indent--;
		_line("\\end{itemize}\n");
		_line("\n");
	}
}

/* PUBLIC FUNCTIONS */

void executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	Program * program = compilerState->abstractSyntaxtTree;

	_generatePreamble(program->header);
	_generateHeaderBlock(program->header);
	_generateItems(program->items);
	_generateEpilogue();

	logDebugging(_logger, "Generation is done.");
}
