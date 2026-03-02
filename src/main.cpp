#include <iostream>
#include <string>
#include <vector>
#include "Game.h"
#include "FileLoader.h"
#include "Item.h"
#include "Monster.h"

int main() {
    std::cout << "=== BIENVENUE DANS ALTERDUNE ===" << std::endl;

    std::string playerName;
    std::cout << "Entrez le nom de votre personnage : ";
    std::cin >> playerName;

    std::vector<Item> initialItems;
    std::vector<Monster*> monsterPool;

    try {
        // Chargement des fichiers CSV
        initialItems = FileLoader::loadItems("data/items.csv");
        monsterPool = FileLoader::loadMonsters("data/monsters.csv");
    } catch (const std::exception& e) {
        std::cerr << "Erreur lors du chargement des donnees : " << e.what() << std::endl;
        return 1;
    }

    // Création et lancement du jeu
    Game game(playerName, initialItems, monsterPool);
    game.run();

    return 0;
}