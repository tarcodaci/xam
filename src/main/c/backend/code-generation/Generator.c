#include "Generator.h"
#include <time.h>

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
static FILE * _output = NULL;

static void _line(const char * format, ...) {
	for (int i = 0; i < _indent; i++) fprintf(_output, "\t");
	va_list args;
	va_start(args, format);
	vfprintf(_output, format, args);
	va_end(args);
}

/* Continue on the current line without indentation. */
static void _append(const char * format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(_output, format, args);
	va_end(args);
}

/* Fisher-Yates shuffle for an array of char pointers. */
static void _shuffleStrings(char ** array, int n) {
	for (int i = n - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		char * tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
	}
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

static void _generateAnswerSheet(Header * header, Item * items) {
	if (header == NULL || header->answer_sheet != 1) return;

	_line("\\documentclass[11pt, a4paper]{article}\n");
	_line("\\usepackage[utf8]{inputenc}\n");
	_line("\\usepackage[T1]{fontenc}\n");
	_line("\\usepackage[spanish]{babel}\n");
	_line("\\usepackage[margin=2.5cm]{geometry}\n");
	_line("\\usepackage{amsmath, amssymb}\n");
	_line("\\usepackage{enumitem}\n");
	_line("\n");
	_line("\\begin{document}\n");
	_line("\\sloppy\n");
	_line("\n");
	_line("\\begin{center}\n");
	_indent++;
	_line("{\\Large\\bfseries Hoja de Respuestas}\n");
	if (header->title != NULL) {
		_line("{\\normalsize %s}\n", header->title);
	}
	_indent--;
	_line("\\end{center}\n");
	_line("\\vspace{1em}\n");
	_line("\n");
	_line("\\begin{enumerate}\n");
	_indent++;
	while (items != NULL) {
		if (items->type == ITEM_EXERCISE) {
			Exercise * ex = items->exercise;
			switch (ex->type) {
				case EXERCISE_MULTIPLECHOICE: {
					MultiplechoiceOption * opt = ex->multiplechoice->options;
					_line("\\item ");
					while (opt != NULL) {
						if (opt->is_correct) { _append("\\textbf{%s}", opt->value); break; }
						opt = opt->next;
					}
					_append("\n");
					break;
				}
				case EXERCISE_TRUEORFALSE:
					if (ex->trueorfalse->answer == 1) {
						_line("\\item Verdadero\n");
					} else if (ex->trueorfalse->answer == 0) {
						_line("\\item Falso\n");
					} else {
						_line("\\item ---\n");
					}
					break;
				case EXERCISE_OPENQUESTION:
					if (ex->openquestion->answer != NULL) {
						_line("\\item %s\n", ex->openquestion->answer);
					} else {
						_line("\\item ---\n");
					}
					break;
				case EXERCISE_FILLBLANKS:
					if (ex->fillblanks->answers != NULL) {
						_line("\\item ");
						StringList * ans = ex->fillblanks->answers;
						int i = 1;
						while (ans != NULL) {
							_append("%d) %s ", i++, ans->value);
							ans = ans->next;
						}
						_append("\n");
					} else {
						_line("\\item ---\n");
					}
					break;
				case EXERCISE_CHOOSEFROM:
					if (ex->choosefrom->answer != NULL) {
						_line("\\item ");
						IntList * idx = ex->choosefrom->answer;
						int i = 1;
						while (idx != NULL) {
							StringList * opt = ex->choosefrom->options;
							int pos = 1;
							while (opt != NULL && pos < idx->value) { opt = opt->next; pos++; }
							if (opt != NULL) _append("%d) %s ", i, opt->value);
							i++;
							idx = idx->next;
						}
						_append("\n");
					} else {
						_line("\\item ---\n");
					}
					break;
				case EXERCISE_MATCH: {
					_line("\\item ");
					MatchPair * mp = ex->match->pairs;
					while (mp != NULL) {
						_append("%s $\\rightarrow$ %s", mp->left, mp->right);
						if (mp->next != NULL) _append(", ");
						mp = mp->next;
					}
					_append("\n");
					break;
				}
				case EXERCISE_CHART:
					if (ex->chart->answer != NULL) {
						_line("\\item ~\\\\\n");
						_line("\\begin{tabular}{|");
						for (int c = 0; c < ex->chart->dim_cols; c++) _append("c|");
						_append("}\n");
						_line("\\hline\n");
						StringList * val = ex->chart->answer;
						for (int r = 0; r < ex->chart->dim_rows; r++) {
							_line("");
							for (int c = 0; c < ex->chart->dim_cols; c++) {
								if (val != NULL) { _append("%s", val->value); val = val->next; }
								if (c < ex->chart->dim_cols - 1) _append(" & ");
							}
							_append(" \\\\\n");
							_line("\\hline\n");
						}
						_line("\\end{tabular}\n");
					} else {
						_line("\\item ---\n");
					}
					break;
				default:
					_line("\\item ---\n");
					break;
			}
		}
		items = items->next;
	}
	_indent--;
	_line("\\end{enumerate}\n");
	_line("\n");
	_line("\\end{document}\n");
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
		_line("\\question[%d] %s\\\\[0.5em]\n", node->score, node->question);
	} else {
		_line("\\question %s\\\\[0.5em]\n", node->question);
	}
	/* Collect right column values and shuffle them */
	int count = 0;
	MatchPair * p = node->pairs;
	while (p != NULL) { count++; p = p->next; }

	char * rights[count];
	p = node->pairs;
	for (int i = 0; i < count; i++) { rights[i] = p->right; p = p->next; }
	_shuffleStrings(rights, count);

	_line("\\begin{tabular}{l@{\\hspace{4cm}}l}\n");
	_indent++;
	p = node->pairs;
	for (int i = 0; i < count; i++) {
		_line("%s & %s \\\\\n", p->left, rights[i]);
		p = p->next;
	}
	_indent--;
	_line("\\end{tabular}\n");
	_line("\n");
}

static void _generateSetExercise(Exercise * ex) {
	switch (ex->type) {
		case EXERCISE_MULTIPLECHOICE: _generateMultiplechoice(ex->multiplechoice, "\\part"); break;
		case EXERCISE_TRUEORFALSE: _generateTrueorfalse(ex->trueorfalse, "\\part"); break;
		case EXERCISE_OPENQUESTION: _generateOpenquestion(ex->openquestion, "\\part"); break;
		case EXERCISE_FILLBLANKS: _generateFillblanks(ex->fillblanks, "\\part"); break;
		case EXERCISE_CHOOSEFROM: _generateChoosefrom(ex->choosefrom, "\\part"); break;
		case EXERCISE_MATCH: _generateMatch(ex->match); break;
		default: break;
	}
}

static void _generateSet(SetNode * node) {
	_emitQuestionPrefix("\\question", node->score);
	if (node->text != NULL) {
		_append("%s", node->text);
	}
	_append("\n");
	_line("\\begin{parts}\n");
	_indent++;

	/* Collect exercises into array */
	int count = 0;
	Exercise * ex = node->exercises;
	while (ex != NULL) { count++; ex = ex->next; }

	Exercise * exercises[count];
	ex = node->exercises;
	for (int i = 0; i < count; i++) { exercises[i] = ex; ex = ex->next; }

	/* Shuffle if enabled */
	if (node->shuffle == 1) {
		for (int i = count - 1; i > 0; i--) {
			int j = rand() % (i + 1);
			Exercise * tmp = exercises[i];
			exercises[i] = exercises[j];
			exercises[j] = tmp;
		}
	}

	for (int i = 0; i < count; i++) {
		_generateSetExercise(exercises[i]);
	}

	_indent--;
	_line("\\end{parts}\n\n");
}

static void _emitChartTable(ChartNode * node) {
	_line("\\begin{center}\n");
	_indent++;
	_line("\\begin{tabular}{|");
	for (int c = 0; c < node->dim_cols; c++) _append("c|");
	_append("}\n");
	_line("\\hline\n");
	for (int r = 1; r <= node->dim_rows; r++) {
		_line("");
		for (int c = 1; c <= node->dim_cols; c++) {
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

static void _generateFillchart(ChartNode * node) {
	if (node->score >= 0) {
		_line("\\question[%d] %s\n", node->score, node->task);
	} else {
		_line("\\question %s\n", node->task);
	}
	_emitChartTable(node);
}

static void _generateChart(ChartNode * node) {
	if (node->task != NULL && node->task[0] != '\0') {
		_line("\\uplevel{%s}\n", node->task);
	}
	_emitChartTable(node);
}

static void _generateExercise(Exercise * exercise) {
	switch (exercise->type) {
		case EXERCISE_OPENQUESTION: _generateOpenquestion(exercise->openquestion, "\\question"); break;
		case EXERCISE_MULTIPLECHOICE: _generateMultiplechoice(exercise->multiplechoice, "\\question"); break;
		case EXERCISE_TRUEORFALSE: _generateTrueorfalse(exercise->trueorfalse, "\\question"); break;
		case EXERCISE_FILLBLANKS: _generateFillblanks(exercise->fillblanks, "\\question"); break;
		case EXERCISE_CHOOSEFROM: _generateChoosefrom(exercise->choosefrom, "\\question"); break;
		case EXERCISE_MATCH: _generateMatch(exercise->match); break;
		case EXERCISE_CHART: _generateFillchart(exercise->chart); break;
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
		} else if (items->type == ITEM_CHART) {
			_generateChart(items->chart);
		} else if (items->type == ITEM_SECTION) {
			SectionNode * sec = items->section;
			if (sec->select > 0) {
				/* Collect items into array, shuffle, take first N */
				int count = 0;
				Item * it = sec->items;
				while (it != NULL) { count++; it = it->next; }
				int take = sec->select < count ? sec->select : count;

				Item * arr[count];
				it = sec->items;
				for (int i = 0; i < count; i++) { arr[i] = it; it = it->next; }
				
				for (int i = count - 1; i > 0; i--) {
					int j = rand() % (i + 1);
					Item * tmp = arr[i];
					arr[i] = arr[j];
					arr[j] = tmp;
				}
				/* Generate only the selected items */
				for (int i = 0; i < take; i++) {
					if (arr[i]->type == ITEM_EXERCISE) {
						_generateExercise(arr[i]->exercise);
					} else if (arr[i]->type == ITEM_TEXT) {
						_line("\\uplevel{%s}\n", arr[i]->text);
					}
				}
			} else {
				/* No select — render all items */
				Item * it = sec->items;
				while (it != NULL) {
					if (it->type == ITEM_EXERCISE) {
						_generateExercise(it->exercise);
					} else if (it->type == ITEM_TEXT) {
						_line("\\uplevel{%s}\n", it->text);
					}
					it = it->next;
				}
			}
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
	srand(time(NULL));
	Program * program = compilerState->abstractSyntaxtTree;

	const char * base = compilerState->inputFilename;
	const char * dot = strrchr(base, '.');
	if (dot == NULL || strcmp(dot, ".xam") != 0) {
		logError(_logger, "Input file must have .xam extension.");
		return;
	}
	int baseLen = (int)(dot - base);

	char examFile[256];
	memcpy(examFile, base, baseLen);
	strcpy(examFile + baseLen, ".tex");

	char answersFile[256];
	memcpy(answersFile, base, baseLen);
	strcpy(answersFile + baseLen, "-answers.tex");

	_output = fopen(examFile, "w");
	if (_output == NULL) {
		logError(_logger, "Cannot create %s", examFile);
		return;
	}

	_indent = 0;
	_generatePreamble(program->header);
	_generateHeaderBlock(program->header);
	_generateItems(program->items);
	_generateEpilogue();
	fclose(_output);
	logDebugging(_logger, "Wrote %s", examFile);

	if (program->header != NULL && program->header->answer_sheet == 1) {
		_output = fopen(answersFile, "w");
		if (_output == NULL) {
			logError(_logger, "Cannot create %s", answersFile);
			return;
		}
		_indent = 0;
		_generateAnswerSheet(program->header, program->items);
		fclose(_output);
		logDebugging(_logger, "Wrote %s", answersFile);
	}

	_output = NULL;
	logDebugging(_logger, "Generation is done.");
}
