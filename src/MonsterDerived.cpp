#include "MonsterDerived.h"

using namespace std;

// NORMAL
NormalMonster::NormalMonster(string name, int hp, int atk, int def, int mercyGoal, vector<string> acts)
    : Monster(name, hp, atk, def, "NORMAL", mercyGoal, acts) {}

int NormalMonster::getActCapacity() const { return 2; }

Monster* NormalMonster::clone() const { 
    return new NormalMonster(*this); 
}


// MINIBOSS
MinibossMonster::MinibossMonster(string name, int hp, int atk, int def, int mercyGoal, vector<string> acts)
    : Monster(name, hp, atk, def, "MINIBOSS", mercyGoal, acts) {}

int MinibossMonster::getActCapacity() const { return 3; }

Monster* MinibossMonster::clone() const { 
    return new MinibossMonster(*this); 
}


// BOSS
BossMonster::BossMonster(string name, int hp, int atk, int def, int mercyGoal, vector<string> acts)
    : Monster(name, hp, atk, def, "BOSS", mercyGoal, acts) {}

int BossMonster::getActCapacity() const { return 4; }

Monster* BossMonster::clone() const { 
    return new BossMonster(*this); 
}
