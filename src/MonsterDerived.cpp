#include "MonsterDerived.h"

using namespace std;

// NORMAL
NormalMonster::NormalMonster(string name, int hp, int atk, int def, int mercyGoal, vector<string> acts)
    : Monster(name, hp, atk, def, "NORMAL", mercyGoal, acts) {}

int NormalMonster::getActCapacity() const {
    return 2;
}


// MINIBOSS
MinibossMonster::MinibossMonster(string name, int hp, int atk, int def, int mercyGoal, vector<string> acts)
    : Monster(name, hp, atk, def, "MINIBOSS", mercyGoal, acts) {}

int MinibossMonster::getActCapacity() const {
    return 3;
}
// BOSS
BossMonster::BossMonster(string name, int hp, int atk, int def, int mercyGoal, vector<string> acts)
    : Monster(name, hp, atk, def, "BOSS", mercyGoal, acts) {}

int BossMonster::getActCapacity() const {
    return 4;
}