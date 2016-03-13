#include <unistd.h>
#include "dbg.h"
#include "foosbot.h"
#include "monitor.h"

// Change this to reference actual library on the Pi:
#include "wiringPi.h"

// Be sure these actually correspond to the correct pins:
unsigned int leds[2][4] = { { 7, 0, 2, 3 }, { 10, 11, 17, 18 } };
unsigned int gates[] = { 8, 9, 10 };

// Wait 2 seconds between gate signals so that a flicker doesn't trigger
// multiple goals (such as when the tilt sensor ball bounces from a hard shot)
const int THROTTLE = 2;

void set_modes() {
  int n;

  for(n = 0; n < 4; n++) {
    pinMode(leds[TEAM_A][n], OUTPUT);
    pinMode(leds[TEAM_B][n], OUTPUT);
  }
  for(n = 0; n < 3; n++) {
    pinMode(gates[n], INPUT);
  }
}

void update_leds(int score, unsigned int *pins) {
  digitalWrite(pins[0], score & 1);
  digitalWrite(pins[1], score & 2);
  digitalWrite(pins[2], score & 4);
  digitalWrite(pins[3], score & 8);
}

void show_scores() {
  update_leds(scores[TEAM_A], leds[TEAM_A]);
  update_leds(scores[TEAM_B], leds[TEAM_B]);
}


void goal(int pin) {
  int score = 0;

  if (pin == gates[TEAM_A]) {
    score = scores[TEAM_B]++;
  }else if(pin == gates[TEAM_B]) {
    score = scores[TEAM_A]++;
  }

  if (score > 8) {
    trigger_win();
  }else{
    trigger_update();
  }
}

void reset(int pin) {
  trigger_reset();
}

typedef void (*gate_callback)(int pin);

void watch_gate(
  int pin,
  gate_callback callback)
{
  int state = digitalRead(pin);
  int newState;

  // Spawn a child process to listen to the gate:
  if (fork() == 0) {

    // Monitor the gate until we're asked to exit:
    while(force_exit == 0) {

      // Wait a little so that the CPU isn't baking itself: (10 milliseconds)
      usleep(10000);
      newState = digitalRead(pin);

      if (state != newState) {
        // Only trigger a callback on signal up:
        if (newState > state) {
          callback(pin);
          // Throttle:
          sleep(THROTTLE);
        }

        state = newState;
      }
    }
  }
}

void watch_sensors() {
  set_modes();
  show_scores();
  watch_gate(gates[0], goal);
  watch_gate(gates[1], goal);
  watch_gate(gates[2], reset);
}
