// title: Portfolioprüfung – Werkstück A – Alternative 1
// description:  This c program is an example about inter-process communication via a pipe for Linux. I created 4 processes by forking 3 times.
//               The same program you can see in main.c too, but with two fork() calls.
// author:       Nelli Margaryan    
// date:         June 9th 2022            

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
    return 2;
  }

  int thirdpipe[2];
  if (pipe(thirdpipe) == -1)
  {
    printf("Fehler: Das Anlegen der Pipe(3) ist fehlgeschlagen!\n");
    return 3;
  }

  // Erzeugung von Prozesse
  printf("Conv: Dieser Prozess erzeugt Zufallszahlen.\n");
  printf("Conv: PID: %i\n", getpid());

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

  int id1 = fork();
  // Es kam beim fork zu einem Fehler
  if (id1 < 0)
  {
    printf("Es kam bei fork(1) zu einem Fehler!\n");
    // Programmabbruch
    return 4;
  }

  if (write(firstpipe[1], &anzahlZahlen, sizeof(int)) < 0)
  {
    printf("Fehler beim schreiben auf FirstPipe\n");
    return 5;
  }

  if (write(firstpipe[1], array, sizeof(int) * anzahlZahlen) < 0)
  {
    printf("Fehler beim schreiben auf FirstPipe\n");
    return 6;
  }

  if (write(secondpipe[1], &anzahlZahlen, sizeof(int)) < 0)
  {
    printf("Fehler beim Schreiben auf SecondPipe\n");
    return 7;
  }

  if (write(secondpipe[1], array, sizeof(int) * anzahlZahlen) < 0)
  {
    printf("Fehler beim Schreiben auf SecondPipe");
    return 8;
  }

  close(firstpipe[1]);
  close(secondpipe[1]);

  if (id1 == 0)
  {
    printf("Stat:  Dieser Prozess hat die Messwerte von Conv ausgelesen und statistische Daten (Mittelwert und Summe) berechnet.\n");
    printf("Stat: PID: %i\n", getpid());
    close(firstpipe[1]);

    int array[10];
    int anzahlZahlen;
    int i;
    int summe = 0;
    float mittelwert;

    if (read(firstpipe[0], &anzahlZahlen, sizeof(int)) < 0)
    {
      printf("Fehler beim Lesen von FirstPipe");
      return 9;
    }

    if (read(firstpipe[0], array, sizeof(int) * anzahlZahlen) < 0)
    {
      printf("Fehler beim Lesen von FisrtPipe");
      return 10;
    }

    close(firstpipe[0]);

    // Summe und Mittelwert berechnen
    for (i = 0; i < anzahlZahlen; i++)
    {
      summe += array[i];
    }
    mittelwert = (float)summe / anzahlZahlen;

    if (write(thirdpipe[1], &summe, sizeof(int)) < 0)
    {
      printf("Fehler beim schreiben auf ThirdPipe");
      return 11;
    }

    if (write(thirdpipe[1], &mittelwert, sizeof(float)) < 0)
    {
      printf("Fehler beim schreiben auf ThirdPipe");
      return 12;
    }

    close(thirdpipe[1]);

    int id3 = fork();
    // Es kam beim fork zu einem Fehler
    if (id3 < 0)
    {
      printf("Es kam bei fork(3) zu einem Fehler!\n");
      // Programmabbruch
      return 13;
    }

    if (id3 == 0)
    {
      printf("Report: Dieser Prozess hat auf die Ergebnisse von Stat zugegriffen und die statistischen Daten in der Shell ausgegeben.\n");
      printf("Report: PID: %i\n", getpid());
      close(thirdpipe[1]);

      int summe;
      float mittelwert;

      if (read(thirdpipe[0], &summe, sizeof(summe)) < 0)
      {
        printf("Fehler beim Lesen von ThirdPipe");
        return 14;
      }

      if (read(thirdpipe[0], &mittelwert, sizeof(float)) < 0)
      {
        printf("Fehler beim Lesen von ThirdPipe");
        return 15;
      }

      close(thirdpipe[0]);

      printf("Summe: %d\n ", summe);
      printf("Mittelwert: %.2f\n ", mittelwert);
    }
  }
  else
  {
    int id2 = fork();
    // Es kam beim fork zu einem Fehler
    if (id2 < 0)
    {
      printf("Es kam bei fork(2) zu einem Fehler!\n");
      // Programmabbruch
      return 10;
    }
    if (id2 == 0)
    {
      printf("Log: Dieser Prozess hat die Messwerte von Conv ausgelesen und in convDat.txt geschrieben.\n");
      printf("Log: PID: %i\n", getpid());
      close(secondpipe[1]);

      int array[10];
      int anzahlZahlen;
      int i;

      if (read(secondpipe[0], &anzahlZahlen, sizeof(int)) < 0)
      {
        printf("Fehler beim Lesen von SecondPipe");
        return 16;
      }

      if (read(secondpipe[0], array, sizeof(int) * anzahlZahlen) < 0)
      {
        printf("Fehler beim Lesen von SecondPipe");
        return 17;
      }

      close(secondpipe[0]);

      FILE *file = fopen("convDat.txt", "w");
      if (file == NULL)
      {
        printf("Fehler beim erstellen eines Dateis!\n");
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
  return 0;
}