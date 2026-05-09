#pragma once
#include <string>
#include <iostream>

class Entity {
protected:
    std::string name;
    int hpMax;
    int hpCurrent;
    int attack;
    int defense;

public:
   
    Entity(std::string name, int hpMax, int attack, int defense);
    
    virtual ~Entity() = default;


    virtual void displayStats() const = 0; 

    // Méthodes communes
    void takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const;

    // Accesseurs
    std::string getName() const;
    int getHpCurrent() const;
    int getHpMax() const;
    int getAttack() const;
    int getDefense() const;
};