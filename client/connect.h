#ifndef __connect_h__
#define __connect_h__

#include <libwebsockets.h>

extern volatile int force_exit;

int connect_to_server();

#endif
