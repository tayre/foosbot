#include <wiringPi.h>

void show(int score) {
  digitalWrite(7, score & 1);
  digitalWrite(0, score & 2);
  digitalWrite(2, score & 4);
  digitalWrite(3, score & 8);
}

int main (void) {
  // Setup
  wiringPiSetup();

  // GPIO pin numbers don't necessarily correspond to the pin references here:
  pinMode (7, OUTPUT);
  pinMode (0, OUTPUT);
  pinMode (2, OUTPUT);
  pinMode (3, OUTPUT);

  // Increment the score from 0 -> 9 and then stop
  int score;
  for (score = 0; score < 10; score++) {
    show(score);
    delay(1500);
  }
  
  // turn off the LEDs:
  show(0);
  return 0;
}
