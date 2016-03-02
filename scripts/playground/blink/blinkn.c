#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdio.h>

int main(int argc, char **argv)
{ 
  if( argc != 2 )
  {
    printf("Wrong number of arguments\n");
    return 1;
  }

  // Setup
  wiringPiSetup();

  pinMode (0, OUTPUT) ;

  int max = atoi(argv[1]);
  int i;
  // Blink max times, once per 500ms:
  for(i = 0; i < max; ++i) 
  {
    digitalWrite (0, HIGH) ; usleep (125000) ;
    digitalWrite (0,  LOW) ; usleep (125000) ;
  }

  return 0 ;
}
