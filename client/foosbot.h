#ifndef __foosbot_h__
#define __foosbot_h__

extern volatile int force_exit;
extern volatile unsigned int scores[2];

typedef enum {
  TEAM_A,
  TEAM_B
} teams;

void trigger_win();
void trigger_reset();
void trigger_update();

#endif
