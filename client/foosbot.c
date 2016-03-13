#include <signal.h>
#include <unistd.h>
#include "dbg.h"
#include "connect.h"
#include "monitor.h"
#include "music.h"

// Change this to reference actual library on the Pi:
#include "wiringPi.h"

volatile int force_exit = 0;
volatile unsigned int scores[2] = { 0 };

typedef enum {
  TEAM_A,
  TEAM_B
} teams;

void sighandler(int sig)
{
  debug("Received exit signal (%d), exiting.", sig);
  force_exit = 1;
}

void trigger_reset() {
  scores[TEAM_A] = 0;
  scores[TEAM_B] = 0;

  send_message("{\"action\":\"reset\"}");
}

void trigger_win() {
  play_music();
  trigger_reset();
}

void trigger_update() {
  beep(1500, 200);
  send_message("{\"action\":\"update\"}");
}

int main(int argc, char **argv)
{
  int ret = 0;
  signal(SIGINT, sighandler);

  wiringPiSetup();
  watch_sensors();

  ret = connect_to_server();
  check(ret >= 0, "Couldn't connect to server");
  trigger_update();

  // Go into a loop and check for a force exit every second or so:
  while(force_exit == 0) {
    sleep(1);
  }

  debug("Exiting");
  return 0;

error:
  return ret;
}
