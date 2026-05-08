%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus BooleanLexemeAction(TokenLabel label);
extern CompilationStatus DelimiterLexemeAction(TokenLabel label);
extern CompilationStatus IdentifierLexemeAction();
extern CompilationStatus EnterMultilineCommentLexemeAction(FlexContext context);
extern CompilationStatus EnterStringLiteralLexemeAction(FlexContext context);
extern CompilationStatus StringContentLexemeAction();
extern CompilationStatus EOFLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus IntegerLexemeAction();
extern CompilationStatus KeywordLexemeAction(TokenLabel label);
extern CompilationStatus LeaveMultilineCommentLexemeAction();
extern CompilationStatus LeaveStringLiteralLexemeAction();
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus UnknownLexemeAction();

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x STRING_LITERAL

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
whitespace			[[:space:]]+

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"header"							{ return KeywordLexemeAction(HEADER); }
"mc"								{ return KeywordLexemeAction(MC); }
"blanks"							{ return KeywordLexemeAction(BLANKS); }
"direct"							{ return KeywordLexemeAction(DIRECT); }
"chooseFrom"						{ return KeywordLexemeAction(CHOOSE_FROM); }
"match"								{ return KeywordLexemeAction(MATCH); }
"torf"								{ return KeywordLexemeAction(TORF); }
"chart"								{ return KeywordLexemeAction(CHART); }
"set"								{ return KeywordLexemeAction(SET); }
"section"							{ return KeywordLexemeAction(SECTION); }

"title"								{ return KeywordLexemeAction(TITLE); }
"subject"							{ return KeywordLexemeAction(SUBJECT); }
"date"								{ return KeywordLexemeAction(DATE); }
"duration"							{ return KeywordLexemeAction(DURATION); }
"score_grid"						{ return KeywordLexemeAction(SCORE_GRID); }
"answer_sheet"						{ return KeywordLexemeAction(ANSWER_SHEET); }
"student_name"						{ return KeywordLexemeAction(STUDENT_NAME); }
"student_id"						{ return KeywordLexemeAction(STUDENT_ID); }
"course"							{ return KeywordLexemeAction(COURSE); }
"instructions"						{ return KeywordLexemeAction(INSTRUCTIONS); }

"question"							{ return KeywordLexemeAction(QUESTION); }
"option"							{ return KeywordLexemeAction(OPTION); }
"score"								{ return KeywordLexemeAction(SCORE); }
"answer"							{ return KeywordLexemeAction(ANSWER); }
"lines"								{ return KeywordLexemeAction(LINES); }
"task"								{ return KeywordLexemeAction(TASK); }
"options"							{ return KeywordLexemeAction(OPTIONS); }
"shuffle"							{ return KeywordLexemeAction(SHUFFLE); }
"pair"								{ return KeywordLexemeAction(PAIR); }
"justify"							{ return KeywordLexemeAction(JUSTIFY); }
"dim"								{ return KeywordLexemeAction(DIM); }
"cell"								{ return KeywordLexemeAction(CELL); }
"select"							{ return KeywordLexemeAction(SELECT); }
"text"								{ return KeywordLexemeAction(TEXT_KW); }

"{"									{ return DelimiterLexemeAction(OPEN_BRACE); }
"}"									{ return DelimiterLexemeAction(CLOSE_BRACE); }
"["									{ return DelimiterLexemeAction(OPEN_BRACKET); }
"]"									{ return DelimiterLexemeAction(CLOSE_BRACKET); }
":"									{ return DelimiterLexemeAction(COLON); }
";"									{ return DelimiterLexemeAction(SEMICOLON); }
","									{ return DelimiterLexemeAction(COMMA); }
"#"									{ return DelimiterLexemeAction(HASH); }

"/*"								{ return EnterMultilineCommentLexemeAction(MULTILINE_COMMENT); }
<MULTILINE_COMMENT>"*/"				{ return LeaveMultilineCommentLexemeAction(); }
<MULTILINE_COMMENT>[^*]+			{ return IgnoredLexemeAction(); }
<MULTILINE_COMMENT>.				{ return IgnoredLexemeAction(); }

"//"[^\n]*							{ return IgnoredLexemeAction(); }

"true"								{ return BooleanLexemeAction(TRUE); }
"false"								{ return BooleanLexemeAction(FALSE); }

[[:digit:]]+						{ return IntegerLexemeAction(); }
[a-zA-Z][a-zA-Z0-9_]*				{ return IdentifierLexemeAction(); }

"\""							{ return EnterStringLiteralLexemeAction(STRING_LITERAL); }
<STRING_LITERAL>[^"\n]*			{ return StringContentLexemeAction(); }
<STRING_LITERAL>"\""			{ return LeaveStringLiteralLexemeAction(); }
<STRING_LITERAL>"\n"			{ return UnknownLexemeAction(); }

{whitespace}						{ return IgnoredLexemeAction(); }
.									{ return UnknownLexemeAction(); }

<<EOF>>								{ return EOFLexemeAction(); }

%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
