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
Program * ProgramSemanticAction(Item * items);
Program * ProgramWithHeaderSemanticAction(Header * header, Item * items);
HeaderProperty * HeaderPropertySemanticAction(HeaderPropertyType type, char * stringValue, int intValue);
Header * CreateHeaderSemanticAction(HeaderProperty * firstProp);
Header * AddHeaderPropertySemanticAction(Header * header, HeaderProperty * prop);
Item * TextItemSemanticAction(char * text);
Item * ExerciseItemSemanticAction(ExerciseType type, void * node);
Item * AppendItemSemanticAction(Item * list, Item * item);
McNode * CreateMcNodeSemanticAction();
McNode * McSetQuestionSemanticAction(McNode * node, char * question);
McNode * McAddOptionSemanticAction(McNode * node, char * value, int isCorrect);
McNode * McAddIntOptionSemanticAction(McNode * node, int value, int isCorrect);
McNode * McSetScoreSemanticAction(McNode * node, int score);
TorfNode * CreateTorfNodeSemanticAction();
TorfNode * TorfSetQuestionSemanticAction(TorfNode * node, char * question);
TorfNode * TorfSetJustifySemanticAction(TorfNode * node, int justify);
TorfNode * TorfSetAnswerSemanticAction(TorfNode * node, int answer);
TorfNode * TorfSetScoreSemanticAction(TorfNode * node, int score);
DirectNode * CreateDirectNodeSemanticAction();
DirectNode * DirectSetQuestionSemanticAction(DirectNode * node, char * question);
DirectNode * DirectSetLinesSemanticAction(DirectNode * node, int lines);
DirectNode * DirectSetAnswerSemanticAction(DirectNode * node, char * answer);
DirectNode * DirectSetScoreSemanticAction(DirectNode * node, int score);
BlanksNode * CreateBlanksNodeSemanticAction();
BlanksNode * BlanksSetQuestionSemanticAction(BlanksNode * node, char * question);
BlanksNode * BlanksAddAnswerSemanticAction(BlanksNode * node, char * answer);
BlanksNode * BlanksSetScoreSemanticAction(BlanksNode * node, int score);
StringList * CreateStringListSemanticAction(char * value);
StringList * AppendStringListSemanticAction(StringList * list, char * value);
IntList * CreateIntListSemanticAction(int value);
IntList * AppendIntListSemanticAction(IntList * list, int value);
ChooseFromNode * CreateChooseFromNodeSemanticAction();
ChooseFromNode * ChooseFromSetTaskSemanticAction(ChooseFromNode * node, char * task);
ChooseFromNode * ChooseFromSetOptionsSemanticAction(ChooseFromNode * node, StringList * options);
ChooseFromNode * ChooseFromSetTextSemanticAction(ChooseFromNode * node, char * text);
ChooseFromNode * ChooseFromSetAnswerSemanticAction(ChooseFromNode * node, IntList * answer);
ChooseFromNode * ChooseFromSetScoreSemanticAction(ChooseFromNode * node, int score);
ChooseFromNode * ChooseFromSetShuffleSemanticAction(ChooseFromNode * node, int shuffle);

#endif
