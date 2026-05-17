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
	MultiplechoiceNode * multiplechoiceNode;
	TrueorfalseNode * trueorfalseNode;
	OpenquestionNode * openquestionNode;
	FillblanksNode * fillblanksNode;
	ChoosefromNode * choosefromNode;
	MatchNode * matchNode;
	ChartNode * chartNode;
	SetNode * setNode;
	SectionNode * sectionNode;
	ImageNode * imageNode;
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
%token <token> MULTIPLECHOICE
%token <token> FILLBLANKS
%token <token> OPENQUESTION
%token <token> CHOOSEFROM
%token <token> MATCH
%token <token> TRUEORFALSE
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

%token <token> IMAGE
%token <token> PATH
%token <token> CAPTION
%token <token> WIDTH

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <header> header
%type <header> header_props
%type <headerProperty> header_prop
%type <item> items
%type <item> item
%type <multiplechoiceNode> multiplechoice_block
%type <multiplechoiceNode> multiplechoice_props
%type <trueorfalseNode> trueorfalse_block
%type <trueorfalseNode> trueorfalse_props
%type <openquestionNode> openquestion_block
%type <openquestionNode> openquestion_props
%type <fillblanksNode> fillblanks_block
%type <fillblanksNode> fillblanks_props
%type <choosefromNode> choosefrom_block
%type <choosefromNode> choosefrom_props
%type <matchNode> match_block
%type <matchNode> match_props
%type <chartNode> chart_block
%type <chartNode> chart_props
%type <setNode> set_block
%type <setNode> set_props
%type <sectionNode> section_block
%type <sectionNode> section_props
%type <imageNode> image_block
%type <imageNode> image_props
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

item: multiplechoice_block							{ $$ = ExerciseItemSemanticAction(EXERCISE_MULTIPLECHOICE, $1); }
	| trueorfalse_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_TRUEORFALSE, $1); }
	| openquestion_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_OPENQUESTION, $1); }
	| fillblanks_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_FILLBLANKS, $1); }
	| choosefrom_block					{ $$ = ExerciseItemSemanticAction(EXERCISE_CHOOSEFROM, $1); }
	| match_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_MATCH, $1); }
	| chart_block						{ $$ = ExerciseItemSemanticAction(EXERCISE_CHART, $1); }
	| set_block							{ $$ = ExerciseItemSemanticAction(EXERCISE_SET, $1); }
	| section_block						{ $$ = SectionItemSemanticAction($1); }
	| TEXT_KW COLON STRING SEMICOLON	{ $$ = TextItemSemanticAction($3); }
	| image_block						{ $$ = ImageItemSemanticAction($1); }
	;

/* ── mc ── */

multiplechoice_block: MULTIPLECHOICE OPEN_BRACE multiplechoice_props CLOSE_BRACE			{ $$ = $3; }
	;

multiplechoice_props: multiplechoice_props QUESTION COLON STRING SEMICOLON		{ $$ = MultiplechoiceSetQuestionSemanticAction($1, $4); }
	| multiplechoice_props OPTION COLON STRING SEMICOLON			{ $$ = MultiplechoiceAddOptionSemanticAction($1, $4, 0); }
	| multiplechoice_props OPTION COLON STRING HASH SEMICOLON		{ $$ = MultiplechoiceAddOptionSemanticAction($1, $4, 1); }
	| multiplechoice_props OPTION COLON INTEGER SEMICOLON			{ $$ = MultiplechoiceAddIntOptionSemanticAction($1, $4, 0); }
	| multiplechoice_props OPTION COLON INTEGER HASH SEMICOLON		{ $$ = MultiplechoiceAddIntOptionSemanticAction($1, $4, 1); }
	| multiplechoice_props SCORE COLON INTEGER SEMICOLON			{ $$ = MultiplechoiceSetScoreSemanticAction($1, $4); }
	| %empty											{ $$ = CreateMultiplechoiceNodeSemanticAction(); }
	;

/* ── torf ── */

trueorfalse_block: TRUEORFALSE OPEN_BRACE trueorfalse_props CLOSE_BRACE		{ $$ = $3; }
	;

trueorfalse_props: trueorfalse_props QUESTION COLON STRING SEMICOLON	{ $$ = TrueorfalseSetQuestionSemanticAction($1, $4); }
	| trueorfalse_props JUSTIFY COLON TRUE SEMICOLON			{ $$ = TrueorfalseSetJustifySemanticAction($1, 1); }
	| trueorfalse_props JUSTIFY COLON FALSE SEMICOLON			{ $$ = TrueorfalseSetJustifySemanticAction($1, 0); }
	| trueorfalse_props ANSWER COLON TRUE SEMICOLON			{ $$ = TrueorfalseSetAnswerSemanticAction($1, 1); }
	| trueorfalse_props ANSWER COLON FALSE SEMICOLON			{ $$ = TrueorfalseSetAnswerSemanticAction($1, 0); }
	| trueorfalse_props SCORE COLON INTEGER SEMICOLON			{ $$ = TrueorfalseSetScoreSemanticAction($1, $4); }
	| %empty											{ $$ = CreateTrueorfalseNodeSemanticAction(); }
	;

/* ── direct ── */

openquestion_block: OPENQUESTION OPEN_BRACE openquestion_props CLOSE_BRACE	{ $$ = $3; }
	;

openquestion_props: openquestion_props QUESTION COLON STRING SEMICOLON	{ $$ = OpenquestionSetQuestionSemanticAction($1, $4); }
	| openquestion_props LINES COLON INTEGER SEMICOLON			{ $$ = OpenquestionSetLinesSemanticAction($1, $4); }
	| openquestion_props ANSWER COLON STRING SEMICOLON			{ $$ = OpenquestionSetAnswerSemanticAction($1, $4); }
	| openquestion_props SCORE COLON INTEGER SEMICOLON			{ $$ = OpenquestionSetScoreSemanticAction($1, $4); }
	| %empty												{ $$ = CreateOpenquestionNodeSemanticAction(); }
	;

/* ── blanks ── */

fillblanks_block: FILLBLANKS OPEN_BRACE fillblanks_props CLOSE_BRACE	{ $$ = $3; }
	;

fillblanks_props: fillblanks_props QUESTION COLON STRING SEMICOLON	{ $$ = FillblanksSetQuestionSemanticAction($1, $4); }
	| fillblanks_props ANSWER COLON STRING SEMICOLON			{ $$ = FillblanksAddAnswerSemanticAction($1, $4); }
	| fillblanks_props SCORE COLON INTEGER SEMICOLON			{ $$ = FillblanksSetScoreSemanticAction($1, $4); }
	| %empty												{ $$ = CreateFillblanksNodeSemanticAction(); }
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

choosefrom_block: CHOOSEFROM OPEN_BRACE choosefrom_props CLOSE_BRACE		{ $$ = $3; }
	;

choosefrom_props: choosefrom_props TASK COLON STRING SEMICOLON			{ $$ = ChoosefromSetTaskSemanticAction($1, $4); }
	| choosefrom_props OPTIONS COLON string_list SEMICOLON					{ $$ = ChoosefromSetOptionsSemanticAction($1, $4); }
	| choosefrom_props TEXT_KW COLON STRING SEMICOLON						{ $$ = ChoosefromSetTextSemanticAction($1, $4); }
	| choosefrom_props ANSWER COLON int_list SEMICOLON						{ $$ = ChoosefromSetAnswerSemanticAction($1, $4); }
	| choosefrom_props SCORE COLON INTEGER SEMICOLON						{ $$ = ChoosefromSetScoreSemanticAction($1, $4); }
	| choosefrom_props SHUFFLE COLON TRUE SEMICOLON						{ $$ = ChoosefromSetShuffleSemanticAction($1, 1); }
	| choosefrom_props SHUFFLE COLON FALSE SEMICOLON						{ $$ = ChoosefromSetShuffleSemanticAction($1, 0); }
	| %empty																{ $$ = CreateChoosefromNodeSemanticAction(); }
	;

/* ── set ── */

set_block: SET OPEN_BRACE set_props CLOSE_BRACE								{ $$ = $3; }
	;

set_props: set_props TEXT_KW COLON STRING SEMICOLON							{ $$ = SetSetTextSemanticAction($1, $4); }
	| set_props SCORE COLON INTEGER SEMICOLON								{ $$ = SetSetScoreSemanticAction($1, $4); }
	| set_props SHUFFLE COLON TRUE SEMICOLON								{ $$ = SetSetShuffleSemanticAction($1, 1); }
	| set_props SHUFFLE COLON FALSE SEMICOLON								{ $$ = SetSetShuffleSemanticAction($1, 0); }
	| set_props multiplechoice_block													{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_MULTIPLECHOICE, $2); }
	| set_props trueorfalse_block													{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_TRUEORFALSE, $2); }
	| set_props openquestion_block												{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_OPENQUESTION, $2); }
	| set_props fillblanks_block												{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_FILLBLANKS, $2); }
	| set_props choosefrom_block											{ $$ = SetAddExerciseSemanticAction($1, EXERCISE_CHOOSEFROM, $2); }
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

/* ── image ── */

image_block: IMAGE OPEN_BRACE image_props CLOSE_BRACE	{ $$ = $3; }
	;

image_props: image_props PATH COLON STRING SEMICOLON	{ $$ = ImageSetPathSemanticAction($1, $4); }
	| image_props CAPTION COLON STRING SEMICOLON		{ $$ = ImageSetCaptionSemanticAction($1, $4); }
	| image_props WIDTH COLON INTEGER SEMICOLON			{ $$ = ImageSetWidthSemanticAction($1, $4); }
	| %empty											{ $$ = CreateImageNodeSemanticAction(); }
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
