#pragma once
#include "Monster.h"
#include <vector>
#include <string>

// NORMAL
class NormalMonster : public Monster {
public:
    NormalMonster(std::string name, int hp, int atk, int def, int mercyGoal, std::vector<std::string> acts);
    int getActCapacity() const override;
};

// MINIBOSS
class MinibossMonster : public Monster {
public:
    MinibossMonster(std::string name, int hp, int atk, int def, int mercyGoal, std::vector<std::string> acts);
    int getActCapacity() const override;
};

// BOSS
class BossMonster : public Monster {
public:
    BossMonster(std::string name, int hp, int atk, int def, int mercyGoal, std::vector<std::string> acts);
    int getActCapacity() const override;
};