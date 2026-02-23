#include "Item.h"
#include "Item.h"
#include <string>

using namespace std;

Item::Item(string name, string type, int value, int quantity)
    : name(name), type(type), value(value), quantity(quantity) {}

string Item::getName() const { return name; }
string Item::getType() const { return type; }
int Item::getValue() const { return value; }
int Item::getQuantity() const { return quantity; }

void Item::addQuantity(int amount) {
    quantity += amount;
}

bool Item::consume() {
    if (quantity > 0) {
        quantity--;
        return true;
    }
    return false;
}