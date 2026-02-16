#include <iostream>
#include <string>

using namespace std;

// کلاس پایه abstract
class Entity {
protected:
    int id;
    string name;

public:
    Entity(int _id, string _name) : id(_id), name(_name) {}

    // تابع virtual خالص
    virtual string type() const = 0;

    virtual ~Entity() {} // destructor virtual
};

Player
    class Player : public Entity {
public:
    Player(int id, string name) : Entity(id, name) {}

    string type() const override {
        return "Player";
    }
};

// کلاس Enemy
class Enemy : public Entity {
public:
    Enemy(int id, string name) : Entity(id, name) {}

    string type() const override {
        return "Enemy";
    }
};
