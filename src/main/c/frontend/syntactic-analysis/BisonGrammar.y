%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
 void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	signed int integer;
	char * string;
	TokenLabel token;

	Header * header;
	HeaderProperty * headerProperty;
	Item * item;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
 %destructor { free($$); } <string>

%token <integer> INTEGER
%token <string> STRING
%token <string> IDENTIFIER
%token <token> TRUE
%token <token> FALSE
%token <token> CLOSE_BRACE
%token <token> CLOSE_BRACKET
%token <token> CLOSE_COMMENT
%token <token> COLON
%token <token> COMMA
%token <token> HASH
%token <token> OPEN_BRACE
%token <token> OPEN_BRACKET
%token <token> OPEN_COMMENT
%token <token> SEMICOLON

%token <token> HEADER
%token <token> MC
%token <token> BLANKS
%token <token> DIRECT
%token <token> CHOOSE_FROM
%token <token> MATCH
%token <token> TORF
%token <token> CHART
%token <token> SET
%token <token> SECTION

%token <token> TITLE
%token <token> SUBJECT
%token <token> DATE
%token <token> DURATION
%token <token> SCORE_GRID
%token <token> ANSWER_SHEET
%token <token> STUDENT_NAME
%token <token> STUDENT_ID
%token <token> COURSE
%token <token> INSTRUCTIONS

%token <token> QUESTION
%token <token> OPTION
%token <token> SCORE
%token <token> ANSWER
%token <token> LINES
%token <token> TASK
%token <token> OPTIONS
%token <token> SHUFFLE
%token <token> PAIR
%token <token> JUSTIFY
%token <token> DIM
%token <token> CELL
%token <token> SELECT
%token <token> TEXT_KW

%token <token> IGNORED
%token <token> UNKNOWN

%type <header> header
%type <header> header_props
%type <headerProperty> header_prop
%type <item> items
%type <item> item
%type <program> program

%%

program: header items					{ $$ = ProgramWithHeaderSemanticAction($1, $2); }
	| items								{ $$ = ProgramSemanticAction($1); }
	;

/* ── header ── */

header: HEADER OPEN_BRACE header_props CLOSE_BRACE		{ $$ = $3; }
	;

header_props: header_prop								{ $$ = CreateHeaderSemanticAction($1); }
	| header_props header_prop							{ $$ = AddHeaderPropertySemanticAction($1, $2); }
	;

header_prop: TITLE COLON STRING SEMICOLON				{ $$ = HeaderPropertySemanticAction(HEADER_PROP_TITLE, $3, 0); }
	| SUBJECT COLON STRING SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_SUBJECT, $3, 0); }
	| DATE COLON STRING SEMICOLON						{ $$ = HeaderPropertySemanticAction(HEADER_PROP_DATE, $3, 0); }
	| DURATION COLON INTEGER SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_DURATION, NULL, $3); }
	| SCORE_GRID COLON TRUE SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_SCORE_GRID, NULL, 1); }
	| SCORE_GRID COLON FALSE SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_SCORE_GRID, NULL, 0); }
	| ANSWER_SHEET COLON TRUE SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_ANSWER_SHEET, NULL, 1); }
	| ANSWER_SHEET COLON FALSE SEMICOLON				{ $$ = HeaderPropertySemanticAction(HEADER_PROP_ANSWER_SHEET, NULL, 0); }
	| STUDENT_NAME COLON TRUE SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_STUDENT_NAME, NULL, 1); }
	| STUDENT_NAME COLON FALSE SEMICOLON				{ $$ = HeaderPropertySemanticAction(HEADER_PROP_STUDENT_NAME, NULL, 0); }
	| STUDENT_ID COLON TRUE SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_STUDENT_ID, NULL, 1); }
	| STUDENT_ID COLON FALSE SEMICOLON					{ $$ = HeaderPropertySemanticAction(HEADER_PROP_STUDENT_ID, NULL, 0); }
	| COURSE COLON TRUE SEMICOLON						{ $$ = HeaderPropertySemanticAction(HEADER_PROP_COURSE, NULL, 1); }
	| COURSE COLON FALSE SEMICOLON						{ $$ = HeaderPropertySemanticAction(HEADER_PROP_COURSE, NULL, 0); }
	| INSTRUCTIONS COLON STRING SEMICOLON				{ $$ = HeaderPropertySemanticAction(HEADER_PROP_INSTRUCTIONS, $3, 0); }
	;

/* ── items ── */

items: items item						{ $$ = AppendItemSemanticAction($1, $2); }
	| %empty							{ $$ = NULL; }
	;

item: TEXT_KW COLON STRING SEMICOLON	{ $$ = TextItemSemanticAction($3); }
	;

%%
