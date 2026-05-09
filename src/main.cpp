#include <iostream>
#include <string>
#include <vector>
#include "Game.h"
#include "FileLoader.h"
#include "Item.h"
#include "Monster.h"
#include "DisplayManager.h"

int main() {
    // Écran titre animé
    DisplayManager::renderTitleScreen();

    // Saisie du nom
    DisplayManager::clearScreen();

    // encadré pour la saisie du nom
    const int W = 60;
    auto border = [&](char l, char m, char r) {
        std::cout << l << std::string(W, m) << r << "\n";
    };
    auto cline = [&](const std::string& s) {
        int vl = 0; bool esc = false;
        for (unsigned char c : s) {
            if (c=='\033'){esc=true;continue;}
            if (esc){if(c=='m')esc=false;continue;}
            vl++;
        }
        int pad = W - 2 - vl;
        if (pad < 0) pad = 0;
        std::cout << "| " << s << std::string(pad, ' ') << " |\n";
    };

    std::cout << "\033[96m\033[1m";
    border('+', '=', '+');
    std::cout << "\033[0m";
    cline("\033[96m\033[1m  CRÉER VOTRE AVENTURIER\033[0m");
    std::cout << "\033[96m";
    border('+', '-', '+');
    std::cout << "\033[0m";
    cline("");
    cline("\033[90m  Entrez le nom de votre personnage.\033[0m");
    cline("");
    std::cout << "\033[96m\033[1m";
    border('+', '=', '+');
    std::cout << "\033[0m";
    std::cout << "\n  Nom : ";

    std::string playerName;
    std::cin >> playerName;

    // ── Chargement des données 
    std::vector<Item> initialItems;
    std::vector<Monster*> monsterPool;

        try {
        initialItems = FileLoader::loadItems("data/items.csv");
        monsterPool = FileLoader::loadMonsters("data/monsters.csv");
    } catch (const std::exception& e) {
        std::cerr << "Erreur chargement données : " << e.what() << std::endl;
        return 1;
    }

    // Lancement du jeu
    Game game(playerName, initialItems, monsterPool);
    game.run();

    return 0;
}