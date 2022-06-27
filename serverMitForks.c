// Portfolioprüfung – Werkstück A – Alternative 1
// Interprozesskommunikation mit TCP Sockets
// Variante mit Endlosprozessen
// Theresa Alten

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>

// Festgelegter Port
#define PORT 50000

// Methode für Fehlermeldungen
void error(const char *msg)
{

	perror(msg);
	exit(1);
}

// SigInt für den Programmabbruch durch Tastenkombination Ctrl+C
int received_sigint;

// Methode für den Programmabbruch
void handle_sigint_in_main()
{
	printf("Interrupt handler in main\n");
	received_sigint = 1;
}

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, valread;
	int clientaddrlength;
	struct sockaddr_in adresse;
	received_sigint = 0;
	int pid;

	// Signal Handler
	struct sigaction act;
	act.sa_handler = &handle_sigint_in_main;
	act.sa_flags = SA_RESTART;
	sigfillset(&act.sa_mask);

	if (sigaction(SIGINT, &act, NULL) == -1)
	{
		perror("Fehler bei sigaction\n");
		exit(1);
	}

	// Speicherbereich von sockaddr_in mit Nullen füllen
	memset(&adresse, 0, sizeof(adresse));
	// Alternative: bzero((char *) &serv_addr, sizeof(serv_addr));

	// Socket Adresse in sockaddr_in speichern
	adresse.sin_family = AF_INET;
	adresse.sin_addr.s_addr = INADDR_ANY;
	adresse.sin_port = htons(PORT);

	// Socket erstellen
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		error("Fehler beim Erstellen des Socket.");
	}
	else
	{
		printf("Socket wurde erstellt.\n");
	}

	// SET SOCK OPT
	int reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("setsockopt(SO_REUSEADDR) fehlgeschlagen");
	}

	if (bind(sockfd, (struct sockaddr *)&adresse, sizeof(adresse)) < 0)
	{
		error("Verbindung zu Port fehlgeschlagen.");
	}
	else
	{
		printf("Socket wurde an den Port gebunden.\n");
	}

	if (listen(sockfd, 5) == 0)
	{
		printf("Warte auf Verbindungen.\n");
	}

	clientaddrlength = sizeof(adresse);

	// Erstellung der Endlosprozesse
	while (1)
	{
		if (received_sigint == 0)
		{

			newsockfd = accept(sockfd, (struct sockaddr *)&adresse, &clientaddrlength);

			if (newsockfd < 0)
			{
				error("Fehler bei accept");
			}
			else
			{
				printf("Verbindung zum Client erfolgreich.\n");
			}

			printf("I am the forked process for sending data to client.");
			pid = fork();
			if (pid == 0)
			{
				printf("From Forked Process\n");
				printf("We are process CONV \n");

				int anzahlZahlen, i;
				int array[10];

				printf("Zufallszahlen : ");
				// Zufallszahlen erzeugen
				srand(time(NULL));
				anzahlZahlen = rand() % 10 + 1; // Maximal 10 Zufallszahlen 

				for (i = 0; i < anzahlZahlen; i++)
				{
					array[i] = rand() % 11; // Zahlen sind zwischen 0 und 10

					printf("% d ", array[i]);
				}

				printf("\n");

				valread = write(newsockfd, array, sizeof(array));
				if (valread < 0)
				{
					error("Schreibzugriff fehlgeschlagen.");
				}

				valread = write(newsockfd, &anzahlZahlen, sizeof(anzahlZahlen));
				if (valread < 0)
				{
					error("Schreibzugriff fehlgeschlagen.");
				}

				close(newsockfd);
			}
			else
			{ // Elternprozess
				wait(0);
			}
		}
		else
		{

			close(newsockfd);
			close(sockfd);
			if (pid == 0)
			{
				kill(getppid(), 9);
			}
			else
			{
				kill(getpid(), 9);
			}
			return 0;
		}
	}

	close(sockfd);
	return 0;
}