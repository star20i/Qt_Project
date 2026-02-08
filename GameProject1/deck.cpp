#include "deck.h"
#include <QRandomGenerator>

void Deck::initStandard()
{
    m_cards.clear();
    for(int i=0;i<4;i++) m_cards.push_back(AgentType::Scout);
    for(int i=0;i<3;i++) m_cards.push_back(AgentType::Sniper);
    for(int i=0;i<3;i++) m_cards.push_back(AgentType::Sergeant);
}
