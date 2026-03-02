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

public:
    Game(const std::string& name, std::vector<Item> items, std::vector<Monster*> monsters);
    ~Game();

    void run();
    void showStats() const;
    void showBestiary() const;
    void showItems();
    void startCombat();
    void runCombat(Monster* m);
};

#endif