#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule();

/**
 * Bison semantic actions.
 */

/* ── program ── */
Program * ProgramSemanticAction(Item * items);
Program * ProgramWithHeaderSemanticAction(Header * header, Item * items);

/* ── items ── */
Item * TextItemSemanticAction(char * text);
Item * ExerciseItemSemanticAction(ExerciseType type, void * node);
Item * AppendItemSemanticAction(Item * list, Item * item);

/* ── header ── */
HeaderProperty * HeaderPropertySemanticAction(HeaderPropertyType type, char * stringValue, int intValue);
Header * CreateHeaderSemanticAction(HeaderProperty * firstProp);
Header * AddHeaderPropertySemanticAction(Header * header, HeaderProperty * prop);

/* ── mc ── */
McNode * CreateMcNodeSemanticAction();
McNode * McSetQuestionSemanticAction(McNode * node, char * question);
McNode * McAddOptionSemanticAction(McNode * node, char * value, int isCorrect);
McNode * McAddIntOptionSemanticAction(McNode * node, int value, int isCorrect);
McNode * McSetScoreSemanticAction(McNode * node, int score);

/* ── torf ── */
TorfNode * CreateTorfNodeSemanticAction();
TorfNode * TorfSetQuestionSemanticAction(TorfNode * node, char * question);
TorfNode * TorfSetJustifySemanticAction(TorfNode * node, int justify);
TorfNode * TorfSetAnswerSemanticAction(TorfNode * node, int answer);
TorfNode * TorfSetScoreSemanticAction(TorfNode * node, int score);

/* ── direct ── */
DirectNode * CreateDirectNodeSemanticAction();
DirectNode * DirectSetQuestionSemanticAction(DirectNode * node, char * question);
DirectNode * DirectSetLinesSemanticAction(DirectNode * node, int lines);
DirectNode * DirectSetAnswerSemanticAction(DirectNode * node, char * answer);
DirectNode * DirectSetScoreSemanticAction(DirectNode * node, int score);

/* ── blanks ── */
BlanksNode * CreateBlanksNodeSemanticAction();
BlanksNode * BlanksSetQuestionSemanticAction(BlanksNode * node, char * question);
BlanksNode * BlanksAddAnswerSemanticAction(BlanksNode * node, char * answer);
BlanksNode * BlanksSetScoreSemanticAction(BlanksNode * node, int score);

/* ── lists ── */
StringList * CreateStringListSemanticAction(char * value);
StringList * AppendStringListSemanticAction(StringList * list, char * value);
IntList * CreateIntListSemanticAction(int value);
IntList * AppendIntListSemanticAction(IntList * list, int value);

/* ── chooseFrom ── */
ChooseFromNode * CreateChooseFromNodeSemanticAction();
ChooseFromNode * ChooseFromSetTaskSemanticAction(ChooseFromNode * node, char * task);
ChooseFromNode * ChooseFromSetOptionsSemanticAction(ChooseFromNode * node, StringList * options);
ChooseFromNode * ChooseFromSetTextSemanticAction(ChooseFromNode * node, char * text);
ChooseFromNode * ChooseFromSetAnswerSemanticAction(ChooseFromNode * node, IntList * answer);
ChooseFromNode * ChooseFromSetScoreSemanticAction(ChooseFromNode * node, int score);
ChooseFromNode * ChooseFromSetShuffleSemanticAction(ChooseFromNode * node, int shuffle);

/* ── match ── */
MatchNode * CreateMatchNodeSemanticAction();
MatchNode * MatchSetQuestionSemanticAction(MatchNode * node, char * question);
MatchNode * MatchAddPairSemanticAction(MatchNode * node, char * left, char * right);
MatchNode * MatchSetScoreSemanticAction(MatchNode * node, int score);

/* ── chart ── */
ChartNode * CreateChartNodeSemanticAction();
ChartNode * ChartSetTaskSemanticAction(ChartNode * node, char * task);
ChartNode * ChartSetDimSemanticAction(ChartNode * node, int rows, int cols);
ChartNode * ChartAddCellSemanticAction(ChartNode * node, int row, int col, char * value);
ChartNode * ChartSetAnswerSemanticAction(ChartNode * node, StringList * answer);
ChartNode * ChartSetScoreSemanticAction(ChartNode * node, int score);

/* ── set ── */
SetNode * CreateSetNodeSemanticAction();
SetNode * SetSetTextSemanticAction(SetNode * node, char * text);
SetNode * SetSetScoreSemanticAction(SetNode * node, int score);
SetNode * SetSetShuffleSemanticAction(SetNode * node, int shuffle);
SetNode * SetAddExerciseSemanticAction(SetNode * node, ExerciseType type, void * exerciseNode);

/* ── section ── */
Item * SectionItemSemanticAction(SectionNode * section);
SectionNode * CreateSectionNodeSemanticAction();
SectionNode * SectionSetNameSemanticAction(SectionNode * node, char * name);
SectionNode * SectionSetSelectSemanticAction(SectionNode * node, int select);
SectionNode * SectionAddItemSemanticAction(SectionNode * node, Item * item);

#endif
