#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <libwebsockets.h>
#include "dbg.h"
#include "connect.h"

static int was_closed;

int callback_dumb_increment(
    struct libwebsocket_context *this,
    struct libwebsocket *wsi,
    enum libwebsocket_callback_reasons reason,
    void *user,
    void *in,
    size_t len)
{
  switch (reason) {

  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    debug("LWS_CALLBACK_CLIENT_ESTABLISHED");
    break;

  case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
    log_err("LWS_CALLBACK_CLIENT_CONNECTION_ERROR");
    was_closed = 1;
    break;

  case LWS_CALLBACK_CLOSED:
    debug("LWS_CALLBACK_CLOSED");
    was_closed = 1;
    break;

  case LWS_CALLBACK_CLIENT_RECEIVE:
    ((char *)in)[len] = '\0';
    debug("rx %d '%s'\n", (int)len, (char *)in);
    break;

  case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
    check((strcmp(in, "deflate-stream") == 0), "denied deflate-stream extension");
    check((strcmp(in, "deflate-frame") == 0), "denied deflate-frame extension");
    check((strcmp(in, "x-google-mux") == 0), "denied x-google-mux extension");
    break;

  default:
    break;
  }

  return 0;

error:
  return 1;
}

/**
 * list of supported protocols and callbacks
 * https://github.com/warmcat/libwebsockets/blob/v1.4-chrome43-firefox-36/lib/libwebsockets.h#L923
 * - protocol name
 * - callback
 * - memory allocation per session
 * - largest expected received frame size
 **/
struct libwebsocket_protocols protocols[] = {
  {
    "increment",
    callback_dumb_increment,
    0,
    20,
  },
  { NULL, NULL, 0, 0 } /* end */
};

int connect_to_server()
{
  // Set our return value to an error state, and then clear that if we reach
  // the end state cleanly.
  int ret = 1;
  int n;
  struct libwebsocket_context *context;
  struct libwebsocket *ws;
  struct lws_context_creation_info info;
  const char *address = "foosbot.server";

  #ifndef NDEBUG
  lws_set_log_level(
    LLL_ERR |
    LLL_WARN |
    LLL_NOTICE |
    LLL_INFO |
    LLL_DEBUG |
    LLL_PARSER |
    LLL_HEADER |
    LLL_EXT |
    LLL_CLIENT |
    LLL_LATENCY,
    NULL);
  #endif

  /*
   * create the websockets context.  This tracks open connections and
   * knows how to route any traffic and which protocol version to use,
   * and if each connection is client or server side.
   *
   * For this client-only demo, we tell it to not listen on any port.
   */
  memset(&info, 0, sizeof info);
  info.port = CONTEXT_PORT_NO_LISTEN;
  info.protocols = protocols;
  info.gid = -1;
  info.uid = -1;

  context = libwebsocket_create_context(&info);
  check(context != NULL, "Creating libwebsocket context failed");

  /**
   * libwebsocket_client_connect() - Connect to another websocket server
   * @context:  Websocket context
   * @address:  Remote server address, eg, "myserver.com"
   * @port: Port to connect to on the remote server, eg, 80
   * @ssl_connection: 0 = ws://, 1 = wss:// encrypted, 2 = wss:// allow self
   *      signed certs
   * @path: Websocket path on server
   * @host: Hostname on server
   * @origin: Socket origin name
   * @protocol: Comma-separated list of protocols being asked for from
   *    the server, or just one.  The server will pick the one it
   *    likes best.  If you don't want to specify a protocol, which is
   *    legal, use NULL here.
   * @ietf_version_or_minus_one: -1 to ask to connect using the default, latest
   **/
  ws = libwebsocket_client_connect(
      context,
      address,
      80,
      0,
      "/",
      address,
      address,
      protocols[0].name,
      -1);

  check(ws != NULL, "libwebsocket connect failed");
  debug("Waiting for connect...");

  n = 0;
  while (n >= 0 && !was_closed && !force_exit) {
    n = libwebsocket_service(context, 10);
  }

  // Clear our return state to a non-error level:
  check(n >= 0, "Service error (%d)", n);
  ret = 0;

error:
  debug("Exiting");
  libwebsocket_context_destroy(context);

  return ret;
}
