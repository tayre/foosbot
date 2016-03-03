/*
 * libwebsockets-test-client - libwebsockets test implementation
 *
 * Copyright (C) 2011 Andy Green <andy@warmcat.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation:
 *  version 2.1 of the License.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <libwebsockets.h>

static int was_closed;
static int deny_deflate;
static int deny_mux;
static volatile int force_exit = 0;

/*
 *  dumb-increment-protocol:  we connect to the server and print the number we are given
 */

static int
callback_dumb_increment(struct libwebsocket_context *this,
      struct libwebsocket *wsi,
      enum libwebsocket_callback_reasons reason,
                 void *user, void *in, size_t len)
{
  switch (reason) {

  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    fprintf(stderr, "callback_dumb_increment: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
    break;

  case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
    fprintf(stderr, "LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
    was_closed = 1;
    break;

  case LWS_CALLBACK_CLOSED:
    fprintf(stderr, "LWS_CALLBACK_CLOSED\n");
    was_closed = 1;
    break;

  case LWS_CALLBACK_CLIENT_RECEIVE:
    ((char *)in)[len] = '\0';
    fprintf(stderr, "rx %d '%s'\n", (int)len, (char *)in);
    break;

  case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
    if ((strcmp(in, "deflate-stream") == 0) && deny_deflate) {
      fprintf(stderr, "denied deflate-stream extension\n");
      return 1;
    }
    if ((strcmp(in, "deflate-frame") == 0) && deny_deflate) {
      fprintf(stderr, "denied deflate-frame extension\n");
      return 1;
    }
    if ((strcmp(in, "x-google-mux") == 0) && deny_mux) {
      fprintf(stderr, "denied x-google-mux extension\n");
      return 1;
    }

    break;

  default:
    break;
  }

  return 0;
}

/**
 * list of supported protocols and callbacks
 * https://github.com/warmcat/libwebsockets/blob/v1.4-chrome43-firefox-36/lib/libwebsockets.h#L923
 **/
static struct libwebsocket_protocols protocols[] = {
  {
    "increment protocol",
    callback_dumb_increment,
    0,
    20,
  },
  { NULL, NULL, 0, 0 } /* end */
};

void sighandler(int sig)
{
  force_exit = 1;
}

int main(int argc, char **argv)
{
  int n = 0;
  int ret = 0;
  struct libwebsocket_context *context;
  struct libwebsocket *wsi_dumb;
  struct lws_context_creation_info info;

  memset(&info, 0, sizeof info);

  signal(SIGINT, sighandler);

  /*
   * create the websockets context.  This tracks open connections and
   * knows how to route any traffic and which protocol version to use,
   * and if each connection is client or server side.
   *
   * For this client-only demo, we tell it to not listen on any port.
   */

  info.port = CONTEXT_PORT_NO_LISTEN;
  info.protocols = protocols;
  info.gid = -1;
  info.uid = -1;

  context = libwebsocket_create_context(&info);
  if (context == NULL) {
    fprintf(stderr, "Creating libwebsocket context failed\n");
    return 1;
  }

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
  const char *address = "echo.websocket.org";
  wsi_dumb = libwebsocket_client_connect(
      context,
      address,
      80,
      0,
      "",
      address,
      address,
      protocols[0].name,
      -1);

  if (wsi_dumb == NULL) {
    fprintf(stderr, "libwebsocket connect failed\n");
    ret = 1;
    goto bail;
  }

  fprintf(stderr, "Waiting for connect...\n");
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

  n = 0;
  while (n >= 0 && !was_closed && !force_exit) {
    n = libwebsocket_service(context, 10);

    if (n < 0)
      continue;
  }

bail:
  fprintf(stderr, "Exiting\n");

  libwebsocket_context_destroy(context);

  return ret;
}
