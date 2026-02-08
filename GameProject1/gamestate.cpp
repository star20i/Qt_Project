#include "gamestate.h"
#include <QSet>
#include <QHash>
#include <QRandomGenerator>

void GameState::initNewGame()
{
    deckA.initStandard();
    deckB.initStandard();
    deckA.shuffle();
    deckB.shuffle();
    currentPlayer = Player::A;
    startTurn();
}
