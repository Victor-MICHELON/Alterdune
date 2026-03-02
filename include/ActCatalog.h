#pragma once
#include <string>
#include <iostream>
#include <map>

struct ActAction {
    std::string id;
    std::string name;
    int mercyImpact;
};

const std::map<std::string, ActAction>& getActCatalog();
