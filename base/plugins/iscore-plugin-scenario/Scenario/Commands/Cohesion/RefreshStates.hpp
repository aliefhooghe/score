#pragma once
#include <qlist.h>

namespace iscore
{
class CommandStack;
class Document;
}
class StateModel;

void RefreshStates(iscore::Document* doc);
void RefreshStates(const QList<const StateModel*>&, iscore::CommandStack&);
