#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <event.h>
#include "sw/airborne/fms/fms_debug.h"
#include "sw/airborne/fms/fms_spi_link.h"
#include "sw/airborne/fms/fms_periodic.h"
#include "gs_shm.h"
#include "sw/in_progress/qwip/server/qwip_gs_link.h"
#include "sw/in_progress/qwip/server/qwip_settings.h"


struct GsServer {
  struct timespec start_time;
  struct GroundSegment* gs;
  struct GsLink* gs_link;

  struct event foo_event;
};

static struct GsServer gs_server;

static int main_init(void);
static void main_periodic(int my_sig_num);
static void main_trick_libevent(void);
static void on_foo_event(int fd, short event __attribute__((unused)), void *arg);



int main(int argc, char** argv) {

  TRACE(TRACE_DEBUG, "%s", "Starting initialization\n");
  main_init();
  main_trick_libevent();

  TRACE(TRACE_DEBUG, "%s", "Entering mainloop\n");
  
  /* Enter our mainloop */
  event_dispatch();

  return 0;
}

static int main_init(void) {

  /* Initializes shared memory */
  gs_server.gs = gs_shm_init();
  if (gs_server.gs == NULL) {
    perror("unable to initialize shared memory");
    return -1;
  }
  
#ifndef DISABLE_SPI_LINK
  /* Initalize our SPI link to IO processor */
  if (spi_link_init()) {
    TRACE(TRACE_ERROR, "%s", "failed to open SPI link \n");
    return -2;
  }
#endif  
  
  /* Initalize the event library */
  event_init();

  /* Initalize our ô so accurate periodic timer */
  if (fms_periodic_init(main_periodic)) {
    TRACE(TRACE_ERROR, "%s", "failed to start periodic generator\n");
    return -3; 
  }

  clock_gettime(CLOCK_MONOTONIC, &gs_server.start_time);

  /* Initializes the link to external ground segments */
  gs_server.gs_link = gs_link_new(4343);
  
  /* loads a local aircraft */
  gs_server.gs->nb_local_vehicles = 1;
  g_strlcpy(&gs_server.gs->local_vehicles[0].name, "Microjet", NAME_LEN);
  struct Settings* bla = settings_new_from_file("/home/paul/paparazzi/var/Microjet/settings.xml");
  //  settings_dump(bla);
  struct GS_Settings* s = &gs_server.gs->local_vehicles[0].settings;
  s->nb_settings = bla->nb_settings;
  int i;
  for (i=0; i<bla->nb_settings; i++) {
    struct Setting* sx = (struct Setting*)g_slist_nth_data(bla->settings, i);
    strcpy(&s->settings[i].var, sx->var);
    s->settings[i].value = sx->value;
  }


  return 0;
}


static void main_periodic(int my_sig_num __attribute__ ((unused))) {

   struct timespec now;
   clock_gettime(CLOCK_MONOTONIC, &now);

   gs_server.gs->time_running = now.tv_sec - gs_server.start_time.tv_sec;


}





#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void main_trick_libevent(void) {

  int fd = open("/dev/ttyS0", O_RDONLY);
  if (fd == -1) {
    TRACE(TRACE_ERROR, "%s", "failed to open /dev/null \n");
    return;
  }
  event_set(&gs_server.foo_event, fd, EV_READ | EV_PERSIST, on_foo_event, NULL);
  event_add(&gs_server.foo_event, NULL);

}

static void on_foo_event(int fd __attribute__((unused)), short event __attribute__((unused)), void *arg __attribute__((unused))) {

}
