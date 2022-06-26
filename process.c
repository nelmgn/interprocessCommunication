#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Erzeugung von Prozessen
// Nelli Margaryan

int main()
{
    printf("Conv: PID: %i\n", getpid());
    int id1 = fork();
    if (id1 == 0)
    {
        printf("Stat: PID: %i\n", getpid());
        int id3 = fork();
        if (id3 == 0)
        {
            printf("Report: PID: %i\n", getpid());
        }
    }
    else
    {
        int id2 = fork();
        if (id2 == 0)
        {
            printf("Log: PID: %i\n", getpid());
        }
    }
    printf("\n");
    return 0;
}