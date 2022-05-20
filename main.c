#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

  int main(){

    int id1 = fork();
    int id2 = fork();

    if (id1 == 0) {
      if(id2 == 0) {
        printf("We are process REPORT\n");
      } else {
        printf("We are process STAT\n");
        // int a, b, sum;
        // int sum = a + b;
        // float mittelwert = sum/2;
      }
    } else {
      if ( id2 == 0){
        printf("We are process LOG\n");
        // FILE *out_file = fopen("log", "w");
        // fprintf("log", "this is a test %d\n", r);  
      } else {
        printf("We are process CONV");
    // srand(time(NULL));
    // int lowerLimit = 0, upperLimit = 100;
    // int r =  lowerLimit + rand() % (upperLimit - lowerLimit);
    // printf("Random rumber: %d", r);
      }
    }
    printf("\n");
    
    return 0;
  }