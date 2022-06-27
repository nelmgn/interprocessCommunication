// Message Queues

// standard includes
#include <stdio.h>
#include <stdlib.h>

// includes für Message Queues
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// include für fork
#include <unistd.h>
#include <string.h>

// Main Methode  ---------------------------------------------ElternProzess--------------------------------------------------------
int main(int argc, char **argv)
{

    // Attribute der Message Queue
    int messageKey = 1402; // MessageSchlüssel
    int irrelevant;
    int codeMessageGet;
    int codeMessageCTL; 
    int codeMessageReceive;

    // Attribute der Prozesse
    int anzahlZahlen;
    int i;
    int array[10];

    // Definierte MessageBuffer Struktur
    typedef struct messageBuffer
    {
        long messageType;     // Nachrichtentyp,  Prozesse werden dem Typ zugewiesen um diese im Buffer zu definineren
        char messageSize[80]; // Größe definiert
    } message;

    // Attribute des Buffers
    message sendBuffer;    // Empfangspuffer
    message receiveBuffer; // Sendepuffer

    // Message Queue wird im Elternprozess erstellt
    codeMessageGet = msgget(messageKey, IPC_CREAT | 0600); // IPC CREAT Neuerzeugung der Message Queue/ Warteschlange
    if (codeMessageGet < 0)                                
    {
        printf("Wartrschlange wurde nicht erstellt!\n");
        perror("messageGet");
        exit(1);
    }
    else
    {
        printf("Warteschlange %i inklusive ID %i ist bereit\n", messageKey, codeMessageGet);
    }

    // Prozesse werden erzeugt
    printf("Conv: PID: %i\n", getpid());

    // Anzahl von Zufallszahlen
    // Zufallszahlen erzeugen
    srand(time(NULL));
    // Es können maximal 10 Zufallszahlen erzeugt werden
    anzahlZahlen = rand() % 10 + 1;
    printf("%i ", anzahlZahlen);
    printf("Zufallszahlen: ");
    for (i = 0; i < anzahlZahlen; i++)
    {
        // Die Zufallszahlen sind zwischen 0 und 10
        array[i] = rand() % 11;
        printf("%d ", array[i]);
    }
    printf("\n");

    sendBuffer.messageType = 1;                                                                          // Nachrichtentyp festlegen   CONV sendet an STAT und Log
    strncpy(sendBuffer.messageSize, " Conv: PID: %i  array %d and  anzahlZahlen %d", getpid() && array[i] && anzahlZahlen); // Nachricht in Sendepuffer schreiben
    // Sender
    if (msgsnd(codeMessageGet, &sendBuffer, sizeof(sendBuffer.messageSize), IPC_NOWAIT) == -1) 
    {
        printf("Senden der Nachricht fehlgeschlagen!\n");
        perror("messageSend"); 
        exit(1);               
    }
    else
    {
        printf("Geschriebene Nachricht: %s\n", sendBuffer.messageSize);
    }

    // Im Elternprozess soll es keinen Empfänger geben! CONV empfängt nichts, daher kein Receive

    //-----------------------------------------------------Prozess--------------------------------------------------------------------------------------
    int id1 = fork();
    if (id1 == 0)
    {

        printf("Stat: PID: %i\n", getpid());
        int summe = 0;
        float mittelwert;

        // Summe und Mittelwert berechnen
        for (i = 0; i < anzahlZahlen; i++)
        {
            summe += array[i];
        }
        mittelwert = (float)summe / anzahlZahlen;
        // printf("\n mittelwert: %.2f \n", mittelwert);        ???

        sendBuffer.messageType = 2;                                                                               // Nachrichtentyp festlegen Senden, Stat sendet an Report
        receiveBuffer.messageType = 1;                                                                            // Nachrichtentyp festlegen Empfangen, Stat empfängt von CONV
        strncpy(sendBuffer.messageSize, "Stat: PID: %i mittelwert %i summe %d", getpid() && mittelwert && summe); // Nachricht in Sendepuffer schreiben
        // Sender
        if (msgsnd(codeMessageGet, &sendBuffer, sizeof(sendBuffer.messageSize), IPC_NOWAIT) == -1) 
        {
            printf("Senden der Nachricht fehlgeschlagen!\n");
            perror("messageSend");
            exit(1);
        }
        else
        {
            printf("Geschriebene Nachricht: %s\n", sendBuffer.messageSize);
        }

        // Empfänger
        codeMessageReceive = msgrcv(codeMessageGet, &receiveBuffer, sizeof(receiveBuffer.messageSize), receiveBuffer.messageType, MSG_NOERROR | IPC_NOWAIT); 

        if (codeMessageReceive < 0)
        {
            printf("Lesen der Message fehlgeschlagen!\n");
            perror("messageReceive");
            exit(1);
        }
        else
        {
            printf("Empfangene Nachricht: %s\n", receiveBuffer.messageSize); // Empfangener Text wird ausgegeben
            printf("Laenge der Nachricht: %i Zeichen \n", codeMessageReceive); //Laenge
        }

        //----------------------------------------------------------Prozess-----------------------------------------------------------------------------------

        int id3 = fork();
        if (id3 == 0)
        {

            printf("Report: PID: %i\n", getpid());

            printf("Summe: %d\n", summe);
            printf("Mittelwert: %.2f\n ", mittelwert);

            receiveBuffer.messageType = 2; // Nachrichtentyp festlegen, Report empfängt von Stat
            // Report soll nur Empfangen, daher kein Sender
            // Empfänger
            codeMessageReceive = msgrcv(codeMessageGet, &receiveBuffer, sizeof(receiveBuffer.messageSize), receiveBuffer.messageType, MSG_NOERROR | IPC_NOWAIT); 

            if (codeMessageReceive < 0)
            {
                printf("Lesen der Message fehlgeschlagen!\n");
                perror("messageReceive");
                exit(1);
            }
            else
            {
                printf("Empfangene Nachricht: %s\n", receiveBuffer.messageSize); // Text wird ausgegeben der Empfangen wurde
                printf("Laenge der Nachricht: %i Zeichen \n", codeMessageReceive);
            }
        }
    }
    else //------------------------------------------------Prozess-----------------------------------------------------------------------
    {
        int id2 = fork();
        if (id2 == 0)
        {

            printf("Log: PID: %i\n", getpid());
            FILE *file = fopen("convDat.txt", "w");
            if (file == NULL)
            {
                printf("Fehler beim erstellen einer Datei!\n");
                return 18;
            }
            for (i = 0; i < anzahlZahlen; i++)
            {
                fprintf(file, "%d\t", array[i]);
            }

            fclose(file);
        }
    }
    printf("\n");

    receiveBuffer.messageType = 1; // Nachrichtentyp festlegen
    // Log soll nur Empfangen, daher kein Sender
    // Empfänger
    codeMessageReceive = msgrcv(codeMessageGet, &receiveBuffer, sizeof(receiveBuffer.messageSize), receiveBuffer.messageType, MSG_NOERROR | IPC_NOWAIT); 

    if (codeMessageReceive < 0)
    {
        printf("Lesen der Message fehlgeschlagen!\n");
        perror("messageReceive");
        exit(1);
    }
    else
    {
        printf("Empfangene Nachricht: %s\n", receiveBuffer.messageSize); // Text wird ausgegeben der Empfangen wurde
        printf("Laenge der Nachricht: %i Zeichen \n", codeMessageReceive);
    }

    // --------------------------------------------------------------MSQ--------------------------------------------------------------

    codeMessageCTL = msgctl(codeMessageGet, IPC_RMID, 0);  //Warteschlange wird geloescht
    if (codeMessageCTL < 0)
    {
        printf("Warteschlange wurde nicht geloescht!");
        perror("messageCTL");
        exit(1);
    }
    else
    {
        printf("%i mit ID %i wurde geloescht \n", messageKey, codeMessageGet);
    }

    return 0; // exit(0);
}