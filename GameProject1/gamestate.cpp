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

void GameState::startTurn()
{
    Deck &d = (currentPlayer==Player::A) ? deckA : deckB;
    currentCard = d.top();
}

void GameState::endTurn()
{
    Deck &d = (currentPlayer==Player::A) ? deckA : deckB;
    d.rotateTopToBack();

    currentPlayer = (currentPlayer==Player::A) ? Player::B : Player::A;
    startTurn();
}

int GameState::hp(AgentType t) const
{
    if(t==AgentType::Scout) return 5;
    if(t==AgentType::Sniper) return 4;
    if(t==AgentType::Sergeant) return 3;
    return 0;
}

int GameState::diceCount(AgentType t) const
{
    return (t==AgentType::Sniper) ? 3 : 1;
}

QVector<QString> GameState::sourcesForCurrentCard() const
{
    QVector<QString> res;
    for(auto* n: board.nodes()){
        if(n->pieceOwner==currentPlayer && n->pieceType==currentCard)
            res.push_back(n->id);
    }
    return res;
}

QVector<QString> GameState::enemyTargets() const
{
    QVector<QString> res;
    Player enemy = (currentPlayer==Player::A) ? Player::B : Player::A;
    for(auto* n: board.nodes()){
        if(n->pieceOwner==enemy) res.push_back(n->id);
    }
    return res;
}

bool GameState::canEnter(CellNode *from, CellNode *to) const
{
    if(!from || !to) return false;
    if(to->pieceOwner != Player::None) return false;
    if(!from->neighbors.contains(to)) return false;

    if(from->pieceType == AgentType::Scout) return true;

    if(currentPlayer==Player::A) return to->markedA;
    return to->markedB;
}

QVector<QString> GameState::moveDests(const QString &fromId) const
{
    QVector<QString> res;
    CellNode* from = board.get(fromId);
    if(!from) return res;
    for(CellNode* nb: from->neighbors)
        if(canEnter(from, nb)) res.push_back(nb->id);
    return res;
}

bool GameState::movePiece(const QString &fromId, const QString &toId)
{
    CellNode* from = board.get(fromId);
    CellNode* to = board.get(toId);
    if(!canEnter(from, to)) return false;

    to->pieceOwner = from->pieceOwner;
    to->pieceType  = from->pieceType;
    from->pieceOwner = Player::None;
    from->pieceType  = AgentType::None;
    return true;
}

bool GameState::scoutMarkCurrent(const QString &scoutId)
{
    CellNode* s = board.get(scoutId);
    if(!s) return false;
    if(!(s->pieceOwner==currentPlayer && s->pieceType==AgentType::Scout)) return false;

    if(currentPlayer==Player::A){
        if(s->markedA) return false;
        s->markedA = true;
    }else{
        if(s->markedB) return false;
        s->markedB = true;
    }
    return true;
}

bool GameState::sergeantControlOrReleaseCurrent(const QString &sergId)
{
    CellNode* c = board.get(sergId);
    if(!c) return false;
    if(!(c->pieceOwner==currentPlayer && c->pieceType==AgentType::Sergeant)) return false;

    Player me = currentPlayer;
    Player enemy = (me==Player::A) ? Player::B : Player::A;

    if(c->controlOwner == enemy){
        c->controlOwner = Player::None;
        return true;
    }else{
        c->controlOwner = me;
        return true;
    }
}

QVector<CellNode*> GameState::bfsPath(CellNode *start, CellNode *goal) const
{
    if(!start || !goal) return {};

    QHash<CellNode*, CellNode*> parent;
    QSet<CellNode*> visited;
    QVector<CellNode*> q;

    visited.insert(start);
    parent.insert(start, nullptr);
    q.push_back(start);

    int qi=0;
    while(qi<q.size()){
        CellNode* cur = q[qi++];
        if(cur==goal) break;
        for(CellNode* nb: cur->neighbors){
            if(!visited.contains(nb)){
                visited.insert(nb);
                parent.insert(nb, cur);
                q.push_back(nb);
            }
        }
    }

    if(!parent.contains(goal)) return {};
    QVector<CellNode*> path;
    for(CellNode* cur=goal; cur!=nullptr; cur=parent.value(cur))
        path.push_back(cur);
    std::reverse(path.begin(), path.end());
    return path;
}

AttackResult GameState::attack(const QString &attackerId, const QString &defenderId)
{
    AttackResult out;

    CellNode* att = board.get(attackerId);
    CellNode* def = board.get(defenderId);
    if(!att || !def) return out;

    if(!(att->pieceOwner==currentPlayer && att->pieceType==currentCard)) return out;

    Player enemy = (currentPlayer==Player::A) ? Player::B : Player::A;
    if(def->pieceOwner != enemy) return out;

    auto path = bfsPath(att, def);
    if(path.isEmpty()) return out;

    int sumShield=0;
    for(int i=1; i+1<path.size(); ++i){
        CellNode* n = path[i];
        if(n->pieceOwner != Player::None) continue;
        sumShield += n->shield;
    }

    int x = sumShield + hp(def->pieceType);
    if(x > 10) x = 10;

    out.criteria = x;
    out.ok = true;

    int dCount = diceCount(att->pieceType);
    bool success = false;
    for(int i=0;i<dCount;i++){
        int d = QRandomGenerator::global()->bounded(1, 11); // 1..10
        out.rolls.push_back(d);
        if(d >= x) success = true;
    }
    out.success = success;

    if(!success) return out;


    Deck &enemyDeck = (currentPlayer==Player::A) ? deckB : deckA;
    enemyDeck.burnOne(def->pieceType);
    out.burnedType = def->pieceType;


    if(enemyDeck.countOf(def->pieceType)==0){
        for(auto* n: board.nodes()){
            if(n->pieceOwner==enemy && n->pieceType==def->pieceType){
                n->pieceOwner = Player::None;
                n->pieceType = AgentType::None;
            }
        }
        out.pieceRemoved = true;
    }

    return out;
}

bool GameState::checkWin(Player &winner) const
{
    Player me = currentPlayer;
    Player enemy = (me==Player::A) ? Player::B : Player::A;

    bool enemyHasPiece=false;
    for(auto* n: board.nodes()){
        if(n->pieceOwner==enemy){ enemyHasPiece=true; break; }
    }
    if(!enemyHasPiece){
        winner = me;
        return true;
    }

    int ctrl=0;
    for(auto* n: board.nodes())
        if(n->controlOwner==me) ctrl++;

    if(ctrl >= 7){
        winner = me;
        return true;
    }

    winner = Player::None;
    return false;
}
