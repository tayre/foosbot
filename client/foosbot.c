#include <signal.h>
#include "dbg.h"
#include "foosbot.h"
#include "connect.h"

void sighandler(int sig)
{
  force_exit = 1;
}

int main(int argc, char **argv)
{
  int c;
  signal(SIGINT, sighandler);

  c = connect_to_server();

  return c;
}
