#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 50000

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{

  int sockfd;
  int valread;
  struct sockaddr_in adresse;

  //char buffer[1024] = {0};
  // Wenn IP Adresse fehlt (If IP adress is not written in Terminal)
  if (argc < 2)
  {
    error("Ip adresse fehlt\n");
  }

  // Socket erstellen (Create Socket)
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    error("Error opening Socket");
  }
  else
  {
    printf("Socket was created\n");
  }


  
  // Speicher von sockaddr_im mit Nullen füllen (Filling the storage area of sockaddr_in with zeros)
  memset(&adresse, 0, sizeof(adresse));
  

  // Socket adresse in sockaddr_ in speichern (save socket adress in sockaddr_in)
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons(PORT);
  adresse.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (connect(sockfd, (struct sockaddr *) &adresse, sizeof(adresse)) < 0)
  {
    error("connection failed");
  }
  else
  {
    printf("Connection to Server created\n");
  }
  
 
  // anweisungen (instructions)
   
  int anzahlZahlen = 10; //number of numbers
  int i;
  int summe = 0; // sum
  int array[10];
  float mittelwert; //average
  
  valread = read(sockfd, array, sizeof(array));
 if (valread < 0)
 {
  error("error at reading");
  //bzero(buffer, 1024);
 }

 valread = read(sockfd, &anzahlZahlen, sizeof(anzahlZahlen));
 if (valread < 0)
 {
  error("error at reading");
  //bzero(buffer, 1024);
 }

 printf("We are process LOG\n");
      FILE *lokaleDatei;
      lokaleDatei = fopen("log", "w");
      if(lokaleDatei == 0) {
        error("Error at creating File. \n");
      }
      for(i= 0; i<anzahlZahlen; i++) {
        fprintf(lokaleDatei, "%d\t", array[i]);
        //fputs(array[i], lokaleDatei);

      }
      
      fclose(lokaleDatei); 
      printf("File created. \n");
      
      // fprintf("log", "this is a test %d\n", r);
 
 
    for (int i = 0; i < anzahlZahlen; i++)
    {
      summe += array[i];
    }
    mittelwert = (float) summe / anzahlZahlen;

    printf("Used numbers: ");

    for(int i= 0; i<anzahlZahlen; i++) {
       printf("%d ", array[i]);
    }
    printf("\n");
    printf("Number of numbers: %d \n", anzahlZahlen);
    

    printf("We are process REPORT\n");
    printf("Sum: %d \n", summe);
    printf("Average: %f\n", mittelwert);
      
   


  /*int id1 = fork();
  if (id1 == 0)
  {

    

    int id3 = fork();
    if (id3 == 0)
    {
      
    }
  }
  else
  {
    int id2 = fork();
    if (id2 == 0)
    {
     
    }
  } */



/*valread = write(sockfd, summe, sizeof(summe));

if (valread < 0)
{
  error("error at writing");
}*/
printf("We are process STAT\n"); 




//bzero(buffer, 1024); 



  close(sockfd);
  return 0;
}
