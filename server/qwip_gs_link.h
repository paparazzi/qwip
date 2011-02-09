#ifndef QWIP_GS_LINK_H
#define QWIP_GS_LINK_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <event.h>

/*
 *  link to remote ground segments
 */

struct GsLink {
  int listening_sock;
  struct sockaddr_in sockaddr_in;
  struct event listening_event;
};

extern struct GsLink* gs_link_new(int port);


#endif /* QWIP_GS_LINK_H */

