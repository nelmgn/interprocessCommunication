// Titel:         Portfolioprüfung – Werkstück A – Alternative 1
// Beschreibung:  Dieses C-Programm ist ein Beispiel für die Kommunikation zwischen Prozessen über eine Pipe.
//                Ich habe 4 Prozesse erstellt, indem ich 2 mal das Systemaufruf fork() benutzt habe.
//                Dasselbe Programm können Sie auch in prog.c sehen, aber mit drei Aufrufen von fork(). Bewerten Sie bitte nur prog.c!
// Autor:         Nelli Margaryan
// Datum:         09.06.2022

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

int main(int argc, char *argv[])
{
  // Das Anlegen der Pipe
  int firstpipe[2];
  if (pipe(firstpipe) == -1)
  {
    printf("Fehler: Das Anlegen der Pipe(1) ist fehlgeschlagen!\n");
    return 1;
  }

  int secondpipe[2];
  if (pipe(secondpipe) == -1)
  {
    printf("Fehler: Das Anlegen der Pipe(2) ist fehlgeschlagen!\n");
    return 7;
  }

  int thirdpipe[2];
  if (pipe(thirdpipe) == -1)
  {
    printf("Fehler: Das Anlegen der Pipe(3) ist fehlgeschlagen!\n");
    return 13;
  }

  // Erzeugung von Kindprozesse
  int id1 = fork();
  int id2 = fork();
  if (id1 < 0 || id2 < 0)
  {
    printf("Fehler bei fork!\n");
    return 2;
  }

  if (id1 == 0)
  {
    if (id2 == 0)
    {
      printf("Report: Dieser Prozess hat auf die Ergebnisse von Stat zugegriffen und die statistischen Daten in der Shell ausgegeben.\n");
      close(thirdpipe[1]);

      int summe;
      float mittelwert;

      if (read(thirdpipe[0], &summe, sizeof(summe)) < 0)
      {
        printf("Fehler beim Lesen von ThirdPipe\n");
        return 14;
      }

      if (read(thirdpipe[0], &mittelwert, sizeof(float)) < 0)
      {
        printf("Fehler beim Lesen von ThirdPipe\n");
        return 15;
      }

      close(thirdpipe[0]);

      printf("Summe: %d\n ", summe);
      printf("Mittelwert: %.2f\n ", mittelwert);
    }
    else
    {
      printf("Stat:  Dieser Prozess hat die Messwerte von Conv ausgelesen und statistische Daten (Mittelwert und Summe) berechnet.\n");
      close(firstpipe[1]);
      close(thirdpipe[0]);

      int array[10];
      int anzahlZahlen;
      int i;
      int summe = 0;
      float mittelwert;

      if (read(firstpipe[0], &anzahlZahlen, sizeof(int)) < 0)
      {
        printf("Fehler beim Lesen von FirstPipe\n");
        return 5;
      }

      if (read(firstpipe[0], array, sizeof(int) * anzahlZahlen) < 0)
      {
        printf("Fehler beim Lesen von FisrtPipe\n");
        return 6;
      }

      close(firstpipe[0]);

      for (i = 0; i < anzahlZahlen; i++)
      {
        summe += array[i];
      }
      mittelwert = (float)summe / anzahlZahlen;

      if (write(thirdpipe[1], &summe, sizeof(int)) < 0)
      {
        printf("Fehler beim schreiben auf ThirdPipe\n");
        return 16;
      }

      if (write(thirdpipe[1], &mittelwert, sizeof(float)) < 0)
      {
        printf("Fehler beim schreiben auf ThirdPipe\n");
        return 17;
      }

      close(thirdpipe[1]);
    }
  }
  else
  {
    if (id2 == 0)
    {
      printf("Log: Dieser Prozess hat die Messwerte von Conv ausgelesen und in convDaten.txt geschrieben.\n");
      close(secondpipe[1]);

      int array[10];
      int anzahlZahlen;
      int i;

      if (read(secondpipe[0], &anzahlZahlen, sizeof(int)) < 0)
      {
        printf("Fehler beim Lesen von SecondPipe\n");
        return 10;
      }

      if (read(secondpipe[0], array, sizeof(int) * anzahlZahlen) < 0)
      {
        printf("Fehler beim Lesen von SecondPipe\n");
        return 11;
      }

      close(secondpipe[0]);

      FILE *file = fopen("convDaten.txt", "w");
      if (file == NULL)
      {
        printf("Fehler beim erstellen eines Dateis!\n");
        return 12;
      }
      for (i = 0; i < anzahlZahlen; i++)
      {
        fprintf(file, "%d\t", array[i]);
      }

      fclose(file);
    }
    else
    {
      printf("Conv: Dieser Prozess erzeugt Zufallszahlen.\n");
      close(firstpipe[0]);
      close(secondpipe[0]);

      // Anzahl von Zufallszahlen
      int anzahlZahlen;
      int i;
      int array[10];

      // Zufallszahlen erzeugen
      srand(time(NULL));
      // Es können maximal 10 Zufallszahlen erzeugt werden
      anzahlZahlen = rand() % 10 + 1;

      printf("Zufallszahlen: ");
      for (i = 0; i < anzahlZahlen; i++)
      {
        // Die Zufallszahlen sind zwischen 0 und 10
        array[i] = rand() % 11;
        printf("%d ", array[i]);
      }
      printf("\n");

      if (write(firstpipe[1], &anzahlZahlen, sizeof(int)) < 0)
      {
        printf("Fehler beim schreiben auf FirstPipe\n");
        return 3;
      }

      if (write(firstpipe[1], array, sizeof(int) * anzahlZahlen) < 0)
      {
        printf("Fehler beim schreiben auf FirstPipe\n");
        return 4;
      }

      if (write(secondpipe[1], &anzahlZahlen, sizeof(int)) < 0)
      {
        printf("Fehler beim Schreiben auf SecondPipe\n");
        return 8;
      }

      if (write(secondpipe[1], array, sizeof(int) * anzahlZahlen) < 0)
      {
        printf("Fehler beim Schreiben auf SecondPipe\n");
        return 9;
      }

      close(firstpipe[1]);
      close(secondpipe[1]);
    }
  }
  printf("\n");
  return 0;
}