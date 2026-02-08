#include "deck.h"
#include <QRandomGenerator>

void Deck::initStandard()
{
    m_cards.clear();
    for(int i=0;i<4;i++) m_cards.push_back(AgentType::Scout);
    for(int i=0;i<3;i++) m_cards.push_back(AgentType::Sniper);
    for(int i=0;i<3;i++) m_cards.push_back(AgentType::Sergeant);
}

void Deck::shuffle()
{
    for(int i=m_cards.size()-1;i>0;--i){
        int j = QRandomGenerator::global()->bounded(i+1);
        m_cards.swapItemsAt(i,j);
    }
}

AgentType Deck::top() const
{
    return m_cards.isEmpty() ? AgentType::None : m_cards.front();
}

void Deck::rotateTopToBack()
{
    if(m_cards.isEmpty()) return;
    AgentType t = m_cards.front();
    m_cards.pop_front();
    m_cards.push_back(t);
}

bool Deck::burnOne(AgentType t)
{
    for(int i=0;i<m_cards.size();++i){
        if(m_cards[i]==t){
            m_cards.removeAt(i);
            return true;
        }
    }
    return false;
}

int Deck::countOf(AgentType t) const
{
    int c=0;
    for(auto x: m_cards) if(x==t) c++;
    return c;
}
