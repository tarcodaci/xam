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
	_line("\\renewcommand{\\questionlabel}{\\textbf{\\thequestion.}}\n");
	_line("\\renewcommand{\\totalformat}{Total}\n");
	_line("\\hqword{Ejercicio}\n");
	_line("\\hpword{Puntos}\n");
	_line("\\hsword{Nota}\n");
	_line("\\htword{Total}\n");
	_line("\n");
	_line("\\pagestyle{headandfoot}\n");
	if (header != NULL && (header->student_name == 1 || header->student_id == 1 || header->course == 1)) {
		_line("\\header{");
		if (header->student_name == 1) _append("Nombre y Apellido:\\rule{5cm}{0.4pt}\\hspace{1em} ");
		if (header->student_id == 1) _append("Legajo:\\rule{1.5cm}{0.4pt}\\hspace{1em} ");
		if (header->course == 1) _append("Curso:\\rule{1.5cm}{0.4pt}\\hspace{1em}");
		_append("}{}{\\thepage}\n");
	} else {
		_line("\\header{}{}{\\thepage}\n");
	}
	_line("\\footer{}{}{}\n");
	_line("\n");
	_line("\\begin{document}\n");
	_line("\\sloppy\n");
	_line("\n");
}

static void _generateEpilogue() {
	_line("\\end{document}\n");
}

static void _emitQuestionPrefix(const char * prefix, int score) {
	if (score >= 0) {
		_line("%s[%d] ", prefix, score);
	} else {
		_line("%s ", prefix);
	}
}

static void _generateOpenquestion(OpenquestionNode * node, const char * prefix) {
	_emitQuestionPrefix(prefix, node->score);
	_append("%s\n", node->question);
	int lines = node->lines >= 0 ? node->lines : 3;
	if (lines > 0) {
		_line("\\fillwithlines{%d\\baselineskip}\n", lines);
	}
	_line("\n");
}

static void _generateMultiplechoice(MultiplechoiceNode * node, const char * prefix) {
	_emitQuestionPrefix(prefix, node->score);
	_append("%s\n", node->question);
	_line("\\begin{choices}\n");
	_indent++;
	MultiplechoiceOption * opt = node->options;
	while (opt != NULL) {
		if (opt->is_correct) {
			_line("\\CorrectChoice %s\n", opt->value);
		} else {
			_line("\\choice %s\n", opt->value);
		}
		opt = opt->next;
	}
	_indent--;
	_line("\\end{choices}\n");
	_line("\n");
}

static void _generateTrueorfalse(TrueorfalseNode * node, const char * prefix) {
	_emitQuestionPrefix(prefix, node->score);
	_append("%s\n", node->question);
	_line("\\begin{choices}\n");
	_indent++;
	_line("\\choice Verdadero\n");
	_line("\\choice Falso\n");
	_indent--;
	_line("\\end{choices}\n");
	if (node->justify == 1) {
		_line("Justifique:\\fillwithlines{2\\baselineskip}\n");
	}
	_line("\n");
}

static void _emitTextWithBlanks(const char * text) {
	const char * p = text;
	while (*p != '\0') {
		if (*p == '_') {
			int count = 0;
			while (*p == '_') { count++; p++; }
			_append("\\underline{\\hspace{%dcm}}", count);
		} else {
			_append("%c", *p);
			p++;
		}
	}
}

static void _generateFillblanks(FillblanksNode * node, const char * prefix) {
	_emitQuestionPrefix(prefix, node->score);
	_emitTextWithBlanks(node->question);
	_append("\n\n");
}

static void _generateChoosefrom(ChoosefromNode * node, const char * prefix) {
	_emitQuestionPrefix(prefix, node->score);
	if (node->task != NULL) {
		_append("%s\n", node->task);
	} else {
		_append("\n");
	}
	_line("\\textbf{Opciones:} ");
	StringList * opt = node->options;
	while (opt != NULL) {
		_append("%s", opt->value);
		if (opt->next != NULL) _append(" -- ");
		opt = opt->next;
	}
	_append("\\\\[0.5em]\n");
	if (node->text != NULL) {
		_line("");
		_emitTextWithBlanks(node->text);
		_append("\n");
	}
	_line("\n");
}

static void _generateMatch(MatchNode * node) {
	if (node->score >= 0) {
		_line("\\question[%d] %s\n", node->score, node->question);
	} else {
		_line("\\question %s\n", node->question);
	}
	_line("\\begin{tabular}{r@{\\hspace{2cm}}l}\n");
	_indent++;
	MatchPair * pair = node->pairs;
	while (pair != NULL) {
		_line("%s & \\rule{3cm}{0.4pt} \\\\\n", pair->left);
		pair = pair->next;
	}
	_indent--;
	_line("\\end{tabular}\n");
	_line("\\vspace{0.5em}\n");
	_line("\\\\\\textbf{Columna B:} ");
	pair = node->pairs;
	while (pair != NULL) {
		_append("%s", pair->right);
		if (pair->next != NULL) _append(" -- ");
		pair = pair->next;
	}
	_append("\n\n");
}

static void _generateSet(SetNode * node) {
	_emitQuestionPrefix("\\question", node->score);
	if (node->text != NULL) {
		_append("%s", node->text);
	}
	_append("\n");
	_line("\\begin{parts}\n");
	_indent++;
	Exercise * ex = node->exercises;
	while (ex != NULL) {
		switch (ex->type) {
			case EXERCISE_MULTIPLECHOICE: _generateMultiplechoice(ex->multiplechoice, "\\part"); break;
			case EXERCISE_TRUEORFALSE: _generateTrueorfalse(ex->trueorfalse, "\\part"); break;
			case EXERCISE_OPENQUESTION: _generateOpenquestion(ex->openquestion, "\\part"); break;
			case EXERCISE_FILLBLANKS: _generateFillblanks(ex->fillblanks, "\\part"); break;
			case EXERCISE_CHOOSEFROM: _generateChoosefrom(ex->choosefrom, "\\part"); break;
			case EXERCISE_MATCH: _generateMatch(ex->match); break;
			default: break;
		}
		ex = ex->next;
	}
	_indent--;
	_line("\\end{parts}\n\n");
}

static void _generateChart(ChartNode * node) {
	if (node->score >= 0) {
		_line("\\question[%d] %s\n", node->score, node->task);
	} else {
		_line("\\question %s\n", node->task);
	}
	_line("\\begin{center}\n");
	_indent++;
	_line("\\begin{tabular}{|");
	for (int c = 0; c < node->dim_cols; c++) _append("c|");
	_append("}\n");
	_line("\\hline\n");
	for (int r = 1; r <= node->dim_rows; r++) {
		_line("");
		for (int c = 1; c <= node->dim_cols; c++) {
			/* Check if this cell is pre-filled */
			char * value = NULL;
			ChartCell * cell = node->cells;
			while (cell != NULL) {
				if (cell->row == r && cell->col == c) { value = cell->value; break; }
				cell = cell->next;
			}
			if (value != NULL) _append("%s", value);
			else _append("\\hspace{2cm}");
			if (c < node->dim_cols) _append(" & ");
		}
		_append(" \\\\\n");
		_line("\\hline\n");
	}
	_indent--;
	_line("\\end{tabular}\n");
	_line("\\end{center}\n");
	_line("\n");
}

static void _generateExercise(Exercise * exercise) {
	switch (exercise->type) {
		case EXERCISE_OPENQUESTION: _generateOpenquestion(exercise->openquestion, "\\question"); break;
		case EXERCISE_MULTIPLECHOICE: _generateMultiplechoice(exercise->multiplechoice, "\\question"); break;
		case EXERCISE_TRUEORFALSE: _generateTrueorfalse(exercise->trueorfalse, "\\question"); break;
		case EXERCISE_FILLBLANKS: _generateFillblanks(exercise->fillblanks, "\\question"); break;
		case EXERCISE_CHOOSEFROM: _generateChoosefrom(exercise->choosefrom, "\\question"); break;
		case EXERCISE_MATCH: _generateMatch(exercise->match); break;
		case EXERCISE_CHART: _generateChart(exercise->chart); break;
		case EXERCISE_SET: _generateSet(exercise->set); break;
		default: break;
	}
}

static void _generateImage(ImageNode * node) {
	_line("\\begin{center}\n");
	_indent++;
	int width = node->width > 0 ? node->width : 100;
	_line("\\includegraphics[width=0.%d\\textwidth]{%s}\n", width, node->path);
	if (node->caption != NULL) {
		_line("\\\\{\\small %s}\n", node->caption);
	}
	_indent--;
	_line("\\end{center}\n");
	_line("\n");
}

static void _generateItems(Item * items) {
	_line("\\noindent\\textbf{Ejercicios}\n");
	_line("\\begin{questions}\n");
	_line("\n");
	_indent++;
	while (items != NULL) {
		if (items->type == ITEM_EXERCISE) {
			_generateExercise(items->exercise);
		} else if (items->type == ITEM_TEXT) {
			_line("\\uplevel{%s}\n", items->text);
		} else if (items->type == ITEM_IMAGE) {
			_indent--;
			_line("\\end{questions}\n");
			_generateImage(items->image);
			_line("\\begin{questions}\n");
			_indent++;
		} else if (items->type == ITEM_SECTION) {
			_generateItems(items->section->items);
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
		StringList * instr = header->instructions;
		while (instr != NULL) {
			_line("\\item %s\n", instr->value);
			instr = instr->next;
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
