#include "Game.h"
#include <iostream>

Game::Game(const std::string& name, std::vector<Item> items, std::vector<Monster*> monsters)
    : player(name), monsterPool(monsters), actCatalog(getActCatalog()) {
    for (const auto& item : items) {
        player.addItem(item);
    }
}

Game::~Game() {
    for (Monster* m : monsterPool) delete m;
    for (Monster* m : bestiary) delete m;
}

void Game::run() {
    int choice = 0;
    while (player.getVictories() < 10 && choice != 5) {
        std::cout << "\n=== MENU PRINCIPAL ===" << std::endl;
        std::cout << "1. Combat\n2. Bestiaire\n3. Stats\n4. Items\n5. Quitter" << std::endl;
        std::cout << "Choix : ";
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1: startCombat(); break;
            case 2: showBestiary(); break;
            case 3: showStats(); break;
            case 4: showItems(); break;
            case 5: break;
        }
    }
}

void Game::showStats() const {
    player.displayStats();
}

void Game::showItems() {
    player.displayInventory();
}

void Game::showBestiary() const {
    if (bestiary.empty()) {
        std::cout << "Bestiaire vide." << std::endl;
        return;
    }
    for (Monster* m : bestiary) m->displayStats();
}

void Game::startCombat() {
    // Logique de sélection de monstre aléatoire ici
    std::cout << "Recherche d'un adversaire..." << std::endl;
}

void Game::runCombat(Monster* m) {
    // Sera implémenté en Phase 5
}