#ifndef QWIP_SETTINGS_H
#define QWIP_SETTINGS_H

#include <glib.h>
#include <inttypes.h>

struct Setting {
  const char* var;
  float value;
  float min;
  float max;
  float step;
  const char* shortname;
  const char* module;
  const char* handler;
};

struct SettingsPage {
  const char* name;
  GSList* settings;
};

struct Settings {
  int32_t nb_settings;
  GSList* settings;
  GNode*  settings_tree;
  GNode*  cur_node;
};

extern struct Settings* settings_new_from_file( const char* filename);

extern void settings_dump(struct Settings*);

#endif /* QWIP_SETTINGS_H */
