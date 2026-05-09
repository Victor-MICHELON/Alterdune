#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <string>
#include "Item.h"
#include "Monster.h"

class FileLoader {
public:
    static std::vector<Item> loadItems(const std::string& filename);
    static std::vector<Monster*> loadMonsters(const std::string& filename);
};

#endif