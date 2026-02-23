#pragma once
#include "Entity.h"
#include "Item.h"
#include <vector>

 
class Player : public Entity {
private:
    std::vector<Item> inventory; 
    int victories;
    int monstersKilled;
    int monstersSpared;

public:
    // Constructeur
    Player(std::string name);

    void displayStats() const override;

    void addItem(const Item& item);
    void displayInventory() const;
    void useItemFromInventory(int index);

    void addVictory(bool killedMonster);
    
    // Accesseurs
    int getVictories() const;
    int getMonstersKilled() const;
    int getMonstersSpared() const;
};