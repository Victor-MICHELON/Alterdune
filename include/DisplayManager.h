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
    static void renderTitleScreen();                          
    static void renderNamePrompt();                    
    static void renderMainMenu(const Player& player);

    // Combat
    static void renderCombat(const Player& player,
                             const Monster& monster,
                             const std::vector<std::string>& logs,
                             const std::string& prompt = "");

    static void renderActMenu(const Player& player,
                              const Monster& monster,
                              const std::vector<std::string>& logs,
                              const std::vector<std::string>& acts,
                              const std::map<std::string, ActAction>& catalog);

    static void renderBestiary(const std::vector<Monster*>& bestiary);
    static void renderStats(const Player& player);
    static void renderItems(const Player& player);

    // Fins de partie
    static void renderEndScreen(const Player& player);       
    static void renderDeathScreen(const Player& player,      
                                  const Monster& killer);
    static void renderVictoryFlash(const Monster& monster,
                                   bool spared);

private:
    // Dimensions
    static const int W  = 60;
    static const int CW = 26;  

    // Primitives de rendu
    static std::string repeat(char c, int n);
    static std::string border(char l, char m, char r, int w = W);
    static std::string line(const std::string& content, int w = W);
    static std::string centerText(const std::string& s, int w = W);
    static std::string pad(const std::string& s, int width);
    static std::string bar(int cur, int max, int width,
                           char fill = '#', char empty = '.');

    
    static std::vector<std::string> playerSprite();
    static std::vector<std::string> monsterSprite(const std::string& category);
    static std::vector<std::string> deadSprite();

    // Utilitaires 
    static int visLen(const std::string& s);
    static void waitEnter(const std::string& msg = "");
    static void animateLine(const std::string& text, int delayMs = 18);
};