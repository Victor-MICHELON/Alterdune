#pragma once
#include <string>
#include <vector>
#include <map>
#include "Player.h"
#include "Monster.h"
#include "ActCatalog.h"

class DisplayManager {
public:
    static void clearScreen();
    static void renderMainMenu(const Player& player);
    static void renderCombat(const Player& player, const Monster& monster,
                             const std::vector<std::string>& logs, const std::string& prompt = "");
    static void renderActMenu(const Player& player, const Monster& monster,
                              const std::vector<std::string>& logs, const std::vector<std::string>& acts,
                              const std::map<std::string, ActAction>& catalog);
    static void renderBestiary(const std::vector<Monster*>& bestiary);
    static void renderStats(const Player& player);
    static void renderItems(const Player& player);
    static void renderEndScreen(const Player& player);

private:
    static const int W = 52;
    static std::string bar(int current, int max, int width, char fill = '#', char empty = '.');
    static std::string pad(const std::string& s, int width);
    static std::string repeat(char c, int n);
    static std::string border(char left, char mid, char right);
    static std::string line(const std::string& content);
};