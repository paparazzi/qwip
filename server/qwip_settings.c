#include "sw/in_progress/qwip/server/qwip_settings.h"

#include <stdlib.h>
#include <stdio.h>

static void xml_start_element (GMarkupParseContext *context, const gchar *element_name,
			       const gchar **attribute_names,
			       const gchar **attribute_values, gpointer user_data,
			       GError **error);

static void xml_end_element (GMarkupParseContext *context,
                             const gchar *element_name, gpointer user_data,
                             GError **error);

static void xml_text (GMarkupParseContext *context, const gchar *text,
                      gsize text_len, gpointer user_data, GError **error);

static void xml_com (GMarkupParseContext *context, const gchar *text,
		     gsize text_len, gpointer user_data, GError **error);





struct Settings* settings_new_from_file( const char* filename) {

  struct Settings* me = malloc(sizeof(struct Settings));
  me->nb_settings = 0;
  me->settings = NULL;
  me->settings_tree = NULL;
  me->cur_node = NULL;

  GError *error = NULL;
  gchar *text = NULL;
  gsize length = -1;
  g_file_get_contents (filename, &text, &length, &error);
  if (error) {
    g_free(error);
    return NULL;
  }
  
  /* http://library.gnome.org/devel/glib/stable/glib-Simple-XML-Subset-Parser.html#GMarkupParser */

  GMarkupParser markup_parser = { .start_element = xml_start_element,
				  .end_element = xml_end_element,    
				  .text = xml_text,                  
				  .passthrough = xml_com,            
				  .error = NULL };
  GMarkupParseContext* markup_parse_context = g_markup_parse_context_new (&markup_parser, 0,
									  me, NULL);
  
  g_markup_parse_context_parse (markup_parse_context, text, length, &error);

  g_free (markup_parse_context);
  g_free (text);
    
  return me;
}



static void xml_start_element (GMarkupParseContext *context, const gchar *element_name,
			       const gchar **attribute_names,
			       const gchar **attribute_values, gpointer user_data,
			       GError **error) {

  struct Settings* me = (struct Settings*)user_data;
  //  printf("in  xml_start_element %s\n", element_name);
  if (!g_strcmp0(element_name, "dl_settings")) {
    //    printf("in  xml_start_element %s\n", element_name);
    struct SettingsPage* new_page = malloc(sizeof(struct SettingsPage));
    new_page->name = NULL;
    new_page->settings = NULL;
    GNode* new_node =  g_node_new (new_page);
    if (!me->settings_tree)
      me->settings_tree = new_node;
    else
      g_node_append(me->settings_tree, new_node);
    int i=0;
    while (attribute_names[i]) {
      //      printf("  %s -> %s\n", attribute_names[i], attribute_values[i]);
      if (!g_strcmp0(attribute_names[i], "name"))	
	new_page->name = g_strdup(attribute_values[i]);
      i++;
    }
    me->cur_node = new_node;
  }
  else if (!g_strcmp0(element_name, "dl_setting")) {
    struct Setting* s = malloc(sizeof(struct Setting));
    me->nb_settings++;
    me->settings = g_slist_append(me->settings, s);
    struct SettingsPage* p = (struct SettingsPage*)me->cur_node->data;
    p->settings = g_slist_append(p->settings, s);
    int i=0;
    while (attribute_names[i]) {
      //      printf("  %s -> %s\n", attribute_names[i], attribute_values[i]);
      if (!g_strcmp0(attribute_names[i], "var"))	
	s->var = g_strdup(attribute_values[i]);
      else if (!g_strcmp0(attribute_names[i], "value"))
	s->value = g_ascii_strtod (attribute_values[i], NULL);
      else if (!g_strcmp0(attribute_names[i], "min"))
	s->min = g_ascii_strtod (attribute_values[i], NULL);
      else if (!g_strcmp0(attribute_names[i], "max"))
	s->max = g_ascii_strtod (attribute_values[i], NULL);
      else if (!g_strcmp0(attribute_names[i], "step"))
	s->step = g_ascii_strtod (attribute_values[i], NULL);
      else if (!g_strcmp0(attribute_names[i], "shortname"))	
	s->shortname = g_strdup(attribute_values[i]);
      else if (!g_strcmp0(attribute_names[i], "module"))	
	s->module = g_strdup(attribute_values[i]);
      else if (!g_strcmp0(attribute_names[i], "handler"))	
	s->handler = g_strdup(attribute_values[i]);
      i++;
    }
  }


}

static void xml_end_element (GMarkupParseContext *context,
                             const gchar *element_name, gpointer user_data,
                             GError **error) {

  struct Settings* me = (struct Settings*)user_data;
  //  printf("in  xml_end_element %s\n\n", element_name);
  if (!g_strcmp0(element_name, "dl_settings")) {
    //    printf("in  xml_end_element %s\n\n", element_name);
    if (me->cur_node->parent)
      me->cur_node = me->cur_node->parent;
  }
}

static void xml_text (GMarkupParseContext *context, const gchar *text,
                      gsize text_len, gpointer user_data, GError **error) {}

static void xml_com (GMarkupParseContext *context, const gchar *text,
		     gsize text_len, gpointer user_data, GError **error) {}



static gboolean    dump_page(GNode *node, gpointer data);
static gboolean    dump_page(GNode *node, gpointer data) {

  struct SettingsPage* p = (struct SettingsPage*)node->data;
  printf("page name : %s\n", p->name);
  for (int i=0; i<g_slist_length(p->settings); i++) {
    struct Setting* s = g_slist_nth_data(p->settings, i);
    printf("%d %s (%s)\n", i, s->shortname, s->var);
  }
  return FALSE;
}

void settings_dump(struct Settings* me) {

  // dump the list of settings
  printf("me: %d %d\n", me->nb_settings, g_slist_length(me->settings) );
  int i;
  for (i=0; i<g_slist_length(me->settings); i++) {
    struct Setting* s = g_slist_nth_data(me->settings, i);
    printf("%d %s (%s)\n", i, s->shortname, s->var);
  }
  // dump them by page
  g_node_traverse(me->settings_tree, G_IN_ORDER, G_TRAVERSE_ALL, -1, dump_page, NULL);


}


