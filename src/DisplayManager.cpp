#include "DisplayManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>

#ifdef _WIN32
  #include <windows.h>
  static bool _ansiOk = false;
  static void _enableAnsi() {
      if (_ansiOk) return;
      HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
      DWORD m = 0; GetConsoleMode(h, &m);
      SetConsoleMode(h, m | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
      _ansiOk = true;
  }
#else
  static void _enableAnsi() {}
#endif


namespace A {
    const std::string R  = "\033[0m";
    const std::string B  = "\033[1m";
    const std::string D  = "\033[2m";
    const std::string I  = "\033[3m";
    const std::string BL = "\033[5m";
    const std::string W  = "\033[97m";
    const std::string GR = "\033[90m";
    const std::string G  = "\033[92m";
    const std::string Y  = "\033[93m";
    const std::string RE = "\033[91m";
    const std::string CY = "\033[96m";
    const std::string MA = "\033[95m";
    const std::string BU = "\033[94m";
}


int DisplayManager::visLen(const std::string& s) {
    int n = 0; bool esc = false;
    for (unsigned char c : s) {
        if (c == '\033') { esc = true; continue; }
        if (esc) { if (c == 'm') esc = false; continue; }
        
        if ((c & 0xC0) == 0x80) continue; 
        
        n++;
    }
    return n;
}

std::string DisplayManager::repeat(char c, int n) {
    return n > 0 ? std::string(n, c) : "";
}

std::string DisplayManager::border(char l, char m, char r, int w) {
    return std::string(1,l) + repeat(m, w) + std::string(1,r) + "\n";
}

std::string DisplayManager::line(const std::string& content, int w) {
    int pad = w - 2 - visLen(content);
    if (pad < 0) pad = 0;
    return "| " + content + repeat(' ', pad) + " |\n";
}

std::string DisplayManager::centerText(const std::string& s, int w) {
    int vl = visLen(s);
    int total = w - 2;
    int left  = (total - vl) / 2;
    int right = total - vl - left;
    if (left < 0) left = 0;
    if (right < 0) right = 0;
    return "| " + repeat(' ', left) + s + repeat(' ', right) + " |\n";
}

std::string DisplayManager::pad(const std::string& s, int width) {
    int p = width - visLen(s);
    return p > 0 ? s + std::string(p, ' ') : s;
}

std::string DisplayManager::bar(int cur, int max, int width, char fill, char empty) {
    if (max <= 0) return "[" + repeat(empty, width) + "]";
    int f = std::max(0, std::min(cur * width / max, width));
    return "[" + repeat(fill, f) + repeat(empty, width - f) + "]";
}

void DisplayManager::clearScreen() {
    _enableAnsi();
    std::cout << "\033[2J\033[H" << std::flush;
}

void DisplayManager::waitEnter(const std::string& msg) {
    std::string m = msg.empty() ? "  Appuyez sur [ENTREE] pour continuer..." : msg;
    std::cout << A::GR << A::D << m << A::R << std::flush;
    std::cin.ignore(1000, '\n');
    std::cin.get();
}


void DisplayManager::renderTitleScreen() {
    clearScreen();
    std::ostringstream buf;

    buf << A::CY << A::B;
    buf << border('+', '=', '+');

    // Logo ASCII simple et standard
    const std::vector<std::string> logo = {
        "    _   _  _____  ___  ___  ___  _   _  _   _  ___    ",
        "   / \\ | ||_   _|| __|| _ \\|   \\| | | || \\ | || __|   ",
        "  / ^ \\| |__| |  | _| |   /| |) | |_| ||  \\| || _|    ",
        " /_/ \\_\\____|_|  |___||_|_\\|___/ \\___/ |_|\\__||___|   "
    };

    buf << A::R;
    buf << line("");
    
    // Affichage du logo centré
    for (auto& l : logo) {
        buf << centerText(A::CY + A::B + l + A::R);
    }
    
    buf << line("");
    buf << A::Y << border('+', '-', '+') << A::R;
    buf << line("");
    buf << A::CY << border('+', '=', '+') << A::R;
    buf << centerText(A::CY + A::B + "[ Appuyez sur ENTREE pour commencer ]" + A::R);

    std::cout << buf.str() << std::flush;
    std::cin.get(); 
}
//  MENU PRINCIPAL

void DisplayManager::renderMainMenu(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    // ── En-tête ──────────────────────────────────────────────────────────
    buf << A::CY << A::B << border('+', '=', '+') << A::R;
    buf << centerText(A::CY + A::B + "A L T E R D U N E" + A::R);
    buf << A::CY << A::B << border('+', '=', '+') << A::R;

    // ── Carte du joueur ───────────────────────────────────────────────────
    int hpPct  = (player.getHpMax() > 0) ? player.getHpCurrent() * 100 / player.getHpMax() : 0;
    std::string hpCol = hpPct > 50 ? A::G : hpPct > 20 ? A::Y : A::RE;

    buf << line(A::B + A::W + " JOUEUR" + A::R + "  " + A::B + player.getName() + A::R);

    std::ostringstream hpRow;
    hpRow << " HP  " << hpCol << A::B
          << std::setw(3) << player.getHpCurrent() << "/" << player.getHpMax() << A::R
          << "  " << hpCol << bar(player.getHpCurrent(), player.getHpMax(), 22, '#', '.') << A::R;
    buf << line(hpRow.str());

    // Barre de progression victoires
    std::ostringstream vicRow;
    int v = player.getVictories();
    vicRow << " PROGRESSION : ";
    for (int i = 0; i < 10; ++i)
        vicRow << (i < v ? A::CY + A::B + "#" + A::R : A::GR + "." + A::R);
    vicRow << "  " << A::B << v << "/10" << A::R;
    buf << line(vicRow.str());

    // Stats rapides
    std::ostringstream sRow;
    sRow << " " << A::RE << "Tués: " << A::B << player.getMonstersKilled() << A::R
         << "    " << A::G << "Épargnés: " << A::B << player.getMonstersSpared() << A::R;
    buf << line(sRow.str());

    buf << A::Y << border('+', '-', '+') << A::R;

    // ── Menu ─────────────────────────────────────────────────────────────
    buf << centerText(A::Y + A::B + "MENU PRINCIPAL" + A::R);
    buf << A::Y << border('+', '-', '+') << A::R;

    const std::vector<std::pair<std::string,std::string>> opts = {
        {"1", "  Partir au combat"},
        {"2", "  Bestiaire"},
        {"3", "  Statistiques"},
        {"4", "  Inventaire / Items"},
        {"5", "  Quitter"},
    };
    for (auto& [k,v2] : opts) {
        buf << line(A::W + " [" + k + "] " + A::R + v2);
    }
    buf << A::CY << border('+', '=', '+') << A::R;
    buf << " Votre choix : ";

    std::cout << buf.str() << std::flush;
}

//  VUE COMBAT
void DisplayManager::renderCombat(const Player& player, const Monster& monster,
                                   const std::vector<std::string>& logs,
                                   const std::string& prompt) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    buf << A::RE << A::B << border('+', '=', '+') << A::R;
    {
        std::string catTag;
        if      (monster.getCategory() == "BOSS")     catTag = A::RE + A::B + "[ BOSS ]" + A::R;
        else if (monster.getCategory() == "MINIBOSS") catTag = A::Y  + A::B + "[ MINI-BOSS ]" + A::R;
        else                                           catTag = A::G  + A::D + "[ NORMAL ]" + A::R;
        
        buf << centerText(A::RE + A::B + "COMBAT vs " + A::R
                          + A::Y + A::B + monster.getName() + A::R
                          + "  " + catTag);
    }
    buf << A::RE << A::B << border('+', '=', '+') << A::R;

    // Barres HP / Mercy
    auto renderBar = [&](const std::string& label, const std::string& nameStr,
                         int cur, int maxv, bool isMercy) {
        int pct = (maxv > 0) ? cur * 100 / maxv : 0;
        std::string col = isMercy
            ? (pct >= 100 ? A::G : pct >= 50 ? A::Y : A::GR)
            : (pct > 50   ? A::G : pct > 20  ? A::Y : A::RE);

        std::ostringstream row;
        row << " " << A::B << pad(label, 8) << A::R
            << pad(nameStr, 14)
            << col << A::B << std::setw(3) << cur << "/" << std::setw(3) << maxv << A::R
            << " " << col << bar(cur, maxv, 16, isMercy ? '*' : '#', '.') << A::R;
        buf << line(row.str());
    };

    renderBar("HP", player.getName(),   player.getHpCurrent(), player.getHpMax(), false);
    renderBar("HP", monster.getName(),  monster.getHpCurrent(), monster.getHpMax(), false);
    renderBar("MERCY", monster.getName(), monster.getMercyCurrent(), monster.getMercyGoal(), true);

    // Journal de combat
    buf << A::Y << border('+', '-', '+') << A::R;
    buf << line(A::GR + A::D + " ▼ JOURNAL DE COMBAT" + A::R);

    int logStart = std::max(0, (int)logs.size() - 5);
    for (int i = logStart; i < (int)logs.size(); ++i) {
        buf << line(A::D + "  › " + A::R + logs[i]);
    }
  
    for (int i = (int)(logs.size() - logStart); i < 5; ++i)
        buf << line("");

    // Actions
    buf << A::Y << border('+', '-', '+') << A::R;
    buf << line(A::W + A::B + " [1] FIGHT   " + A::R
              + A::CY + A::B + "[2] ACT   " + A::R
              + A::G  + A::B + "[3] ITEM   " + A::R
              + A::Y  + A::B + "[4] MERCY" + A::R);
    buf << A::CY << border('+', '=', '+') << A::R;

    if (!prompt.empty()) buf << prompt;
    else buf << " Votre action (1/2/3/4) : ";

    std::cout << buf.str() << std::flush;
}

//  MENU ACT

void DisplayManager::renderActMenu(const Player& player, const Monster& monster,
                                    const std::vector<std::string>& logs,
                                    const std::vector<std::string>& acts,
                                    const std::map<std::string, ActAction>& catalog) {
    renderCombat(player, monster, logs, "");
    std::ostringstream buf;
    buf << "\n";
    buf << A::CY << A::B << border('+', '-', '+') << A::R;
    buf << centerText(A::CY + A::B + "* ACTIONS ACT *" + A::R);
    buf << A::CY << border('+', '-', '+') << A::R;

    for (size_t i = 0; i < acts.size(); ++i) {
        const std::string& id = acts[i];
        std::ostringstream row;
        row << " " << A::W << A::B << "[" << i+1 << "] " << A::R
            << A::CY << pad(id, 16) << A::R;
        if (catalog.count(id)) {
            int imp = catalog.at(id).mercyImpact;
            std::string sign  = (imp >= 0) ? "+" : "";
            std::string col   = (imp >= 0) ? A::G : A::RE;
            
            int absImp = std::abs(imp);
            int dots   = std::min(absImp / 5, 8);
            std::string impBar = col + "[" + std::string(dots, '|') + std::string(8-dots, ' ') + "] " + A::R;
            row << impBar << col << "Mercy " << sign << imp << A::R;
        }
        buf << line(row.str());
    }
    buf << A::CY << border('+', '-', '+') << A::R;
    buf << " Votre choix : ";
    std::cout << buf.str() << std::flush;
}

//  FLASH VICTOIRE
void DisplayManager::renderVictoryFlash(const Monster& monster, bool spared) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    if (spared) {
        buf << A::G << A::B << border('+', '=', '+') << A::R;
        buf << centerText(A::G + A::B + "MONSTRE EPARGNE" + A::R);
        buf << A::G << border('+', '-', '+') << A::R;

        buf << line("");
        buf << centerText(A::GR + "+ Mercy accomplie +" + A::R);
        buf << line("");
    } else {
        buf << A::RE << A::B << border('+', '=', '+') << A::R;
        buf << centerText(A::RE + A::B + "MONSTRE VAINCU" + A::R);
        buf << A::RE << border('+', '-', '+') << A::R;

        buf << line("");
        buf << centerText(A::RE + A::B + monster.getName() + A::R + A::RE + A::I + " est mort" + A::R);
        buf << line("");
    }
    buf << A::Y << A::B << border('+', '=', '+') << A::R;

    std::cout << buf.str();
    waitEnter();
}


//  BESTIAIRE

void DisplayManager::renderBestiary(const std::vector<Monster*>& bestiary) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";
    buf << A::MA << A::B << border('+', '=', '+') << A::R;
    buf << centerText(A::MA + A::B + "BESTIAIRE DES RENCONTRES" + A::R);
    buf << A::MA << border('+', '-', '+') << A::R;

    if (bestiary.empty()) {
        buf << line("");
        buf << centerText(A::GR + A::I + "Aucune rencontre enregistree." + A::R);
        buf << line("");
    } else {
        int idx = 1;
        for (const Monster* m : bestiary) {
            bool sp = m->isSpared();
            std::string resTag = sp ? A::G + A::B + " [EPARGNE] " + A::R
                                    : A::RE + A::B + " [TUE    ] " + A::R;
            std::string catCol = (m->getCategory()=="BOSS") ? A::RE :
                                 (m->getCategory()=="MINIBOSS") ? A::Y : A::G;

            buf << line(A::B + A::W + " #" + std::to_string(idx++) + " " + A::R
                      + A::B + pad(m->getName(), 16) + A::R
                      + catCol + A::D + "[" + m->getCategory() + "]" + A::R
                      + resTag);

            std::ostringstream st;
            st << "     HP:" << A::CY << std::setw(4) << m->getHpMax() << A::R
               << "  ATK:" << A::RE << std::setw(3) << m->getAttack() << A::R
               << "  DEF:" << A::BU << std::setw(3) << m->getDefense() << A::R
               << "  Mercy:" << A::Y << m->getMercyGoal() << A::R;
            buf << line(st.str());
            buf << line(A::GR + A::D + "  " + std::string(W - 6, '.') + A::R);
        }
    }
    buf << A::MA << A::B << border('+', '=', '+') << A::R;
    std::cout << buf.str();
    waitEnter();
}


//  STATISTIQUES

void DisplayManager::renderStats(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    int hpPct = (player.getHpMax() > 0) ? player.getHpCurrent() * 100 / player.getHpMax() : 0;
    std::string hpCol = hpPct > 50 ? A::G : hpPct > 20 ? A::Y : A::RE;

    buf << A::BU << A::B << border('+', '=', '+') << A::R;
    buf << centerText(A::BU + A::B + "FICHE DU JOUEUR" + A::R);
    buf << A::BU << border('+', '-', '+') << A::R;
    buf << line("");

    buf << centerText(A::W + A::B + "<< " + player.getName() + " >>" + A::R);
    buf << line("");
    buf << A::BU << border('+', '-', '+') << A::R;

    std::ostringstream hp;
    hp << "  HP      " << hpCol << A::B << std::setw(3) << player.getHpCurrent()
       << "/" << player.getHpMax() << A::R
       << "   " << hpCol << bar(player.getHpCurrent(), player.getHpMax(), 24, '#', '.') << A::R;
    buf << line(hp.str());

    buf << line("");
    buf << A::BU << border('+', '-', '+') << A::R;

    std::ostringstream vRow;
    vRow << "  Victoires    ";
    int v = player.getVictories();
    for (int i = 0; i < 10; ++i)
        vRow << (i < v ? A::CY + A::B + "#" + A::R : A::GR + "." + A::R);
    vRow << "  " << A::B << v << "/10" << A::R;
    buf << line(vRow.str());

    buf << line("  Monstres tues      " + A::RE  + A::B + std::to_string(player.getMonstersKilled())  + A::R);
    buf << line("  Monstres epargnes  " + A::G   + A::B + std::to_string(player.getMonstersSpared()) + A::R);

    int total = player.getMonstersKilled() + player.getMonstersSpared();
    if (total > 0) {
        buf << line("");
        int pacifPct = player.getMonstersSpared() * 100 / total;
        std::string karmaLabel = pacifPct >= 80 ? A::G + "PACIFISTE" + A::R :
                                  pacifPct >= 40 ? A::Y + "NEUTRE"    + A::R :
                                                   A::RE + "GENOCIDAIRE" + A::R;
        buf << line("  Voie : " + karmaLabel);
        std::ostringstream kBar;
        kBar << "  " << A::RE + "[" + A::R;
        int filled = pacifPct * 20 / 100;
        for (int i = 0; i < 20; ++i) {
            if (i < filled) kBar << A::G + "#" + A::R;
            else             kBar << A::RE + "." + A::R;
        }
        kBar << A::RE + "]" + A::R;
        buf << line(kBar.str());
    }

    buf << line("");
    buf << A::BU << A::B << border('+', '=', '+') << A::R;
    std::cout << buf.str();
    waitEnter();
}


//  INVENTAIRE / ITEMS

void DisplayManager::renderItems(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    buf << A::G << A::B << border('+', '=', '+') << A::R;
    buf << centerText(A::G + A::B + "INVENTAIRE" + A::R);
    buf << A::G << border('+', '-', '+') << A::R;

    const auto& inv = player.getInventory();
    if (inv.empty()) {
        buf << line("");
        buf << centerText(A::GR + A::I + "Votre inventaire est vide..." + A::R);
        buf << line("");
    } else {
        for (size_t i = 0; i < inv.size(); ++i) {
            const auto& it = inv[i];
            std::ostringstream row;
            row << " " << A::W << A::B << "[" << i+1 << "] " << A::R
                << A::G << pad(it.getName(), 16) << A::R
                << A::Y << "x" << it.getQuantity() << A::R
                << A::GR << "  " << pad(it.getType(), 6) << A::R;
            if (it.getType() == "HEAL") {
                row << A::G << " +" << it.getValue() << " HP" << A::R;
                int dots = std::min(it.getValue() / 3, 10);
                row << "  " << A::G + A::D + "[" + std::string(dots, '+') + std::string(10-dots, ' ') + "]" + A::R;
            }
            buf << line(row.str());
        }
    }
    buf << A::G << A::B << border('+', '=', '+') << A::R;
    buf << " Utiliser un item ? (0 pour annuler) : ";
    std::cout << buf.str() << std::flush;
}


//  ÉCRAN FIN – MORT DU JOUEUR 

void DisplayManager::renderDeathScreen(const Player& player, const Monster& killer) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    buf << A::RE << A::B << border('+', '=', '+') << A::R;
    buf << centerText(A::RE + A::B + A::BL + "VOUS ETES MORT" + A::R);
    buf << A::RE << A::B << border('+', '=', '+') << A::R;
    buf << line("");

    buf << centerText(A::W + A::B + player.getName() + A::R
                    + A::GR + " a ete vaincu par " + A::R
                    + A::RE + A::B + killer.getName() + A::R);
    buf << line("");
    buf << line("");
    buf << A::RE << border('+', '-', '+') << A::R;

    buf << line("  Victoires atteintes  : " + A::CY + A::B + std::to_string(player.getVictories()) + "/10" + A::R);
    buf << line("  Monstres tues        : " + A::RE + std::to_string(player.getMonstersKilled()) + A::R);
    buf << line("  Monstres epargnes    : " + A::G  + std::to_string(player.getMonstersSpared()) + A::R);
    buf << line("");
    buf << A::RE << A::B << border('+', '=', '+') << A::R;

    std::cout << buf.str() << std::flush;
    waitEnter("  Appuyez sur [ENTREE] pour quitter...");
}

//  ÉCRAN FIN

void DisplayManager::renderEndScreen(const Player& player) {
    std::ostringstream buf;
    buf << "\033[2J\033[H";

    int killed  = player.getMonstersKilled();
    int spared  = player.getMonstersSpared();
    bool isPaci = (killed == 0);
    bool isGeno = (spared == 0);

    std::string mainCol  = isPaci ? A::G : isGeno ? A::RE : A::Y;
    std::string finTitle = isPaci ? "FIN PACIFISTE"
                         : isGeno ? "FIN GENOCIDAIRE"
                         :          "FIN NEUTRE";

    buf << mainCol << A::B << border('+', '=', '+') << A::R;
    buf << centerText(mainCol + A::B + finTitle + A::R);
    buf << mainCol << A::B << border('+', '=', '+') << A::R;
    buf << line("");

    buf << centerText(A::W + A::B + "RESULTAT FINAL" + A::R);
    buf << line("");
    buf << line("  Victoires          " + A::CY + A::B + "10/10" + A::R + "  " + A::CY + "V" + A::R);

    int total = killed + spared;
    if (total > 0) {
        std::ostringstream kRow;
        kRow << "  Tues    " << A::RE << A::B << std::setw(3) << killed << A::R
             << "  " << A::RE << bar(killed, total, 20, '#', '.') << A::R;
        buf << line(kRow.str());

        std::ostringstream sRow;
        sRow << "  Epargnes " << A::G << A::B << std::setw(3) << spared << A::R
             << "  " << A::G << bar(spared, total, 20, '#', '.') << A::R;
        buf << line(sRow.str());
    }

    buf << line("");
    buf << mainCol << border('+', '-', '+') << A::R;

    if (isPaci) {
        buf << centerText(A::G + A::I + "\"L'integralite des monstres rencontres" + A::R);
        buf << centerText(A::G + A::I + "a ete epargnee lors de cette session.\"" + A::R);
    } else if (isGeno) {
        buf << centerText(A::RE + A::I + "\"L'integralite des monstres rencontres" + A::R);
        buf << centerText(A::RE + A::I + "a ete vaincue lors de cette session.\"" + A::R);
    } else {
        buf << centerText(A::Y + A::I + "\"Tu as tue et epargne. Un chemin" + A::R);
        buf << centerText(A::Y + A::I + "ambigu. Alterdune se souvient.\"" + A::R);
    }
    buf << line("");
    buf << mainCol << A::B << border('+', '=', '+') << A::R;

    std::cout << buf.str() << std::flush;
    waitEnter("  Appuyez sur [ENTREE] pour quitter...");
}
