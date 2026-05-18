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
MultiplechoiceNode * CreateMultiplechoiceNodeSemanticAction();
MultiplechoiceNode * MultiplechoiceSetQuestionSemanticAction(MultiplechoiceNode * node, char * question);
MultiplechoiceNode * MultiplechoiceAddOptionSemanticAction(MultiplechoiceNode * node, char * value, int isCorrect);
MultiplechoiceNode * MultiplechoiceAddIntOptionSemanticAction(MultiplechoiceNode * node, int value, int isCorrect);
MultiplechoiceNode * MultiplechoiceSetScoreSemanticAction(MultiplechoiceNode * node, int score);

/* ── torf ── */
TrueorfalseNode * CreateTrueorfalseNodeSemanticAction();
TrueorfalseNode * TrueorfalseSetQuestionSemanticAction(TrueorfalseNode * node, char * question);
TrueorfalseNode * TrueorfalseSetJustifySemanticAction(TrueorfalseNode * node, int justify);
TrueorfalseNode * TrueorfalseSetAnswerSemanticAction(TrueorfalseNode * node, int answer);
TrueorfalseNode * TrueorfalseSetScoreSemanticAction(TrueorfalseNode * node, int score);

/* ── direct ── */
OpenquestionNode * CreateOpenquestionNodeSemanticAction();
OpenquestionNode * OpenquestionSetQuestionSemanticAction(OpenquestionNode * node, char * question);
OpenquestionNode * OpenquestionSetLinesSemanticAction(OpenquestionNode * node, int lines);
OpenquestionNode * OpenquestionSetAnswerSemanticAction(OpenquestionNode * node, char * answer);
OpenquestionNode * OpenquestionSetScoreSemanticAction(OpenquestionNode * node, int score);

/* ── blanks ── */
FillblanksNode * CreateFillblanksNodeSemanticAction();
FillblanksNode * FillblanksSetQuestionSemanticAction(FillblanksNode * node, char * question);
FillblanksNode * FillblanksAddAnswerSemanticAction(FillblanksNode * node, char * answer);
FillblanksNode * FillblanksSetScoreSemanticAction(FillblanksNode * node, int score);

/* ── lists ── */
StringList * CreateStringListSemanticAction(char * value);
StringList * AppendStringListSemanticAction(StringList * list, char * value);
IntList * CreateIntListSemanticAction(int value);
IntList * AppendIntListSemanticAction(IntList * list, int value);

/* ── chooseFrom ── */
ChoosefromNode * CreateChoosefromNodeSemanticAction();
ChoosefromNode * ChoosefromSetTaskSemanticAction(ChoosefromNode * node, char * task);
ChoosefromNode * ChoosefromSetOptionsSemanticAction(ChoosefromNode * node, StringList * options);
ChoosefromNode * ChoosefromSetTextSemanticAction(ChoosefromNode * node, char * text);
ChoosefromNode * ChoosefromSetAnswerSemanticAction(ChoosefromNode * node, IntList * answer);
ChoosefromNode * ChoosefromSetScoreSemanticAction(ChoosefromNode * node, int score);
ChoosefromNode * ChoosefromSetShuffleSemanticAction(ChoosefromNode * node, int shuffle);

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
