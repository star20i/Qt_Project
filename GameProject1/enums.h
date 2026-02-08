#ifndef ENUMS_H
#define ENUMS_H

#include <QString>
#include <QChar>

enum class Player { None=0, A=1, B=2 };
enum class AgentType { None=0, Scout, Sniper, Sergeant };

inline Player playerFromChar(QChar c){
    if (c=='A') return Player::A;
    if (c=='B') return Player::B;
    return Player::None;
}

inline QChar playerToChar(Player p){
    if (p==Player::A) return 'A';
    if (p==Player::B) return 'B';
    return '-';
}


inline QString agentToString(AgentType t){
    switch(t){
    case AgentType::Scout: return "Scout";
    case AgentType::Sniper: return "Sniper";
    case AgentType::Sergeant: return "Sergeant";
    default: return "None";
    }
}


#endif
