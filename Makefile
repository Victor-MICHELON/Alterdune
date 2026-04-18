# --- Variables ---
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Trouve tous les fichiers .cpp dans le dossier src/
SRC = $(wildcard $(SRC_DIR)/*.cpp)
# Déduit le nom des fichiers .o (objets) à partir des .cpp
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# Nom de l'exécutable final
EXEC = $(BIN_DIR)/alterdune

# --- Règles ---

# Règle par défaut : crée les dossiers et compile l'exécutable
all: directories $(EXEC)

# Crée les dossiers obj et bin s'ils n'existent pas (-p évite les erreurs s'ils existent déjà)
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Liaison des fichiers objets pour créer l'exécutable
$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@

# Compilation de chaque fichier .cpp en fichier .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Règle pour compiler ET lancer le programme d'un coup
run: all
	./$(EXEC)

# Nettoyage des fichiers compilés et des dossiers temporaires
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all directories clean run