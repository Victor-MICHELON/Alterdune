#include "ActCatalog.h"

const std::map<std::string, ActAction>& getActCatalog() {
    static const std::map<std::string, ActAction> catalog = {
        {"JOKE",         {"JOKE",         "Tu racontes une blague nulle... le monstre grimace.",     +20}},
        {"COMPLIMENT",   {"COMPLIMENT",   "Tu complimentes ses chaussures. Il rougit.",              +30}},
        {"DISCUSS",      {"DISCUSS",      "Vous discutez de la météo. Ambiance neutre.",             +10}},
        {"OBSERVE",      {"OBSERVE",      "Tu l'observes attentivement. Il se sent mal à l'aise.",  +15}},
        {"PET",          {"PET",          "Tu lui tapotas la tête. Il apprécie bizarrement.",        +25}},
        {"OFFER_SNACK",  {"OFFER_SNACK",  "Tu lui offres un sandwich. Il hésite... puis accepte.",  +35}},
        {"REASON",       {"REASON",       "Tu lui expliques la paix. Il écoute poliment.",           +20}},
        {"DANCE",        {"DANCE",        "Tu fais le moonwalk. Il est intrigué.",                   +15}},
        {"INSULT",       {"INSULT",       "Tu insultes sa mère. C'était une erreur.",               -30}},
        {"TAUNT",        {"TAUNT",        "Tu le provoques. Il est furieux.",                        -20}},
    };
    return catalog;
}