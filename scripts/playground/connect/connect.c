#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <libwebsockets.h>

static int mirror_lifetime;
static struct lws *wsi_dumb;
static volatile int force_exit;
static unsigned int opts;

static int
callback(struct lws *wsi, enum lws_callback_reasons reason,
        void *user, void *in, size_t len)
{
  unsigned char buf[LWS_PRE + 4096];
  unsigned int rands[4];
  int l = 0;
  int n;

  switch (reason) {
  case LWS_CALLBACK_CLIENT_ESTABLISHED:

    lwsl_notice("mirror: LWS_CALLBACK_CLIENT_ESTABLISHED\n");

    lws_get_random(lws_get_context(wsi), rands, sizeof(rands[0]));
    mirror_lifetime = 16384 + (rands[0] & 65535);

    lwsl_info("opened mirror connection with "
        "%d lifetime\n", mirror_lifetime);

    /*
     * mirror_lifetime is decremented each send, when it reaches
     * zero the connection is closed in the send callback.
     * When the close callback comes, wsi_mirror is set to NULL
     * so a new connection will be opened
     *
     * start the ball rolling,
     * LWS_CALLBACK_CLIENT_WRITEABLE will come next service
     */
    lws_callback_on_writable(wsi);
    break;

  case LWS_CALLBACK_CLOSED:
    lwsl_notice("mirror: LWS_CALLBACK_CLOSED mirror_lifetime=%d\n", mirror_lifetime);
    break;

  case LWS_CALLBACK_CLIENT_WRITEABLE:
    for (n = 0; n < 1; n++) {
      lws_get_random(lws_get_context(wsi), rands, sizeof(rands));
      l += sprintf((char *)&buf[LWS_PRE + l],
          "c #%06X %u %u %u;",
          rands[0] & 0xffffff,  /* colour */
          rands[1] & 511,    /* x */
          rands[2] & 255,    /* y */
          (rands[3] & 31) + 1);  /* radius */
    }

    n = lws_write(wsi, &buf[LWS_PRE], l,
            opts | LWS_WRITE_TEXT);
    if (n < 0)
      return -1;
    if (n < l) {
      lwsl_err("Partial write LWS_CALLBACK_CLIENT_WRITEABLE\n");
      return -1;
    }

    mirror_lifetime--;
    if (!mirror_lifetime) {
      lwsl_info("closing mirror session\n");
      return -1;
    }
    /* get notified as soon as we can write again */
    lws_callback_on_writable(wsi);
    break;

  default:
    break;
  }

  return 0;
}

void sighandler(int sig)
{
  force_exit = 1;
}

static struct lws_protocols protocols[] = {
  {
    "fake-nonexistant-protocol",
    callback,
    0,
    128,
  },
  { NULL, NULL, 0, 0 } /* end */
};

static const struct lws_extension exts[] = {
  { NULL, NULL, NULL /* terminator */ }
};

int main (void) {
  struct lws_context_creation_info creation_info;
  struct lws_client_connect_info connection_info;
  struct lws_context *context;

  // Set memory to 0:
  memset(&creation_info, 0, sizeof creation_info);
  memset(&connection_info, 0, sizeof(connection_info));

  // Allow for force stops:
  signal(SIGINT, sighandler);

  // Don't listen on any ports locally:
  creation_info.port = CONTEXT_PORT_NO_LISTEN;
  creation_info.protocols = protocols;
  creation_info.gid = -1;
  creation_info.uid = -1;
  // creation_info.fd_limit_per_thread = 8;

  context = lws_create_context(&creation_info);
  if (context == NULL) {
    fprintf(stderr, "Creating libwebsocket context failed\n");
    return 1;
  }

  // Connection info:
  connection_info.port = 3001;
  connection_info.path = "/";
  connection_info.context = context;
  connection_info.ssl_connection = 0;
  connection_info.address = "172.16.0.203";
  connection_info.host = connection_info.host;
  connection_info.origin = connection_info.host;
  connection_info.ietf_version_or_minus_one = -1;
  connection_info.client_exts = exts;

  fprintf(stdout, "Attempting connection.\n");

  while (!force_exit) {
    connection_info.protocol = protocols[0].name;
    lwsl_notice("using '%s' protocol\n", connection_info.protocol);
    wsi_dumb = lws_client_connect_via_info(&connection_info);

    if (wsi_dumb == NULL) {
      fprintf(stderr, "No connection was found.\n");
      return 2;
    }
    lwsl_notice("Connected successfully\n");

    lws_service(context, 500);
  }

  fprintf(stderr, "Exiting\n");
  lws_context_destroy(context);

  // We'll never reach here, but to be polite:
  return 0;
}
