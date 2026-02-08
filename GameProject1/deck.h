#ifndef DECK_H
#define DECK_H

#include "enums.h"
#include <QVector>

class Deck {
public:
    void initStandard();      // 4 scout, 3 sniper, 3 sergeant 10
    void shuffle();

    AgentType top() const;
    void rotateTopToBack();   // بعد از اکشن، کارت به انتها 11

    bool burnOne(AgentType t); // در حمله موفق: یک کارت از نوع هدف حذف شود 12
    int countOf(AgentType t) const;

private:
    QVector<AgentType> m_cards;
};

#endif // DECK_H
