CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Trouve tous les fichiers .cpp dans le dossier src/
SRC = $(wildcard $(SRC_DIR)/*.cpp)
# Déduit le nom des fichiers .o correspondants
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/alterdune

all: directories $(EXEC)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(EXEC): $(OBJ)
	$(CXX) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXEC)

.PHONY: all directories clean