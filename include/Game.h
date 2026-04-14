#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include <string>
#include "Player.h"
#include "Monster.h"
#include "Item.h"
#include "ActCatalog.h"

class Game {
private:
    Player player;
    std::vector<Monster*> monsterPool;
    std::vector<Monster*> bestiary;
    const std::map<std::string, ActAction>& actCatalog;
    std::vector<std::string> combatLog;

    void pushLog(const std::string& msg);

public:
    Game(const std::string& name, std::vector<Item> items, std::vector<Monster*> monsters);
    ~Game();

    void run();
    void showStats();
    void showBestiary() const;
    void showItems();
    void startCombat();
    void runCombat(Monster* m);
    void playerFight(Monster* m);
    void playerAct(Monster* m);
    void playerItem();
    void playerMercy(Monster* m, bool& combatOver, bool& monsterSpared);
    int  rollDamage(int hpMax) const;
};

#endif