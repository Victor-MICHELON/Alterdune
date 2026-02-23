#include "Player.h"

// On initialise le joueur avec par exemple 50 HP, 10 Atk, 5 Def
Player::Player(std::string name)
    : Entity(name, 50, 10, 5), victories(0), monstersKilled(0), monstersSpared(0) {}

void Player::displayStats() const {
    std::cout << "--- Statistiques de " << name << " ---" << std::endl;
    std::cout << "HP : " << hpCurrent << " / " << hpMax << std::endl;
    std::cout << "Victoires : " << victories << " / 10" << std::endl;
    std::cout << "Monstres tues : " << monstersKilled << std::endl; 
    std::cout << "Monstres epargnes : " << monstersSpared << std::endl;
}

void Player::addItem(const Item& item) {
    inventory.push_back(item);
}

void Player::displayInventory() const {
    std::cout << "--- Inventaire ---" << std::endl;
    if (inventory.empty()) {
        std::cout << "Votre inventaire est vide." << std::endl;
        return;
    }
    for (size_t i = 0; i < inventory.size(); ++i) {
        // Affiche la liste des items et leurs quantités
        std::cout << i + 1 << ". " << inventory[i].getName() 
                  << " (x" << inventory[i].getQuantity() << ") - "
                  << inventory[i].getType() << " " << inventory[i].getValue() << " HP" << std::endl;
    }
}

void Player::useItemFromInventory(int index) {
    // L'index passé par l'utilisateur commence souvent à 1, donc on soustrait 1
    int realIndex = index - 1;
    
    if (realIndex >= 0 && realIndex < static_cast<int>(inventory.size())) {
        Item& item = inventory[realIndex];
        
        if (item.getQuantity() > 0) {
            item.consume();
            if (item.getType() == "HEAL") {
                heal(item.getValue());
                std::cout << "Vous utilisez " << item.getName() << " et recuperez " << item.getValue() << " HP!" << std::endl;
            }
        } else {
            std::cout << "Vous n'avez plus de " << item.getName() << " !" << std::endl;
        }
    } else {
        std::cout << "Objet invalide." << std::endl;
    }
}

void Player::addVictory(bool killedMonster) {
    victories++; // La victoire augmente de 1 [cite: 112]
    if (killedMonster) {
        monstersKilled++;
    } else {
        monstersSpared++;
    }
}