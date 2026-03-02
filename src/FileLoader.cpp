#include "FileLoader.h"
#include "MonsterDerived.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::vector<Item> FileLoader::loadItems(const std::string& filename) {
    std::vector<Item> items;
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Impossible d'ouvrir " + filename);

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name, type, val, qty;
        if (std::getline(ss, name, ';') && std::getline(ss, type, ';') &&
            std::getline(ss, val, ';') && std::getline(ss, qty, ';')) {
            items.emplace_back(name, type, std::stoi(val), std::stoi(qty));
        }
    }
    return items;
}

#include <algorithm> // Pour std::remove_if

std::vector<Monster*> FileLoader::loadMonsters(const std::string& filename) {
    std::vector<Monster*> monsters;
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Impossible d'ouvrir " + filename);

    std::string line;

    // 1. ON SAUTE LA PREMIÈRE LIGNE (L'en-tête : categorie;nom;...)
    std::getline(file, line);

    while (std::getline(file, line)) {
        // Nettoyage des caractères de fin de ligne Windows (\r)
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cat, name, hp, atk, def, mGoal, a1, a2, a3, a4;

        if (std::getline(ss, cat, ';') && std::getline(ss, name, ';') &&
            std::getline(ss, hp, ';') && std::getline(ss, atk, ';') &&
            std::getline(ss, def, ';') && std::getline(ss, mGoal, ';')) {

            try {
                std::vector<std::string> acts;
                // Lecture des actions restantes
                std::string action;
                while (std::getline(ss, action, ';')) {
                    if (action != "-" && !action.empty()) acts.push_back(action);
                }

                // Création selon la catégorie
                if (cat == "NORMAL")
                    monsters.push_back(new NormalMonster(name, std::stoi(hp), std::stoi(atk), std::stoi(def), std::stoi(mGoal), acts));
                else if (cat == "MINIBOSS")
                    monsters.push_back(new MinibossMonster(name, std::stoi(hp), std::stoi(atk), std::stoi(def), std::stoi(mGoal), acts));
                else if (cat == "BOSS")
                    monsters.push_back(new BossMonster(name, std::stoi(hp), std::stoi(atk), std::stoi(def), std::stoi(mGoal), acts));

            } catch (const std::exception& e) {
                // Si une ligne est mal formée, on l'affiche mais on ne bloque pas tout le jeu
                std::cerr << "Ligne ignoree (erreur format) : " << line << " | Erreur : " << e.what() << std::endl;
            }
        }
    }
    return monsters;
}