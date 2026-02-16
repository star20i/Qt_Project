#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "board.h"
#include "deck.h"
#include <QVector>

struct AttackResult {
    bool ok = false;
    bool success = false;
    int criteria = 0;
    QVector<int> rolls;
    AgentType burnedType = AgentType::None;
    bool pieceRemoved = false;
};

class GameState {
public:
    Board board;
    Deck deckA, deckB;

    Player currentPlayer = Player::A;
    AgentType currentCard = AgentType::None;

    void initNewGame();
    void startTurn();
    void endTurn();

    QVector<QString> sourcesForCurrentCard() const;
    QVector<QString> moveDests(const QString& fromId) const;
    QVector<QString> enemyTargets() const;

    bool movePiece(const QString& fromId, const QString& toId);
    bool scoutMarkCurrent(const QString& scoutId);
    bool sergeantControlOrReleaseCurrent(const QString& sergId);
    AttackResult attack(const QString& attackerId, const QString& defenderId);

    bool checkWin(Player &winner) const;

    QVector<CellNode*> bfsPath(CellNode* start, CellNode* goal) const;

private:
    bool canEnter(CellNode* from, CellNode* to) const;
    int hp(AgentType t) const;
    int diceCount(AgentType t) const;
};

#endif
