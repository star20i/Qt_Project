#ifndef DECK_H
#define DECK_H

#include "enums.h"
#include <QVector>

class Deck {
public:
    void initStandard();
    void shuffle();

    AgentType top() const;
    void rotateTopToBack();

    bool burnOne(AgentType t);
    int countOf(AgentType t) const;

private:
    QVector<AgentType> m_cards;
};

#endif
