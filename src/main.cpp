#include <iostream>
#include <string>

int main() {
    std::cout << "=== BIENVENUE DANS ALTERDUNE ===" << std::endl;
    
    // Le projet demande qu'au lancement, le joueur saisisse le nom de son personnage [cite: 25, 27]
    std::string playerName;
    std::cout << "Entrez le nom de votre personnage : ";
    std::cin >> playerName;
    
    std::cout << "Bonjour " << playerName << ", la partie va bientot commencer !" << std::endl;
    
    return 0;
}