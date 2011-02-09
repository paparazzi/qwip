#ifndef QWIP_GROUND_SEGMENT_H
#define QWIP_GROUND_SEGMENT_H

#include <inttypes.h>

#include "sw/airborne/math/pprz_geodetic_double.h"

#define NAME_LEN 128

struct GS_Setting {
  char  var[NAME_LEN];
  char  shortname[NAME_LEN];
  float value;
  float min;
  float max;
  float step;
};

#define GS_MAX_SETTINGS_PAGES 32
#define GS_MAX_SETTINGS 128
struct GS_Settings_Page {
  char  name[NAME_LEN];
  uint8_t nb_settings;
  struct GS_Setting* settings[GS_MAX_SETTINGS];
};

struct GS_Settings {
  uint8_t nb_settings;
  struct GS_Setting settings[GS_MAX_SETTINGS];
  uint8_t nb_pages;
  struct GS_Settings_Page pages[GS_MAX_SETTINGS_PAGES];
};

enum RcStatus {
  RC_OK,
  RC_LOST,
  RC_REALLY_LOST,
  RC_NOT_AVAILABLE,
};

struct GS_Vehicle {
  uint8_t id;
  char    name[NAME_LEN];
  float   cpu_time;
  float   supply;
  enum RcStatus rc_status;
  struct LlaCoor_d pos;
  struct GS_Settings settings;
};

#define GS_MAX_VEHICLES 128

struct GroundSegment {
  char name[NAME_LEN];
  uint32_t time_running;
  uint32_t nb_local_vehicles;
  struct GS_Vehicle local_vehicles[GS_MAX_VEHICLES];
};

#define GS_SHM_KEY    43

#endif /* QWIP_GROUND_SEGMENT_H */
