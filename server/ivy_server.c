#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <glib.h>
#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "ground_segment.h"
#include "gs_shm.h"
#include "sw/airborne/math/pprz_geodetic_double.h"



static void on_ivy_msg(IvyClientPtr app, void *user_data, int argc, char *argv[]);
static gboolean on_timeout(gpointer user_data);
static void on_sigint(int _sig_num);
static void setup_ext_gs_link(char* hostname, int port);

struct IvyGsGateway {
  struct GroundSegment* gs;
  int sockfd;
  struct sockaddr_in serv_addr;
};

static struct IvyGsGateway ivy_gs_gateway;

int main(int argc, char** argv) {
  
  /* try to setup network connection */
  setup_ext_gs_link("localhost", 4343);

#if 0
  /* initializes shared memory */
  ivy_gs_gateway.gs = gs_shm_init();
  if (ivy_gs_gateway.gs == NULL) {
    perror("unable to initialize shared memory");
    return -1;
  }
#endif

  /* attaches sigint callback in order to close properly */
  struct sigaction my_new_sigaction = {
    .sa_handler = on_sigint
  };
  if (sigaction(SIGINT, &my_new_sigaction, NULL)) {
    perror("could not install SIGINT handler\n");
    return -1;
  }

  GMainLoop *ml =  g_main_loop_new(NULL, FALSE);
  g_timeout_add(500, on_timeout, NULL);
  IvyInit ("Ivy2cgi", "Ivy2cgi READY", NULL, NULL, NULL, NULL);
  IvyBindMsg(on_ivy_msg, NULL, "^\\S* GPS (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*) (\\S*)");
  IvyStart("127.255.255.255");

  g_main_loop_run(ml);

  return 0;
}


/*
 * Signal handler
 * 
 */
static void on_sigint(int _sig_num) {
  printf("on sigint\n");
#if 0
  gs_shm_close(ivy_gs_gateway.gs);
#endif
  exit(0);
}



/*
 * periodic callback
 *
 */
static gboolean on_timeout(gpointer user_data) {
#if 0
  struct GS_Vehicle* v = &(ivy_gs_gateway.gs->local_vehicles[0]);
  //  printf("hello\n");
  v->cpu_time += 1.0;
#endif
  return TRUE;
}


/*
 * Ivy callback
 *
 */
static void on_ivy_msg(IvyClientPtr app, void *user_data, int argc, char *argv[]){

#if 1
  struct GS_Vehicle* v = &(ivy_gs_gateway.gs->local_vehicles[0]);

  v->cpu_time += 1.0;

  struct UTMCoor_d utm;
  utm.east  =  atof(argv[1])*1e-2;
  utm.north =  atof(argv[2])*1e-2;
  utm.zone  =  atoi(argv[9]);

  //  printf("utm e:%f n:%f z:%d\n", utm.east, utm.north, utm.zone);

  lla_of_utm(&v->pos, &utm);

  //  printf("lla %f %f\n", v->pos.lat/M_PI*180., v->pos.lon/M_PI*180. );
#endif

}


/*
 * Setup a network connection with a remote GS
 *
 */
static void setup_ext_gs_link(char* hostname, int port) {
 
  ivy_gs_gateway.sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (ivy_gs_gateway.sockfd < 0) {
    perror("ERROR opening socket");
    return;
  }
  struct hostent *server;
  server = gethostbyname(hostname);
  if (server == NULL) {
    perror("gs_link gethostbyname\n");
    return;
  }
  bzero((char *) & ivy_gs_gateway.serv_addr, sizeof(ivy_gs_gateway.serv_addr));
  ivy_gs_gateway.serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&ivy_gs_gateway.serv_addr.sin_addr.s_addr,
	server->h_length);
  ivy_gs_gateway.serv_addr.sin_port = htons(port);
  if (connect(ivy_gs_gateway.sockfd,
	      (const struct sockaddr *)&ivy_gs_gateway.serv_addr,
	      sizeof(ivy_gs_gateway.serv_addr)) < 0) {
    perror("gs_link connecting");
    return;
  }
  
}

