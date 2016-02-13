#include <wiringPi.h>

int pin = 1;

// Frequency in Hertz
// Length in milliseconds
void beep (int hertz, int length) {
  // We sleep between changes in microseconds (roughly), so convert the given
  // frequency into a microsecond wait time (we divide it in half since one
  // cycle involves both a high and a low
  int wait = 1000000 / hertz / 2;
  int until = length * hertz / 1000;

  int counter;
  for (counter = 0; counter < until; counter ++) {
    digitalWrite (pin, HIGH); delayMicroseconds (wait);
    digitalWrite (pin,  LOW); delayMicroseconds (wait);
  }
}

int main (void) {
  // Setup
  wiringPiSetup ();

  // Use pin 1 (marked GPIO pin 12)
  // We want 5V for the speaker
  pinMode (pin, OUTPUT);

  // Two beeps with different tones over the course of a second
  beep (1500, 300);
  beep (1000, 200);

  // We'll never reach here, but to be polite:
  return 0;
}
