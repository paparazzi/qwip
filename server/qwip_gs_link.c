#include "sw/in_progress/qwip/server/qwip_gs_link.h"

#include <stdlib.h>
#include <stdio.h>

static void on_socket_event(int fd __attribute__((unused)), short event __attribute__((unused)), void *arg __attribute__((unused)));

struct GsLink* gs_link_new(int port) {

  struct GsLink* me = malloc(sizeof(struct GsLink));

  /* create the listening socket */
  me->listening_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (me->listening_sock < 0) {
    perror("socket");
    free(me);
    return NULL;
  }

  /* avoids having to wait for badly closed sockets timeout */
  int on = 1;
  if (setsockopt(me->listening_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    free(me);
    return NULL;
  }
  
  /* bind the listening socket to a port */
  me->sockaddr_in.sin_family = AF_INET;
  me->sockaddr_in.sin_addr.s_addr = INADDR_ANY;
  me->sockaddr_in.sin_port = htons(port);
  if (bind(me->listening_sock, (struct sockaddr *) &me->sockaddr_in, sizeof(me->sockaddr_in)) == -1) {
    perror("bind");
    free(me);
    return NULL;
  }
 
  /* makes this socket listen for connections */
  if (listen(me->listening_sock, 5)) {
    perror("listen");
    free(me);
    return NULL;
  }
  printf("gs server listening on port %d\n", port);

  event_set(&me->listening_event, me->listening_sock, EV_READ | EV_PERSIST, on_socket_event, NULL);
  event_add(&me->listening_event, NULL);

  return me;
}


static void on_socket_event(int fd __attribute__((unused)), short event __attribute__((unused)), void *arg __attribute__((unused))) {

 printf("on_socket_event\n");
 
}


