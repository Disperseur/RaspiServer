/*
Client a executer sur la raspi

*/

#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    std::string buffer;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Remplacez "127.0.0.1" par l'adresse IP de la Raspberry Pi
    if (inet_pton(AF_INET, "172.20.10.2", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed\n";
        return -1;
    }

    std::cout << "Connected to server.\n";

    while (true) {
        std::string input;
        std::cout << "Enter a message (type 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "Closing connection.\n";
            break;
        }

        // Envoie le message au serveur
        send(sock, input.c_str(), input.size(), 0);

        // Lit la réponse du serveur
        char tempBuffer[1024] = {0}; // Temporaire pour recevoir les données
        int valread = read(sock, tempBuffer, sizeof(tempBuffer) - 1);
        if (valread <= 0) {
            std::cout << "Server disconnected.\n";
            break;
        }

        // Copie les données reçues dans la string
        buffer.assign(tempBuffer, valread);

        std::cout << "Server echoed: " << buffer << "\n";
    }

    close(sock);
    return 0;
}
