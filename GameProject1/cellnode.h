#ifndef CELLNODE_H
#define CELLNODE_H

#include "enums.h"
#include <QString>
#include <QVector>

struct CellNode {
<<<<<<< HEAD
    QString id;       // A01 ...
    int shield = 0;   // 0..2
=======
    QString id;
    int shield = 0;
>>>>>>> feature-arefeh2

    Player pieceOwner = Player::None;
    AgentType pieceType = AgentType::None;

    bool markedA = false;
    bool markedB = false;

    Player controlOwner = Player::None;

<<<<<<< HEAD
    // pointer-based adjacency (مطابق PDF) 8
    QVector<CellNode*> neighbors;
};
=======
    QVector<CellNode*> neighbors;
};

>>>>>>> feature-arefeh2
#endif
