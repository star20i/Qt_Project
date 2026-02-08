#ifndef CELLNODE_H
#define CELLNODE_H

#include "enums.h"
#include <QString>
#include <QVector>

struct CellNode {
    QString id;
    int shield = 0;

    Player pieceOwner = Player::None;
    AgentType pieceType = AgentType::None;

    bool markedA = false;
    bool markedB = false;

    Player controlOwner = Player::None;

    QVector<CellNode*> neighbors;
};

#endif
