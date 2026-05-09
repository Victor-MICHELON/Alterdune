#include "Game.h"
#include "DisplayManager.h"
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
    if (combatLog.size() > 10)
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
    if (player.getVictories() >= 10)
        DisplayManager::renderEndScreen(player);
}

void Game::showStats()    { DisplayManager::renderStats(player); }
void Game::showBestiary() const { DisplayManager::renderBestiary(bestiary); }

void Game::showItems() {
    DisplayManager::renderItems(player);
    int choice;
    std::cin >> choice;
    if (choice != 0) {
        player.useItemFromInventory(choice);
        std::cout << "\nAppuyez sur Entree pour continuer..." << std::flush;
        std::cin.ignore(1000, '\n');
        std::cin.get();
    }
}

int Game::rollDamage(int maxVal) const { 
    if (maxVal <= 0) return 0;
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, maxVal);
    return dist(rng); 
}

void Game::startCombat() {
    if (monsterPool.empty()) { pushLog("Aucun monstre disponible."); return; }
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, (int)monsterPool.size() - 1);
    Monster* m = monsterPool[dist(rng)]->clone();
    runCombat(m);
}

void Game::runCombat(Monster* monster) {
    combatLog.clear();
    pushLog("\033[93mCombat engage contre \033[1m" + monster->getName() + "\033[0m\033[93m !\033[0m");
    bool combatOver    = false;
    bool monsterSpared = false;

    static std::mt19937 rng(std::random_device{}());

    while (monster->isAlive() && player.isAlive() && !combatOver) {
        DisplayManager::renderCombat(player, *monster, combatLog);
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case 1: playerFight(monster);                              break;
            case 2: playerAct(monster);                                break;
            case 3: playerItem();                                      break;
            case 4: playerMercy(monster, combatOver, monsterSpared);   break;
            default: pushLog("\033[90mChoix invalide, tour perdu.\033[0m"); break;
        }

        if (!monster->isAlive() && !combatOver) {
            combatOver    = true;
            monsterSpared = false;
        }

        if (monster->isAlive() && player.isAlive() && !combatOver) {
            std::uniform_int_distribution<int> dist100(0, 99);
            int roll = dist100(rng);

            if (roll < 20 && !monster->getAvailableActs().empty()) {
                const std::vector<std::string>& acts = monster->getAvailableActs();
                std::uniform_int_distribution<int> actDist(0, (int)acts.size() - 1);
                const std::string& actId = acts[actDist(rng)];
                if (actCatalog.count(actId)) {
                    const ActAction& action = actCatalog.at(actId);
                    pushLog("\033[35m" + monster->getName() + " reagit : " + actId + "\033[0m");
                    pushLog("\033[2m" + action.name + "\033[0m");
                    monster->receiveAct(action.mercyImpact);
                }
            } else {
                // LOGIQUE CORRIGÉE : Dégâts basés sur l'ATK du monstre
                int dmg = rollDamage(monster->getAttack());
                if (dmg == 0) {
                    pushLog("\033[90m" + monster->getName() + " tente une attaque... et rate !\033[0m");
                } else {
                    pushLog("\033[91m" + monster->getName() + " inflige \033[1m"
                          + std::to_string(dmg) + "\033[0m\033[91m degats !\033[0m");
                    player.takeDamage(dmg);
                }
            }

            if (!player.isAlive()) {
                pushLog("\033[91m\033[1mVous etes mort. GAME OVER.\033[0m");
                DisplayManager::renderCombat(player, *monster, combatLog);
                DisplayManager::renderDeathScreen(player, *monster);
                delete monster;
                exit(0);
            }
        }
    }

    if (combatOver || !monster->isAlive()) {
        player.addVictory(!monsterSpared); 
        
        pushLog("\033[96mVictoire ! (" + std::to_string(player.getVictories()) + "/10)\033[0m");
        DisplayManager::renderVictoryFlash(*monster, monsterSpared);
        bestiary.push_back(monster);
    }
}

void Game::playerFight(Monster* monster) {
    
    int dmg = rollDamage(monster->getHpMax());
    
    if (dmg == 0) {
        pushLog("\033[90mVous attaquez... mais vous ratez !\033[0m");
    } else {
        pushLog("\033[92mVous infligez \033[1m" + std::to_string(dmg)
              + "\033[0m\033[92m degats a " + monster->getName() + " !\033[0m");
        monster->takeDamage(dmg);
        
        if (!monster->isAlive())
            pushLog("\033[91m\033[1m" + monster->getName() + " est vaincu !\033[0m");
    }
}


void Game::playerAct(Monster* monster) {
    const std::vector<std::string>& acts = monster->getAvailableActs();
    int cap = std::min((int)acts.size(), monster->getActCapacity());
    std::vector<std::string> cappedActs(acts.begin(), acts.begin() + cap);

    DisplayManager::renderActMenu(player, *monster, combatLog, cappedActs, actCatalog);
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        pushLog("\033[90mChoix invalide, tour perdu.\033[0m");
        return;
    }
    int index = choice - 1;
    if (index < 0 || index >= (int)cappedActs.size()) {
        pushLog("\033[90mChoix invalide, tour perdu.\033[0m");
        return;
    }
    const std::string& actId = cappedActs[index];
    if (actCatalog.count(actId)) {
        const ActAction& action = actCatalog.at(actId);
        int imp = action.mercyImpact;
        std::string impStr = (imp >= 0 ? "\033[92m+" : "\033[91m") + std::to_string(imp) + " Mercy\033[0m";
        pushLog("\033[96mVous utilisez " + actId + ". " + impStr);
        pushLog("\033[2m" + action.name + "\033[0m");
        monster->receiveAct(imp);
    } else {
        pushLog("\033[90mAction inconnue.\033[0m");
    }
}

void Game::playerItem() {
    DisplayManager::renderItems(player);
    int choice;
    std::cin >> choice;
    if (choice == 0) {
        pushLog("\033[90mVous n'utilisez rien.\033[0m");
        return;
    }
    player.useItemFromInventory(choice);
    pushLog("\033[92mVous utilisez un item. HP restaurels !\033[0m");
}

void Game::playerMercy(Monster* monster, bool& combatOver, bool& monsterSpared) {
    if (monster->canBeSpared()) {
        monster->spare();
        combatOver    = true;
        monsterSpared = true;
        pushLog("\033[92m\033[1m" + monster->getName() + " a ete epargne ! ☮\033[0m");
    } else {
        int need = monster->getMercyGoal() - monster->getMercyCurrent();
        pushLog("\033[93mMercy insuffisant. Il manque " + std::to_string(need) + " points.\033[0m");
    }
}
