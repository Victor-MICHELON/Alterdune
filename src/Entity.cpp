#include "Entity.h"
#include <algorithm>

using namespace std;

Entity::Entity(string name, int hpMax, int attack, int defense)
    : name(name), hpMax(hpMax), hpCurrent(hpMax), attack(attack), defense(defense) {}

void Entity::takeDamage(int damage) {
    if (damage > 0) {
        hpCurrent -= damage;
    }
    if (hpCurrent < 0) {
        hpCurrent = 0;
    }
}

void Entity::heal(int amount) {
    if (amount > 0 && isAlive()) {
        hpCurrent += amount;
        // On ne peut pas dépasser les HP Max
        if (hpCurrent > hpMax) {
            hpCurrent = hpMax;
        }
    }
}

bool Entity::isAlive() const {
    return hpCurrent > 0;
}

// Getters
string Entity::getName() const { return name; }
int Entity::getHpCurrent() const { return hpCurrent; }
int Entity::getHpMax() const { return hpMax; }
int Entity::getAttack() const { return attack; }
int Entity::getDefense() const { return defense; }