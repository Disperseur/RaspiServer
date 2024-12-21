/*
Serveur a executer sur la raspi
*/

#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <ctime>       // Pour l'horodatage
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Pour inet_ntoa
#include <unistd.h>

#define PORT 8080

int fonction(int x) {
    return x * x;
}

// Fonction pour obtenir l'heure actuelle formatée
std::string getTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S]", std::localtime(&now));
    return std::string(buf);
}

// Fonction pour enregistrer les messages dans le fichier log
void logMessage(const std::string& message) {
    std::ofstream logFile;
    logFile.open("/home/antoine/RaspiServer/server_log.txt", std::ios_base::app); // Ouvre en mode append
    if (logFile.is_open()) {
        logFile << getTimestamp() << " " << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Erreur lors de l'ouverture du fichier de log\n";
    }
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address, client_address;
    int addrlen = sizeof(address);
    socklen_t client_addrlen = sizeof(client_address);
    std::string buffer;

    // Création du socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return -1;
    }

    // Configuration pour réutiliser l'adresse locale
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true) {
        std::cout << "Waiting for a new client...\n";

        // Accepter une nouvelle connexion client
        if ((client_socket = accept(server_fd, (struct sockaddr*)&client_address, &client_addrlen)) < 0) {
            perror("Accept failed");
            close(server_fd);
            return -1;
        }

        // Récupérer l'adresse IP et le port du client
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_address.sin_port);

        std::cout << "Client connected: IP " << client_ip << ", Port " << client_port << "\n";

        // Enregistrer le message d'accueil dans le fichier log
        logMessage("Client connected: IP " + std::string(client_ip) + ", Port " + std::to_string(client_port));

        // Gérer la communication avec le client
        while (true) {
            char tempBuffer[1024] = {0}; // Buffer temporaire pour `read`
            int valread = read(client_socket, tempBuffer, sizeof(tempBuffer) - 1);
            if (valread <= 0) {
                std::cout << "Client disconnected: IP " << client_ip << ", Port " << client_port << "\n";
                logMessage("Client disconnected: IP " + std::string(client_ip) + ", Port " + std::to_string(client_port));
                close(client_socket); // Fermer la connexion avec ce client
                break; // Revenir en attente d'un nouveau client
            }

            // Copier les données reçues dans le std::string
            buffer.assign(tempBuffer, valread);

            std::cout << "Message received from " << client_ip << ":" << client_port << ": " << buffer << "\n";

            // Enregistrer le message reçu dans le fichier log
            logMessage("Message received from " + std::string(client_ip) + ":" + std::to_string(client_port) + ": " + buffer);

            try {
                int result = fonction(std::stoi(buffer));
                buffer = std::to_string(result);
            } catch (std::invalid_argument const& ex) {
                buffer = "Erreur de calcul. Paramètre invalide.";
            }

            // Renvoie le message au client
            send(client_socket, buffer.c_str(), buffer.size(), 0);

            // Enregistrer la réponse envoyée dans le fichier log
            logMessage("Message sent to " + std::string(client_ip) + ":" + std::to_string(client_port) + ": " + buffer);
        }
    }

    close(server_fd);
    return 0;
}
