/* Project -> Project options -> Parameters -> Linker: -lws2_32 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link della libreria per le socket

#define PORT 53477
#define BUFFER_SIZE 1024

int contaVocali(const char *str) {
    int count = 0;
    char c;
    while ((c = *str++) != '\0') {
        c = tolower(c);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    int clientAddrSize = sizeof(clientAddr);
    struct hostent *clientHost;

    // Inizializzazione Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Errore: WSAStartup fallito.\n");
        return 1;
    }

    // Creazione del socket TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("Errore: socket() fallito.\n");
        WSACleanup();
        return 1;
    }

    // Configurazione dell'indirizzo del server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Binding del socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Errore: bind() fallito.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Messa in ascolto del server
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        printf("Errore: listen() fallito.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    // 1. Il server è avviato sulla porta 53477 e resta sempre in ascolto
    printf("Server TCP in ascolto sulla porta %d...\n", PORT);
	while(true) { 
	    // 4. Il server accetta la connessione del client
	    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
	    if (clientSocket == INVALID_SOCKET) {
	        printf("Errore: accept() fallito.\n");
	        closesocket(serverSocket);
	        WSACleanup();
	        return 1;
	    }
	
	    // Risoluzione dell'indirizzo del client con gethostbyaddr()
	    clientHost = gethostbyaddr((const char *)&clientAddr.sin_addr, sizeof(clientAddr.sin_addr), AF_INET);
	    if (clientHost == NULL) {
	        printf("Errore nella risoluzione dell'indirizzo del client.\n");
	    } 
	
	    // 4. Il server visualizza il messaggio ricevuto sullo standard output
	    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	    if (bytesReceived > 0) {
	        buffer[bytesReceived] = '\0';
	        printf("Messaggio ricevuto dal client: %s\n", buffer);
	    }
	
	    // 5. Il server invia una risposta al client che l'ha contattato con un messaggio del tipo "benvenuto client: <nome-client>"
	    const char *welcomeMessage = "Benvenuto client!";
	    send(clientSocket, welcomeMessage, strlen(welcomeMessage), 0);
	
	    // 8. Il server riceve la stringa e la visualizza sullo std output;
	    bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	    if (bytesReceived > 0) {
	        buffer[bytesReceived] = '\0';
	        printf("Stringa ricevuta dal client: %s\n", buffer);
	    }
	    
	    // 8.1 Il server conta e visualizza il numero di vocali presenti nella stringa ricevuta;
	    int numeroVocali = contaVocali(buffer);
	    printf("Numero di vocali nella stringa: %d\n", numeroVocali);
	
	    // 8.2 Il server invia al client un intero N, pari al numero di vocali trovate;
	    send(clientSocket, (const char *)&numeroVocali, sizeof(int), 0);
	}
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

