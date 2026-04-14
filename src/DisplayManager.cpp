#include "DisplayManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef _WIN32
  #include <windows.h>
  static bool ansiEnabled = false;
  static void enableAnsiWindows() {
      if (ansiEnabled) return;
      HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
      DWORD dwMode = 0;
      if (GetConsoleMode(hOut, &dwMode)) {
          SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
      }
      ansiEnabled = true;
  }
#endif

namespace Ansi {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string DIM     = "\033[2m";
    const std::string WHITE   = "\033[97m";
    const std::string GRAY    = "\033[90m";
    const std::string GREEN   = "\033[92m";
    const std::string YELLOW  = "\033[93m";
    const std::string RED     = "\033[91m";
    const std::string CYAN    = "\033[96m";
    const std::string MAGENTA = "\033[95m";
    const std::string BLUE    = "\033[94m";
}

std::string DisplayManager::repeat(char c, int n) {
    if (n <= 0) return "";
    return std::string(n, c);
}

std::string DisplayManager::border(char left, char mid, char right) {
    return std::string(1, left) + repeat(mid, W) + std::string(1, right) + "\n";
}

std::string DisplayManager::line(const std::string& content) {
    std::string stripped;
    bool inEsc = false;
    for (char c : content) {
        if (c == '\033') { inEsc = true; continue; }
        if (inEsc) { if (c == 'm') inEsc = false; continue; }
        stripped += c;
    }
    int visLen = static_cast<int>(stripped.size());
    int padding = W - 2 - visLen;
    if (padding < 0) padding = 0;
    return "| " + content + repeat(' ', padding) + " |\n";
}

std::string DisplayManager::pad(const std::string& s, int width) {
    int visLen = 0;
    bool inEsc = false;
    for (char c : s) {
        if (c == '\033') { inEsc = true; continue; }
        if (inEsc) { if (c == 'm') inEsc = false; continue; }
        visLen++;
    }
    int padding = width - visLen;
    if (padding <= 0) return s;
    return s + std::string(padding, ' ');
}

std::string DisplayManager::bar(int current, int max, int width, char fill, char empty) {
    if (max <= 0) return std::string(width + 2, empty);
    int filled = std::max(0, std::min(current * width / max, width));
    std::string out = "[";
    out += repeat(fill, filled);
    out += repeat(empty, width - filled);
    out += "]";
    return out;
}

void DisplayManager::clearScreen() {
#ifdef _WIN32
    enableAnsiWindows();
#endif
    std::cout << "\033[2J\033[H" << std::flush;
}

void DisplayManager::renderMainMenu(const Player& player) {
    std::ostringstream buf;
    buf << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << line(pad("  A L T E R D U N E", W - 2)) << border('+', '=', '+') << Ansi::RESET;
    
    std::ostringstream hpLine;
    int hpPct = (player.getHpMax() > 0) ? player.getHpCurrent() * 100 / player.getHpMax() : 0;
    std::string hpColor = (hpPct > 50) ? Ansi::GREEN : (hpPct > 20) ? Ansi::YELLOW : Ansi::RED;
    hpLine << Ansi::BOLD << Ansi::WHITE << player.getName() << Ansi::RESET << "   HP: " << hpColor << Ansi::BOLD << std::setw(3) << player.getHpCurrent() << "/" << player.getHpMax() << Ansi::RESET << "  " << hpColor << bar(player.getHpCurrent(), player.getHpMax(), 18) << Ansi::RESET;
    buf << line(hpLine.str());
    
    std::ostringstream vicLine;
    vicLine << Ansi::CYAN << "Victoires : " << player.getVictories() << "/10" << Ansi::RESET << "   " << Ansi::GREEN << "Eparges: " << player.getMonstersSpared() << Ansi::RESET << "  " << Ansi::RED << "Tues: " << player.getMonstersKilled() << Ansi::RESET;
    buf << line(vicLine.str());
    
    buf << border('+', '-', '+') << line(Ansi::YELLOW + Ansi::BOLD + "  MENU PRINCIPAL" + Ansi::RESET) << border('+', '-', '+');
    buf << line(Ansi::WHITE + " [1] " + Ansi::RESET + "Combat") << line(Ansi::WHITE + " [2] " + Ansi::RESET + "Bestiaire") << line(Ansi::WHITE + " [3] " + Ansi::RESET + "Stats") << line(Ansi::WHITE + " [4] " + Ansi::RESET + "Items") << line(Ansi::WHITE + " [5] " + Ansi::RESET + "Quitter") << border('+', '=', '+') << "Choix : ";
    std::cout << "\033[2J\033[H" << buf.str() << std::flush;
}

void DisplayManager::renderCombat(const Player& player, const Monster& monster, const std::vector<std::string>& logs, const std::string& prompt) {
    std::ostringstream buf;
    buf << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << Ansi::RESET;
    std::ostringstream h;
    h << Ansi::CYAN << Ansi::BOLD << "  COMBAT vs " << Ansi::YELLOW << monster.getName() << Ansi::GRAY << "  [" << monster.getCategory() << "]" << Ansi::RESET;
    buf << line(h.str()) << border('+', '-', '+');

    int hpPctP = (player.getHpMax() > 0) ? player.getHpCurrent() * 100 / player.getHpMax() : 0;
    std::string pColor = (hpPctP > 50) ? Ansi::GREEN : (hpPctP > 20) ? Ansi::YELLOW : Ansi::RED;
    std::ostringstream rowP;
    rowP << Ansi::BOLD << Ansi::WHITE << pad(player.getName(), 14) << Ansi::RESET << " HP " << pColor << Ansi::BOLD << std::setw(3) << player.getHpCurrent() << "/" << std::setw(3) << player.getHpMax() << Ansi::RESET << " " << pColor << bar(player.getHpCurrent(), player.getHpMax(), 14) << Ansi::RESET;
    buf << line(rowP.str());

    int hpPctM = (monster.getHpMax() > 0) ? monster.getHpCurrent() * 100 / monster.getHpMax() : 0;
    std::string mColor = (hpPctM > 50) ? Ansi::GREEN : (hpPctM > 20) ? Ansi::YELLOW : Ansi::RED;
    std::ostringstream rowM;
    rowM << Ansi::BOLD << Ansi::MAGENTA << pad(monster.getName(), 14) << Ansi::RESET << " HP " << mColor << Ansi::BOLD << std::setw(3) << monster.getHpCurrent() << "/" << std::setw(3) << monster.getHpMax() << Ansi::RESET << " " << mColor << bar(monster.getHpCurrent(), monster.getHpMax(), 14) << Ansi::RESET;
    buf << line(rowM.str());

    int mPct = (monster.getMercyGoal() > 0) ? monster.getMercyCurrent() * 100 / monster.getMercyGoal() : 0;
    std::string merColor = (mPct >= 100) ? Ansi::GREEN : (mPct >= 50) ? Ansi::YELLOW : Ansi::GRAY;
    std::ostringstream mRow;
    mRow << Ansi::BOLD << "Mercy" << Ansi::RESET << "           " << merColor << Ansi::BOLD << std::setw(3) << monster.getMercyCurrent() << "/" << std::setw(3) << monster.getMercyGoal() << Ansi::RESET << " " << merColor << bar(monster.getMercyCurrent(), monster.getMercyGoal(), 14, '*', '.') << Ansi::RESET;
    buf << line(mRow.str());

    buf << border('+', '-', '+') << line(Ansi::BOLD + Ansi::GRAY + " Journal" + Ansi::RESET);
    int logStart = std::max(0, (int)logs.size() - 4);
    for (int i = logStart; i < (int)logs.size(); ++i) buf << line(" " + Ansi::DIM + logs[i] + Ansi::RESET);
    for (int i = (int)logs.size() - logStart; i < 4; ++i) buf << line("");

    buf << border('+', '-', '+');
    std::ostringstream actions;
    actions << Ansi::WHITE << "[1] FIGHT" << Ansi::RESET << "  " << Ansi::WHITE << "[2] ACT" << Ansi::RESET << "  " << Ansi::WHITE << "[3] ITEM" << Ansi::RESET << "  " << Ansi::WHITE << "[4] MERCY" << Ansi::RESET;
    buf << line(actions.str()) << border('+', '=', '+');
    
    if (!prompt.empty()) buf << prompt;
    else buf << "Choix (1/2/3/4) : ";
    std::cout << "\033[2J\033[H" << buf.str() << std::flush;
}

void DisplayManager::renderActMenu(const Player& player, const Monster& monster, const std::vector<std::string>& logs, const std::vector<std::string>& acts, const std::map<std::string, ActAction>& catalog) {
    renderCombat(player, monster, logs, "");
    std::ostringstream buf;
    buf << "\n" << Ansi::BOLD << Ansi::YELLOW << border('+', '-', '+') << Ansi::RESET;
    buf << Ansi::BOLD << "| " << pad(" ACTIONS ACT", W - 2) << " |\n" << Ansi::RESET << Ansi::YELLOW << border('+', '-', '+') << Ansi::RESET;

    for (size_t i = 0; i < acts.size(); ++i) {
        const std::string& actId = acts[i];
        std::ostringstream row;
        row << Ansi::WHITE << "[" << i + 1 << "] " << Ansi::RESET << Ansi::BOLD << pad(actId, 14) << Ansi::RESET;
        if (catalog.count(actId)) {
            int impact = catalog.at(actId).mercyImpact;
            std::string sign  = (impact >= 0) ? "+" : "";
            std::string color = (impact >= 0) ? Ansi::GREEN : Ansi::RED;
            row << color << "Mercy " << sign << impact << Ansi::RESET;
        }
        buf << line(row.str());
    }
    buf << Ansi::YELLOW << border('+', '-', '+') << Ansi::RESET << "Choix : ";
    std::cout << buf.str() << std::flush;
}

void DisplayManager::renderBestiary(const std::vector<Monster*>& bestiary) {
    std::ostringstream buf;
    buf << "\033[2J\033[H" << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << Ansi::RESET << line(Ansi::BOLD + "  BESTIAIRE" + Ansi::RESET) << border('+', '-', '+');
    if (bestiary.empty()) {
        buf << line(Ansi::GRAY + "  Aucune rencontre enregistree." + Ansi::RESET);
    } else {
        for (const Monster* m : bestiary) {
            std::ostringstream row;
            std::string result = m->isSpared() ? Ansi::GREEN + "[Epargne]" + Ansi::RESET : Ansi::RED + "[Tue]" + Ansi::RESET;
            row << Ansi::BOLD << Ansi::MAGENTA << pad(m->getName(), 16) << Ansi::RESET << Ansi::GRAY << "[" << m->getCategory() << "]" << Ansi::RESET << "  " << result;
            buf << line(row.str());
            std::ostringstream stats;
            stats << Ansi::DIM << "  HP: " << m->getHpMax() << "  ATK: " << m->getAttack() << "  DEF: " << m->getDefense() << Ansi::RESET;
            buf << line(stats.str()) << line(Ansi::GRAY + std::string(W - 4, '-') + Ansi::RESET);
        }
    }
    buf << Ansi::CYAN << border('+', '=', '+') << Ansi::RESET << "Appuyez sur Entree pour continuer...";
    std::cout << buf.str() << std::flush;
    std::cin.ignore(1000, '\n');
    std::cin.get();
}

void DisplayManager::renderStats(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H" << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << Ansi::RESET << line(Ansi::BOLD + "  STATISTIQUES" + Ansi::RESET) << border('+', '-', '+');
    int hpPct = (player.getHpMax() > 0) ? player.getHpCurrent() * 100 / player.getHpMax() : 0;
    std::string hpColor = (hpPct > 50) ? Ansi::GREEN : (hpPct > 20) ? Ansi::YELLOW : Ansi::RED;
    buf << line(Ansi::BOLD + Ansi::WHITE + "  Personnage : " + Ansi::RESET + player.getName());
    std::ostringstream hp;
    hp << "  HP      : " << hpColor << Ansi::BOLD << player.getHpCurrent() << "/" << player.getHpMax() << Ansi::RESET << "  " << hpColor << bar(player.getHpCurrent(), player.getHpMax(), 20) << Ansi::RESET;
    buf << line(hp.str());
    buf << line("  ATK     : " + Ansi::YELLOW + Ansi::BOLD + std::to_string(player.getAttack()) + Ansi::RESET);
    buf << line("  DEF     : " + Ansi::BLUE   + Ansi::BOLD + std::to_string(player.getDefense()) + Ansi::RESET);
    buf << border('+', '-', '+');
    buf << line("  Victoires  : " + Ansi::CYAN + std::to_string(player.getVictories()) + "/10" + Ansi::RESET);
    buf << line("  Tues       : " + Ansi::RED   + std::to_string(player.getMonstersKilled())  + Ansi::RESET);
    buf << line("  Eparges    : " + Ansi::GREEN  + std::to_string(player.getMonstersSpared()) + Ansi::RESET);
    buf << Ansi::CYAN << border('+', '=', '+') << Ansi::RESET << "Appuyez sur Entree pour continuer...";
    std::cout << buf.str() << std::flush;
    std::cin.ignore(1000, '\n');
    std::cin.get();
}

void DisplayManager::renderItems(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H" << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << Ansi::RESET << line(Ansi::BOLD + "  INVENTAIRE" + Ansi::RESET) << border('+', '-', '+');
    const auto& inv = player.getInventory(); 
    if (inv.empty()) {
        buf << line(Ansi::GRAY + "  Votre inventaire est vide." + Ansi::RESET);
    } else {
        for (size_t i = 0; i < inv.size(); ++i) {
            std::ostringstream row;
            row << Ansi::WHITE << "  [" << i + 1 << "] " << Ansi::RESET << Ansi::BOLD << pad(inv[i].getName(), 16) << Ansi::RESET << Ansi::GREEN << "x" << inv[i].getQuantity() << Ansi::RESET << Ansi::GRAY  << "  " << inv[i].getType() << " +" << inv[i].getValue() << " HP" << Ansi::RESET;
            buf << line(row.str());
        }
    }
    buf << Ansi::CYAN << border('+', '=', '+') << Ansi::RESET << "Utiliser un item ? (0 pour annuler) : ";
    std::cout << buf.str() << std::flush;
}

void DisplayManager::renderEndScreen(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H" << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << Ansi::RESET << line(Ansi::BOLD + Ansi::YELLOW + "    FIN DE PARTIE — 10 VICTOIRES" + Ansi::RESET) << border('+', '-', '+');
    buf << line("  Monstres tues    : " + Ansi::RED   + std::to_string(player.getMonstersKilled())  + Ansi::RESET);
    buf << line("  Monstres eparges : " + Ansi::GREEN + std::to_string(player.getMonstersSpared()) + Ansi::RESET) << border('+', '-', '+');
    
    if (player.getMonstersKilled() == 0) {
        buf << line(Ansi::GREEN + Ansi::BOLD + "  *** FIN PACIFISTE ***" + Ansi::RESET) << line("  Tu as epargne chaque ame rencontree.") << line("  La paix regne sur Alterdune.");
    } else if (player.getMonstersSpared() == 0) {
        buf << line(Ansi::RED + Ansi::BOLD + "  *** FIN GENOCIDAIRE ***" + Ansi::RESET) << line("  Tu as annihile tout ce qui croisait ta route.") << line("  Le silence s'est installe sur Alterdune.");
    } else {
        buf << line(Ansi::YELLOW + Ansi::BOLD + "  *** FIN NEUTRE ***" + Ansi::RESET) << line("  Tu as tue et epargne. Un chemin ambigu.") << line("  Alterdune se souvient de tes choix.");
    }
    buf << Ansi::CYAN << Ansi::BOLD << border('+', '=', '+') << Ansi::RESET;
    std::cout << buf.str() << std::flush;
}