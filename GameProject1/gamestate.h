#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "board.h"
#include "deck.h"
#include <QVector>

struct AttackResult {
    bool ok = false;         // آیا اکشن معتبر بود؟
    bool success = false;
    int criteria = 0;
    QVector<int> rolls;
    AgentType burnedType = AgentType::None;
    bool pieceRemoved = false;
};


#endif // GAMESTATE_H
