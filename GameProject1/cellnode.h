#ifndef CELLNODE_H
#define CELLNODE_H

#include "enums.h"
#include <QString>
#include <QVector>

struct CellNode {
    QString id;       // A01 ...
    int shield = 0;   // 0..2

    Player pieceOwner = Player::None;
    AgentType pieceType = AgentType::None;

    bool markedA = false;
    bool markedB = false;

    Player controlOwner = Player::None;

    // pointer-based adjacency (مطابق PDF) 8
    QVector<CellNode*> neighbors;
};
#endif
