#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <string>
#include "Item.h"
#include "Monster.h"

class FileLoader {
public:
    // Méthodes statiques pour être appelées sans instancier la classe [cite: 182]
    static std::vector<Item> loadItems(const std::string& filename);
    static std::vector<Monster*> loadMonsters(const std::string& filename);
};

#endif