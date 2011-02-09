/******************************************************************************
 *    Copyright (C) 2006 by JOSEPH Nicolas                                    *
 *    gege2061@redaction-developpez.com                                       *
 *                                                                            *
 *    This program is free software; you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation; either version 2 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program; if not, write to the                           *
 *    Free Software Foundation, Inc.,                                         *
 *    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 ******************************************************************************/

#include "gmarkup-dom.h"

#include <stdlib.h>
#include <glib.h>

/* macros =================================================================== */
/* constants ================================================================ */
/* types ==================================================================== */
/* structures =============================================================== */
/* private variables ======================================================== */
/* private functions ======================================================== */
/* internal public functions ================================================ */
static void dom_print (GMarkupDomNode *root)
{
#define INDENT(niv) \
  do                                \
  {                                 \
    gint i;                         \
                                    \
    for (i = 0; i < (niv) - 1; i++) \
    {                               \
      g_print ("\t");               \
    }                               \
  } while (0)

  if (root != NULL)
  {
    INDENT (root->level);
    g_print ("<%s", root->name);
    {
      gint i = 0;

      while (root->attributs[i].name != NULL)
      {
        g_print (" %s=\"%s\"", root->attributs[i].name,
                               root->attributs[i].value);
        i++;
      }
    }
    g_print (">\n");
    if (root->content != NULL)
    {
      INDENT (root->level + 1);
      g_print ("%s\n", root->content);
    }
    dom_print (root->child);
    INDENT (root->level);
    g_print ("</%s>\n", root->name);
    dom_print (root->next);
  }
#undef INDENT
}

/* entry points ============================================================= */
int main (int argc, char **argv)
{
  if (argc == 2)
  {
    GMarkupDomNode *node = g_markup_dom_new (argv[1], NULL);
    dom_print (node);
    g_markup_dom_free (node), node = NULL;
  }
  else
  {
    g_error ("Usage : %s test.xml\n", argv[0]);
  }
  return EXIT_SUCCESS;
}

/* public variables ========================================================= */
