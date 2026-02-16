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


NPC
    class NPC : public Entity {
public:
    NPC(int id, string name) : Entity(id, name) {}

    string type() const override {
        return "NPC";
    }
};

// تابع پلی‌مورفیک
void printEntity(const Entity& e) {
    cout << e.type() << endl;
}
int main() {
    // ایجاد نمونه‌ها
    Player player1(1, "Arash");
    Enemy enemy1(2, "Dragon");
    NPC npc1(3, "Merchant");

    // استفاده پلی‌مورفیک
    cout << "انواع موجودیت‌ها:" << endl;
    printEntity(player1);  // Player
    printEntity(enemy1);   // Enemy
    printEntity(npc1);     // NPC



    // استفاده با اشاره‌گر برای نمایش پلی‌مورفیسم
    cout << "\nبا اشاره‌گر:" << endl;

    Entity* entities[] = {&player1, &enemy1, &npc1};

    for (int i = 0; i < 3; i++) {
        cout << entities[i]->type() << endl;
    }

    return 0;
}

