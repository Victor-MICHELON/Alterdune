#include "Game.h"
#include "DisplayManager.h" // Lien crucial
#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <algorithm>

Game::Game(const std::string& name, std::vector<Item> items, std::vector<Monster*> monsters)
    : player(name), monsterPool(monsters), actCatalog(getActCatalog()) {
    for (const auto& item : items)
        player.addItem(item);
}

Game::~Game() {
    for (Monster* m : monsterPool) delete m;
    for (Monster* m : bestiary)    delete m;
}

void Game::pushLog(const std::string& msg) {
    combatLog.push_back(msg);
    if (combatLog.size() > 8)
        combatLog.erase(combatLog.begin());
}

void Game::run() {
    int choice = 0;
    while (player.getVictories() < 10 && choice != 5) {
        DisplayManager::renderMainMenu(player);
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        switch (choice) {
            case 1: startCombat();   break;
            case 2: showBestiary();  break;
            case 3: showStats();     break;
            case 4: showItems();     break;
            case 5: break;
            default: break;
        }
    }
    DisplayManager::renderEndScreen(player);
}

void Game::showStats() { DisplayManager::renderStats(player); }
void Game::showBestiary() const { DisplayManager::renderBestiary(bestiary); }

void Game::showItems() {
    DisplayManager::renderItems(player);
    int choice;
    std::cin >> choice;
    if (choice != 0) player.useItemFromInventory(choice);
}

void Game::startCombat() {
    if (monsterPool.empty()) { pushLog("Aucun monstre disponible."); return; }
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<int> dist(0, (int)monsterPool.size() - 1);
    Monster* m = monsterPool[dist(rng)];
    runCombat(m);
}

void Game::runCombat(Monster* monster) {
    combatLog.clear();
    pushLog("Combat engage contre " + monster->getName() + " !");
    bool combatOver = false;
    bool monsterSpared = false;

    while (monster->isAlive() && player.isAlive() && !combatOver) {
        DisplayManager::renderCombat(player, *monster, combatLog);
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        switch (choice) {
            case 1: playerFight(monster); break;
            case 2: playerAct(monster);   break;
            case 3: playerItem();         break;
            case 4: playerMercy(monster, combatOver, monsterSpared); break;
            default: pushLog("Choix invalide, tour perdu."); break;
        }

        if (monster->isAlive() && player.isAlive() && !combatOver) {
            int roll = rand() % 100;
            if (roll < 20 && !monster->getAvailableActs().empty()) {
                const std::vector<std::string>& acts = monster->getAvailableActs();
                const std::string& actId = acts[rand() % acts.size()];
                if (actCatalog.count(actId)) {
                    const ActAction& action = actCatalog.at(actId);
                    pushLog(monster->getName() + " utilise " + actId + " !");
                    pushLog(action.name);
                    monster->receiveAct(action.mercyImpact);
                }
            } else {
                int dmg = monster->getAttack();
                if (dmg == 0) pushLog(monster->getName() + " tente une attaque... et rate !");
                else {
                    pushLog(monster->getName() + " inflige " + std::to_string(dmg) + " degats !");
                    player.takeDamage(dmg);
                }
            }

            if (!player.isAlive()) {
                pushLog("Vous etes mort. Game over.");
                DisplayManager::renderCombat(player, *monster, combatLog);
                std::cout << "\nAppuyez sur Entree pour quitter..." << std::flush;
                std::cin.ignore(1000, '\n');
                std::cin.get();
                exit(0);
            }
        }

        if (!monster->isAlive() && !combatOver) {
            combatOver = true;
            monsterSpared = false;
        }
    }

    if (combatOver || !monster->isAlive()) {
        player.addVictory(!monsterSpared);
        pushLog("Victoire ! (" + std::to_string(player.getVictories()) + "/10)");
        DisplayManager::renderCombat(player, *monster, combatLog);
        std::cout << "\nAppuyez sur Entree pour continuer..." << std::flush;
        std::cin.ignore(1000, '\n');
        std::cin.get();

        bestiary.push_back(monster);
        monsterPool.erase(std::remove(monsterPool.begin(), monsterPool.end(), monster), monsterPool.end());
    }
}

void Game::playerFight(Monster* monster) {
    int dmg = rollDamage(monster->getHpMax());
    if (dmg == 0) pushLog("Vous attaquez... mais vous ratez !");
    else {
        pushLog("Vous infligez " + std::to_string(dmg) + " degats a " + monster->getName() + " !");
        monster->takeDamage(dmg);
        if (!monster->isAlive()) pushLog(monster->getName() + " est vaincu !");
    }
}

void Game::playerAct(Monster* monster) {
    const std::vector<std::string>& acts = monster->getAvailableActs();
    DisplayManager::renderActMenu(player, *monster, combatLog, acts, actCatalog);
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        pushLog("Choix invalide, tour perdu.");
        return;
    }
    int index = choice - 1;
    if (index < 0 || index >= static_cast<int>(acts.size())) {
        pushLog("Choix invalide, tour perdu.");
        return;
    }
    const std::string& actId = acts[index];
    if (actCatalog.count(actId)) {
        const ActAction& action = actCatalog.at(actId);
        pushLog("Vous utilisez " + actId + ".");
        pushLog(action.name);
        monster->receiveAct(action.mercyImpact);
    } else pushLog("Action inconnue.");
}

void Game::playerItem() {
    DisplayManager::renderItems(player);
    int choice;
    std::cin >> choice;
    if (choice == 0) { pushLog("Vous n'utilisez rien."); return; }
    player.useItemFromInventory(choice);
    pushLog("Vous utilisez un item.");
}

void Game::playerMercy(Monster* monster, bool& combatOver, bool& monsterSpared) {
    if (monster->canBeSpared()) {
        monster->spare();
        combatOver = true;
        monsterSpared = true;
        pushLog(monster->getName() + " a ete epargne !");
    } else pushLog("Mercy insuffisant. Continuez les actions ACT !");
}

int Game::rollDamage(int hpMax) const { return rand() % (hpMax + 1); }