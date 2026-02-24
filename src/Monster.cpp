#include "Monster.h"
#include <iostream>

using namespace std;

Monster::Monster(string name, int hp, int atk, int def, string category, int mercyGoal, vector<string> acts)
    : Entity(name, hp, atk, def), category(category), mercyCurrent(0), mercyGoal(mercyGoal), availableActs(acts), spared(false) {}

void Monster::displayStats() const {
    cout << "--- " << name << " (" << category << ") ---" << endl;
    cout << "HP : " << hpCurrent << " / " << hpMax << endl;
    cout << "Mercy : " << mercyCurrent << " / " << mercyGoal << endl;
}

void Monster::receiveAct(int impact) {
    mercyCurrent += impact;
    
    if (mercyCurrent < 0) {
        mercyCurrent = 0;
    } else if (mercyCurrent > mercyGoal) {
        mercyCurrent = mercyGoal;
    }
}

bool Monster::canBeSpared() const {
    return mercyCurrent >= mercyGoal && isAlive();
}

void Monster::spare() {
    if (canBeSpared()) {
        spared = true;
    }
}

bool Monster::isSpared() const { return spared; }
string Monster::getCategory() const { return category; }
int Monster::getMercyCurrent() const { return mercyCurrent; }
int Monster::getMercyGoal() const { return mercyGoal; }
const vector<string>& Monster::getAvailableActs() const { return availableActs; }