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
	McNode * mcNode;
	TorfNode * torfNode;
	DirectNode * directNode;
	BlanksNode * blanksNode;
	ChooseFromNode * chooseFromNode;
	MatchNode * matchNode;
	ChartNode * chartNode;
	SetNode * setNode;
	SectionNode * sectionNode;
	StringList * stringList;
	IntList * intList;
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

/** Terminals. */
%token <integer> INTEGER
%token <string> STRING
%token <string> IDENTIFIER
%token <token> TRUE
%token <token> FALSE
%token <token> CLOSE_BRACE
%token <token> CLOSE_BRACKET
%token <token> CLOSE_COMMENT
%token <token> CLOSE_PARENTHESIS
%token <token> COLON
%token <token> COMMA
%token <token> HASH
%token <token> OPEN_BRACE
%token <token> OPEN_BRACKET
%token <token> OPEN_COMMENT
%token <token> OPEN_PARENTHESIS
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

/** Non-terminals. */
%type <header> header
%type <header> header_props
%type <headerProperty> header_prop
%type <item> items
%type <item> item
%type <mcNode> mc_block
%type <mcNode> mc_props
%type <torfNode> torf_block
%type <torfNode> torf_props
%type <directNode> direct_block
%type <directNode> direct_props
%type <blanksNode> blanks_block
%type <blanksNode> blanks_props
%type <chooseFromNode> choose_from_block
%type <chooseFromNode> choose_from_props
%type <matchNode> match_block
%type <matchNode> match_props
%type <chartNode> chart_block
%type <chartNode> chart_props
%type <setNode> set_block
%type <setNode> set_props
%type <sectionNode> section_block
%type <sectionNode> section_props
%type <stringList> string_list
%type <stringList> string_items
%type <intList> int_list
%type <intList> int_items
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

item: mc_block							{ $$ = ExerciseItemSemanticAction(EXERCISE_MC, $1); }
	| torf_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_TORF, $1); }
	| direct_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_DIRECT, $1); }
	| blanks_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_BLANKS, $1); }
	| choose_from_block					{ $$ = ExerciseItemSemanticAction(EXERCISE_CHOOSE_FROM, $1); }
	| match_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_MATCH, $1); }
	| chart_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_CHART, $1); }
	| set_block							{ $$ = ExerciseItemSemanticAction(EXERCISE_SET, $1); }
	| section_block						{ $$ = SectionItemSemanticAction($1); }
	| TEXT_KW COLON STRING SEMICOLON	{ $$ = TextItemSemanticAction($3); }
	;

/* ── mc ── */

mc_block: MC OPEN_BRACE mc_props CLOSE_BRACE			{ $$ = $3; }
	;

mc_props: mc_props QUESTION COLON STRING SEMICOLON		{ $$ = McSetQuestionSemanticAction($1, $4); }
	| mc_props OPTION COLON STRING SEMICOLON			{ $$ = McAddOptionSemanticAction($1, $4, 0); }
	| mc_props OPTION COLON STRING HASH SEMICOLON		{ $$ = McAddOptionSemanticAction($1, $4, 1); }
	| mc_props OPTION COLON INTEGER SEMICOLON			{ $$ = McAddIntOptionSemanticAction($1, $4, 0); }
	| mc_props OPTION COLON INTEGER HASH SEMICOLON		{ $$ = McAddIntOptionSemanticAction($1, $4, 1); }
	| mc_props SCORE COLON INTEGER SEMICOLON			{ $$ = McSetScoreSemanticAction($1, $4); }
	| %empty											{ $$ = CreateMcNodeSemanticAction(); }
	;

/* ── torf ── */

torf_block: TORF OPEN_BRACE torf_props CLOSE_BRACE		{ $$ = $3; }
	;

torf_props: torf_props QUESTION COLON STRING SEMICOLON	{ $$ = TorfSetQuestionSemanticAction($1, $4); }
	| torf_props JUSTIFY COLON TRUE SEMICOLON			{ $$ = TorfSetJustifySemanticAction($1, 1); }
	| torf_props JUSTIFY COLON FALSE SEMICOLON			{ $$ = TorfSetJustifySemanticAction($1, 0); }
	| torf_props ANSWER COLON TRUE SEMICOLON			{ $$ = TorfSetAnswerSemanticAction($1, 1); }
	| torf_props ANSWER COLON FALSE SEMICOLON			{ $$ = TorfSetAnswerSemanticAction($1, 0); }
	| torf_props SCORE COLON INTEGER SEMICOLON			{ $$ = TorfSetScoreSemanticAction($1, $4); }
	| %empty											{ $$ = CreateTorfNodeSemanticAction(); }
	;

/* ── direct ── */

direct_block: DIRECT OPEN_BRACE direct_props CLOSE_BRACE	{ $$ = $3; }
	;

direct_props: direct_props QUESTION COLON STRING SEMICOLON	{ $$ = DirectSetQuestionSemanticAction($1, $4); }
	| direct_props LINES COLON INTEGER SEMICOLON			{ $$ = DirectSetLinesSemanticAction($1, $4); }
	| direct_props ANSWER COLON STRING SEMICOLON			{ $$ = DirectSetAnswerSemanticAction($1, $4); }
	| direct_props SCORE COLON INTEGER SEMICOLON			{ $$ = DirectSetScoreSemanticAction($1, $4); }
	| %empty												{ $$ = CreateDirectNodeSemanticAction(); }
	;

/* ── blanks ── */

blanks_block: BLANKS OPEN_BRACE blanks_props CLOSE_BRACE	{ $$ = $3; }
	;

blanks_props: blanks_props QUESTION COLON STRING SEMICOLON	{ $$ = BlanksSetQuestionSemanticAction($1, $4); }
	| blanks_props ANSWER COLON STRING SEMICOLON			{ $$ = BlanksAddAnswerSemanticAction($1, $4); }
	| blanks_props SCORE COLON INTEGER SEMICOLON			{ $$ = BlanksSetScoreSemanticAction($1, $4); }
	| %empty												{ $$ = CreateBlanksNodeSemanticAction(); }
	;

/* ── match ── */

match_block: MATCH OPEN_BRACE match_props CLOSE_BRACE	{ $$ = $3; }
	;

match_props: match_props QUESTION COLON STRING SEMICOLON										{ $$ = MatchSetQuestionSemanticAction($1, $4); }
	| match_props PAIR COLON OPEN_BRACKET STRING COMMA STRING CLOSE_BRACKET SEMICOLON			{ $$ = MatchAddPairSemanticAction($1, $5, $7); }
	| match_props SCORE COLON INTEGER SEMICOLON													{ $$ = MatchSetScoreSemanticAction($1, $4); }
	| %empty																					{ $$ = CreateMatchNodeSemanticAction(); }
	;

/* ── chart ── */

chart_block: CHART OPEN_BRACE chart_props CLOSE_BRACE	{ $$ = $3; }
	;

chart_props: chart_props TASK COLON STRING SEMICOLON												{ $$ = ChartSetTaskSemanticAction($1, $4); }
	| chart_props DIM COLON OPEN_BRACKET INTEGER COMMA INTEGER CLOSE_BRACKET SEMICOLON				{ $$ = ChartSetDimSemanticAction($1, $5, $7); }
	| chart_props OPEN_PARENTHESIS INTEGER COMMA INTEGER CLOSE_PARENTHESIS COLON STRING SEMICOLON	{ $$ = ChartAddCellSemanticAction($1, $3, $5, $8); }
	| chart_props ANSWER COLON string_list SEMICOLON												{ $$ = ChartSetAnswerSemanticAction($1, $4); }
	| chart_props SCORE COLON INTEGER SEMICOLON														{ $$ = ChartSetScoreSemanticAction($1, $4); }
	| %empty																						{ $$ = CreateChartNodeSemanticAction(); }
	;

/* ── chooseFrom ── */

choose_from_block: CHOOSE_FROM OPEN_BRACE choose_from_props CLOSE_BRACE		{ $$ = $3; }
	;

choose_from_props: choose_from_props TASK COLON STRING SEMICOLON			{ $$ = ChooseFromSetTaskSemanticAction($1, $4); }
	| choose_from_props OPTIONS COLON string_list SEMICOLON					{ $$ = ChooseFromSetOptionsSemanticAction($1, $4); }
	| choose_from_props TEXT_KW COLON STRING SEMICOLON						{ $$ = ChooseFromSetTextSemanticAction($1, $4); }
	| choose_from_props ANSWER COLON int_list SEMICOLON						{ $$ = ChooseFromSetAnswerSemanticAction($1, $4); }
	| choose_from_props SCORE COLON INTEGER SEMICOLON						{ $$ = ChooseFromSetScoreSemanticAction($1, $4); }
	| choose_from_props SHUFFLE COLON TRUE SEMICOLON						{ $$ = ChooseFromSetShuffleSemanticAction($1, 1); }
	| choose_from_props SHUFFLE COLON FALSE SEMICOLON						{ $$ = ChooseFromSetShuffleSemanticAction($1, 0); }
	| %empty																{ $$ = CreateChooseFromNodeSemanticAction(); }
	;

/* ── set ── */

set_block: SET OPEN_BRACE set_props CLOSE_BRACE								{ $$ = $3; }
	;

set_props: set_props TEXT_KW COLON STRING SEMICOLON							{ $$ = SetSetTextSemanticAction($1, $4); }
	| set_props SCORE COLON INTEGER SEMICOLON								{ $$ = SetSetScoreSemanticAction($1, $4); }
	| set_props mc_block													{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_MC, $2); }
	| set_props torf_block													{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_TORF, $2); }
	| set_props direct_block												{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_DIRECT, $2); }
	| set_props blanks_block												{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_BLANKS, $2); }
	| set_props choose_from_block											{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_CHOOSE_FROM, $2); }
	| set_props match_block													{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_MATCH, $2); }
	| set_props chart_block													{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_CHART, $2); }
	| %empty																{ $$ = CreateSetNodeSemanticAction(); }
	;

/* ── section ── */

section_block: SECTION IDENTIFIER OPEN_BRACE section_props CLOSE_BRACE		{ $$ = SectionSetNameSemanticAction($4, $2); }
	;

section_props: section_props SELECT COLON INTEGER SEMICOLON					{ $$ = SectionSetSelectSemanticAction($1, $4); }
	| section_props item													{ $$ = SectionAddItemSemanticAction($1, $2); }
	| %empty																{ $$ = CreateSectionNodeSemanticAction(); }
	;

/* ── lists ── */

string_list: OPEN_BRACKET string_items CLOSE_BRACKET		{ $$ = $2; }
	;

string_items: STRING										{ $$ = CreateStringListSemanticAction($1); }
	| string_items COMMA STRING								{ $$ = AppendStringListSemanticAction($1, $3); }
	;

int_list: OPEN_BRACKET int_items CLOSE_BRACKET				{ $$ = $2; }
	;

int_items: INTEGER											{ $$ = CreateIntListSemanticAction($1); }
	| int_items COMMA INTEGER								{ $$ = AppendIntListSemanticAction($1, $3); }
	;

%%
