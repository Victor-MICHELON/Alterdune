#pragma once
#include "Entity.h"
#include <vector>
#include <string>

class Monster : public Entity {
protected:
    std::string category;
    int mercyCurrent;
    int mercyGoal;
    std::vector<std::string> availableActs;
    bool spared;

public:
    Monster(std::string name, int hp, int atk, int def, std::string category, int mercyGoal, std::vector<std::string> acts);
    
    virtual ~Monster() = default;

    virtual int getActCapacity() const = 0; 

    void displayStats() const override;

    void receiveAct(int impact); 
    bool canBeSpared() const;
    void spare();
    bool isSpared() const;

    std::string getCategory() const;
    int getMercyCurrent() const;
    int getMercyGoal() const;
    const std::vector<std::string>& getAvailableActs() const;
};