
# Variables
CXX = g++
CXXFLAGS = -std=c++17
SRC_DIR = src
BIN_DIR = bin

# Fichiers sources et exécutables
SERVER_SRC = $(SRC_DIR)/serveur.cpp
CLIENT_SRC = $(SRC_DIR)/client.cpp
SERVER_EXEC = $(BIN_DIR)/serveur
CLIENT_EXEC = $(BIN_DIR)/client

# Cibles principales
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Compilation du serveur
$(SERVER_EXEC): $(SERVER_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Compilation du client
$(CLIENT_EXEC): $(CLIENT_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Crée le répertoire bin s'il n'existe pas
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Nettoyage des fichiers générés
clean:
	rm -rf $(BIN_DIR)

# Indique que certaines cibles ne sont pas des fichiers
.PHONY: all clean
