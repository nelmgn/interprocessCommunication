#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 50000

// Methode für Fehlermeldungen (Method for error messages)
void error(const char *msg)
{

    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, valread;
    int clientaddrlength;
    struct sockaddr_in adresse;

    // Pufferinhalt mit Nullbytes füllen (Filling the buffer with Zero-Bytes)
    //char buffer[1024] = {0};
    
    // Speicherbereich von sockaddr_in mit Nullen füllen (Filling the storage area of sockaddr_in with zeros)
    memset(&adresse, 0, sizeof(adresse));
    // Alternative: bzero((char *) &serv_addr, sizeof(serv_addr));

    // Socket Adresse in sockaddr_in speichern (Save socket adress in sockaddr_in)
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = INADDR_ANY;
    adresse.sin_port = htons(PORT);

    // Socket erstellen (Create Socket)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening Socket.");
    }
    else
    {
        printf("Socket was created.\n");
    }

    if (bind(sockfd, (struct sockaddr *)&adresse, sizeof(adresse)) < 0)
    {
        error("Binding to Port failed.");
    }
    else
    {
        printf("Socket was binded to the port.\n");
    }

    if (listen(sockfd, 5) == 0)
    {
        printf("Wait for connections.\n");
    }

    clientaddrlength = sizeof(adresse);

    newsockfd = accept(sockfd, (struct sockaddr *)&adresse, &clientaddrlength);
    if (newsockfd < 0)
    {
        error("Error at accept");
    }
    else
    {
        printf("Connection to Client created.\n");
    }


    printf("We are process CONV \n");
    // Nachricht empfangen (receive message)
    
    
        int anzahlZahlen, i; // number of numbers
        int array[10];

        printf("Random numbers : ");
        // Zufallszahlen erzeugen (Create random numbers)
        srand(time(NULL));
        anzahlZahlen = rand() % 10 + 1; // Maximal 10 Zufallszahlen (Maximum of 10 numbers)

        for (i = 0; i < anzahlZahlen; i++)
        {
            array[i] = rand() % 11; // Zahlen sind zwischen 0 und 10 (Numbers should be between 0 and 10)
            //bzero(buffer, 1024);

            // Empfangene Nachricht lokal ausgeben(print received message)
            printf("% d ", array[i]);
        }



    valread = write(newsockfd, array, sizeof(array));
        if (valread < 0)
        {
            error("Writing access failed");
        }

    valread = write(newsockfd, &anzahlZahlen, sizeof(anzahlZahlen));
        if (valread < 0)
        {
            error("Writing access failed");
        }
    


    close(newsockfd);
    close(sockfd);
    return 0;
}