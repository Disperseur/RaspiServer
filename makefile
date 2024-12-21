all: serveur client

client: client.cpp
	g++ client.cpp -o client

serveur: serveur.cpp
	g++ serveur.cpp -o serveur
