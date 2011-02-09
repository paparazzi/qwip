// http://www.purplepixie.org/cgi/download/cgi_interface.cpp

#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <glib.h>

#include "../server/ground_segment.h"

static void print_error(const char* msg);
static void print_flight_param(GData* req, struct GroundSegment *gs);
static void print_settings(GData* req, struct GroundSegment *gs);
static void print_fleet(GData* req, struct GroundSegment *gs);
static GData* parse_request(void);

const char* rc_status[] = { "ok", "lost", "really lost", "not available" };

int main(int argc, char** argv, char **envp) {



  int  shmid = shmget((key_t)GS_SHM_KEY, sizeof(struct GroundSegment), 0440);
  if (shmid ==-1) { 
    print_error("No ground segment running");
    perror ("shmget");
    return 0; 
  }

  struct GroundSegment *gs = shmat(shmid,NULL,0);
  if (gs == (struct GroundSegment *)-1) {
    perror ("shmat");
    return -2; 
  }

  GData *req = parse_request();
  const char* req_type = (char*)g_datalist_get_data(&req, "req");
  if (!g_strcmp0(req_type, "fleet"))
    print_fleet(req, gs);
  else if (!g_strcmp0(req_type, "settings"))
    print_settings(req, gs);
  else if (!g_strcmp0(req_type, "flight_param"))
    print_flight_param(req, gs);
  else
    print_error("unknown request");

  if (shmdt(gs)) {
    perror ("shmdt");
    return -3; 
  }

  return 0;
}


static GData* parse_request(void) {

  GData* req;
  g_datalist_init(&req);

  char* query_string = getenv("QUERY_STRING");
  //char* query_string = "req=fleet";
  gchar** pairs = g_strsplit(query_string, "&", -1);
  char** p = pairs;
  while (*p) {
    gchar** kv =  g_strsplit(*p, "=", -1);
    if (kv[0] && kv[1]) {
      g_datalist_set_data(&req, kv[0], strdup(kv[1]));
    }
    g_strfreev(kv);
    p++;

  }
  g_strfreev(pairs);

  return req;
}

static void print_fleet(GData* req, struct GroundSegment *gs) {
  printf("Content-type: application/xml\n\n");
  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
  printf("<content>\n");
  printf("  <time_running>%d s</time_running>\n", gs->time_running);
  printf("  <aircrafts>\n");
  int i;
  for (i=0; i<gs->nb_local_vehicles; i++)
  printf("    <aircraft name=\"%s\"/>\n", gs->local_vehicles[i].name);
  printf("  </aircrafts>\n");
  printf("</content>\n\n");
}


static void print_settings(GData* req, struct GroundSegment *gs) {


  //  struct GS_Vehicle* v = gs_get_ac_by_name(gs);
  struct GS_Vehicle* v = &gs->local_vehicles[0];
  if (!v) { print_error("no such vehicle"); return;}
  struct GS_Settings* s = &v->settings;
  printf("Content-type: application/xml\n\n");
  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
  printf("<content>\n");
  printf("  <time_running>%d s</time_running>\n", gs->time_running);
  printf("  <settings>\n");
  for (int i=0; i<s->nb_settings; i++)
    printf("    <setting var=\"%s\" value=\"%f\"/>\n", s->settings[i].var, s->settings[i].value);
  printf("  </setings>\n");
  //  int i;
  //  for (i=0; i<argc; i++)
  //    fprintf(stderr, "<bla>%s</bla>\n",  argv[i]);
  //  i=0;
  //  while (envp[i]) {
  //    printf("<env>%d : %s</env>\n", i, envp[i]);
  //    i++;
  //  }
  printf("<req>ac   %s</req>\n", (char*)g_datalist_get_data(&req, "ac"));
  printf("<req>req %s</req>\n", (char*)g_datalist_get_data(&req, "req"));

  char* query_string = getenv("QUERY_STRING");
  printf("<env>%s</env>\n", query_string);

  printf("</content>\n\n");

}


static void print_flight_param(GData* req, struct GroundSegment *gs) {

  struct GS_Vehicle* v = &gs->local_vehicles[0];
  
  printf("Content-type: application/xml\n\n");

  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
  printf("<content>\n");
  printf("  <time_running>%d s</time_running>\n", gs->time_running);
  printf("  <cpu_time>%.1f s</cpu_time>\n", v->cpu_time);
  printf("  <supply>%.1f V</supply>\n", v->supply);
  printf("  <radio_control>%s</radio_control>\n", rc_status[v->rc_status]);
  printf("  <latitude>%.7f</latitude>\n", v->pos.lat/M_PI*180.);
  printf("  <longitude>%.7f</longitude>\n", v->pos.lon/M_PI*180.);
  printf("</content>\n\n");

}

static void print_error(const char* msg) {

  printf("Content-type: application/xml\n\n");
  printf("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
  printf("<content>\n");
  printf("  <error>%s</error>\n", msg);
  printf("</content>\n\n");

}

