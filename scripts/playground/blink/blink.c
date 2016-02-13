#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>

int main (void)
{
  // Setup
  wiringPiSetup () ;

  // Use pin 0 (marked GPIO pin 17)
  pinMode (0, OUTPUT) ;

  // Blink indefinitely, once per second:
  for (;;)
  {
    digitalWrite (0, HIGH) ; usleep (500000) ;
    digitalWrite (0,  LOW) ; usleep (500000) ;
  }

  // We'll never reach here, but to be polite:
  return 0 ;
}
