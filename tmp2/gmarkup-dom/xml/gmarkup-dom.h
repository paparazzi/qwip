/******************************************************************************
 *    Copyright (C) 2006 by JOSEPH Nicolas                                    *
 *    gege2061@redaction-developpez.com                                       *
 *    Copyright (C) 2010 by SIAUD Bernard                                     *
 *    troumad@libertysurf.fr                                                  *
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

#ifndef H_NJ_MARKUP_DOM_061124
#define H_NJ_MARKUP_DOM_061124

#include <glib/gtypes.h>
#include <glib/gerror.h>

G_BEGIN_DECLS

/* macros =================================================================== */
/* constants ================================================================ */
/* types ==================================================================== */
typedef struct _xml_attr xml_attr;
typedef struct _GMarkupDomNode GMarkupDomNode;

/* structures =============================================================== */
struct _xml_attr
{
  gchar *name;
  gchar *value;
};

typedef struct {
           gchar * texte;
           gint    item;
         } contenu;

struct _GMarkupDomNode
{
  gchar *name;
  gint item; /* numéro de l'item dans la liste des items du père */
  gint level;
  contenu * content;
  gint texte; /* nombre d'entrées texte */
  xml_attr *attributs;
  gint nb_att; /* nombre d'attribut */
  contenu * com;
  gint nb_com; /* nombre de commentaires */
  struct _GMarkupDomNode *parent;
  struct _GMarkupDomNode *child;
  gint fils; /* nombre de nodes filles */
};

/* internal public functions ================================================ */
/* entry points ============================================================= */
GMarkupDomNode *g_markup_dom_new (const gchar *, GError **error);
void g_markup_dom_free (GMarkupDomNode *);

/* public variables ========================================================= */

G_END_DECLS

#endif /* not H_NJ_MARKUP_DOM_061124 */
