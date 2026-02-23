#pragma once
#include <string>
#include <iostream>

class Entity {
protected: // Protected pour que les classes filles (Player, Monster) y aient accès
    std::string name;
    int hpMax;
    int hpCurrent;
    int attack;
    int defense;

public:
    // Constructeur
    Entity(std::string name, int hpMax, int attack, int defense);
    
    // Destructeur virtuel (très important en C++ avec l'héritage/polymorphisme)
    virtual ~Entity() = default;

    //classe Abstraite
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