#ifndef __wiring_pi_h__
#define __wiring_pi_h__

typedef enum {
  INPUT,
  OUTPUT
} pin_mode;

typedef enum {
  HIGH,
  LOW
} pin_state;

int wiringPiSetup();
void pinMode(int pin, pin_mode mode);
int digitalRead(int pin);
void digitalWrite(int pin, int state);

#endif
