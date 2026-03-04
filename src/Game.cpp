#include "Game.h"
#include <iostream>
#include <algorithm>   // pour std::remove
#include <random>      // pour rollDamage
#include <ctime>       // pour time()

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
    std::cout << "\nJoueur : " << player.getName() << std::endl;
    std::cout << "HP : " << player.getHpCurrent() << " / " << player.getHpMax() << std::endl;
    player.displayInventory();
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
    // 10 victoires atteintes → fin de partie
    std::cout << "\n========================================" << std::endl;
    std::cout << "        FIN DE PARTIE - 10 VICTOIRES    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Monstres tues    : " << player.getMonstersKilled() << std::endl;
    std::cout << "Monstres epargnes: " << player.getMonstersSpared() << std::endl;
    std::cout << std::endl;

    if (player.getMonstersKilled() == 0) {
        std::cout << "*** FIN PACIFISTE ***" << std::endl;
        std::cout << "Tu as epargne chaque ame rencontree." << std::endl;
        std::cout << "La paix regne sur Alterdune." << std::endl;
    } else if (player.getMonstersSpared() == 0) {
        std::cout << "*** FIN GENOCIDAIRE ***" << std::endl;
        std::cout << "Tu as annihile tout ce qui croisait ta route." << std::endl;
        std::cout << "Le silence s'est installe sur Alterdune." << std::endl;
    } else {
        std::cout << "*** FIN NEUTRE ***" << std::endl;
        std::cout << "Tu as tue et epargne. Un chemin ambigu." << std::endl;
        std::cout << "Alterdune se souvient de tes choix." << std::endl;
    }

    std::cout << "========================================" << std::endl;
}

void Game::showStats() const {
    player.displayStats();
}

void Game::showItems() {
    player.displayInventory();
    std::cout << "Utiliser un item ? (0 pour annuler) : " << std::endl;
    int choice;
    std::cin >> choice;
    if (choice != 0) {
        player.useItemFromInventory(choice);
    }
}

void Game::showBestiary() const {
    if (bestiary.empty()) {
        std::cout << "Bestiaire vide." << std::endl;
        return;
    }
    std::cout << "=== BESTIAIRE ===" << std::endl;
    for (Monster* m : bestiary) {
        m->displayStats();
        std::cout << "Resultat : " << (m->isSpared() ? "Epargne" : "Tue") << std::endl;
    }
}

void Game::startCombat() {
    if (monsterPool.empty()) {
        std::cout << "\nAucun monstre disponible." << std::endl;
        return;
    }
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<int> dist(0, monsterPool.size()-1);
    Monster* m = monsterPool[dist(rng)];
    runCombat(m);
}

void Game::runCombat(Monster* monster) {
    std::cout << "\n=== COMBAT CONTRE " << monster->getName() << " ===" << std::endl;

    bool combatOver = false;
    bool monsterSpared = false;

    while (monster->isAlive() && player.isAlive() && !combatOver) {
        // Affichage état
        std::cout << "\n";
        player.displayStats();
        monster->displayStats();

        // Menu combat
        std::cout << "\n[FIGHT]  [ACT]  [ITEM]  [MERCY]" << std::endl;
        std::cout << "Choix (1/2/3/4) : ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: playerFight(monster); break;
            case 2: playerAct(monster); break;
            case 3: playerItem(); break;
            case 4: playerMercy(monster, combatOver, monsterSpared); break;
            default: std::cout << "Choix invalide, tour perdu." << std::endl;
        }

        // Tour du monstre (si toujours vivant et combat pas fini)
        // Tour du monstre (si toujours vivant et combat pas fini)
        if (monster->isAlive() && player.isAlive() && !combatOver) {

            // 20% de chance d'utiliser une action ACT du monstre
            int roll = rand() % 100;

            if (roll < 20 && !monster->getAvailableActs().empty()) {
                // Le monstre utilise une de ses actions ACT au hasard
                const std::vector<std::string>& acts = monster->getAvailableActs();
                int actIndex = rand() % acts.size();
                const std::string& actId = acts[actIndex];

                if (actCatalog.count(actId)) {
                    const ActAction& action = actCatalog.at(actId);
                    std::cout << monster->getName() << " utilise " << actId << " !" << std::endl;
                    std::cout << action.name << std::endl;
                    monster->receiveAct(action.mercyImpact);  // ← Mercy mise à jour
                    std::cout << "Mercy : " << monster->getMercyCurrent()
                              << " / " << monster->getMercyGoal() << std::endl;
                }
            } else {
                // 80% : le monstre attaque avec sa valeur d'attaque fixe
                int dmg = monster->getAttack();
                if (dmg == 0) {
                    std::cout << monster->getName() << " tente une attaque... mais rate !" << std::endl;
                } else {
                    std::cout << monster->getName() << " attaque et inflige " << dmg << " degats !" << std::endl;
                    player.takeDamage(dmg);
                }
            }

            if (!player.isAlive()) {
                std::cout << "\nVous etes mort. Game over." << std::endl;
                exit(0);
            }
        }

        // Monstre tué par FIGHT ?
        if (!monster->isAlive() && !combatOver) {
            combatOver = true;
            monsterSpared = false;
        }
    }

    if (combatOver || !monster->isAlive()) {
        player.addVictory(!monsterSpared);
        std::cout << "\nVictoire ! (" << player.getVictories() << "/10)" << std::endl;

        // Copie dans le bestiaire
        Monster* copy = monster; // on garde le pointeur tel quel
        bestiary.push_back(copy);

        // Retirer du pool pour ne pas le réutiliser
        monsterPool.erase(
            std::remove(monsterPool.begin(), monsterPool.end(), monster),
            monsterPool.end()
        );
    }
}

void Game::playerFight(Monster* monster) {
    int dmg = rollDamage(monster->getHpMax());
    if (dmg == 0) {
        std::cout << "Vous attaquez... mais vous ratez !" << std::endl;
    } else {
        std::cout << "Vous infligez " << dmg << " degats a " << monster->getName() << " !" << std::endl;
        monster->takeDamage(dmg);
        if (!monster->isAlive()) {
            std::cout << monster->getName() << " est vaincu !" << std::endl;
        }
    }
}

void Game::playerAct(Monster* monster) {
    const std::vector<std::string>& acts = monster->getAvailableActs();

    std::cout << "=== ACTIONS ACT ===" << std::endl;
    for (size_t i = 0; i < acts.size(); ++i) {
        const std::string& actId = acts[i];
        if (actCatalog.count(actId)) {
            int impact = actCatalog.at(actId).mercyImpact;
            std::string sign = (impact >= 0) ? "+" : "";
            std::cout << i + 1 << ". " << actId
                      << "  [Mercy : " << sign << impact << "]" << std::endl;
        } else {
            std::cout << i + 1 << ". " << actId << std::endl;
        }
    }
    std::cout << "Choix : ";
    int choice;
    std::cin >> choice;

    int index = choice - 1;
    if (index < 0 || index >= static_cast<int>(acts.size())) {
        std::cout << "Choix invalide, tour perdu." << std::endl;
        return;
    }

    const std::string& actId = acts[index];
    if (actCatalog.count(actId)) {
        const ActAction& action = actCatalog.at(actId);
        std::cout << action.name << std::endl;
        monster->receiveAct(action.mercyImpact);
        std::cout << "Mercy : " << monster->getMercyCurrent()
                  << " / " << monster->getMercyGoal() << std::endl;
    } else {
        std::cout << "Action inconnue." << std::endl;
    }
}

void Game::playerItem() {
    player.displayInventory();
    std::cout << "Quel item utiliser ? (0 pour annuler) : ";
    int choice;
    std::cin >> choice;
    if (choice == 0) return;
    player.useItemFromInventory(choice);
}

void Game::playerMercy(Monster* monster, bool& combatOver, bool& monsterSpared) {
    if (monster->canBeSpared()) {
        monster->spare();
        combatOver = true;
        monsterSpared = true;
        std::cout << monster->getName() << " a ete epargne !" << std::endl;
    } else {
        std::cout << "Mercy insuffisant ("
                  << monster->getMercyCurrent() << " / " << monster->getMercyGoal()
                  << "). Continuez les actions ACT !" << std::endl;
    }
}

int Game::rollDamage(int hpMax) const {
    return rand() % (hpMax + 1);
}
