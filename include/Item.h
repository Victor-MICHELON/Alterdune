#pragma once
#include <string>

class Item {
private:
    std::string name;
    std::string type;
    int value; 
    int quantity;     

public:
    // Constructeur
    Item(std::string name, std::string type, int value, int quantity);

    // Accesseurs
    std::string getName() const;
    std::string getType() const;
    int getValue() const;
    int getQuantity() const;

    // Modificateurs
    void addQuantity(int amount);
    bool consume();
};