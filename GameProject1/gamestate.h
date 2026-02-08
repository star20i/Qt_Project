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

class GameState {
public:
    Board board;
    Deck deckA, deckB;

    Player currentPlayer = Player::A;
    AgentType currentCard = AgentType::None;

    void initNewGame();   // init decks + shuffle + start turn 13
    void startTurn();
    void endTurn();       // rotate card, switch player 14

    // action availability
    QVector<QString> sourcesForCurrentCard() const;
    QVector<QString> moveDests(const QString& fromId) const;
    QVector<QString> enemyTargets() const;

    // actions
    bool movePiece(const QString& fromId, const QString& toId);
    bool scoutMarkCurrent(const QString& scoutId);                // scout action 15
    bool sergeantControlOrReleaseCurrent(const QString& sergId);  // control/release 16
    AttackResult attack(const QString& attackerId, const QString& defenderId); // BFS+criteria+dice 17

    bool checkWin(Player &winner) const; // remove all enemy OR control 7 18

    QVector<CellNode*> bfsPath(CellNode* start, CellNode* goal) const; // BFS pointer-based 19

private:
    bool canEnter(CellNode* from, CellNode* to) const;
    int hp(AgentType t) const;         // scout=5 sniper=4 serg=3 20
    int diceCount(AgentType t) const;  // sniper=3 else 1
};

#endif // GAMESTATE_H
