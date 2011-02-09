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

#include "gmarkup-dom.h"

#include <ctype.h>

#include <glib/gfileutils.h>
#include <glib/gmarkup.h>
#include <glib/gmem.h>
#include <glib/gmessages.h>
#include <glib/gstrfuncs.h>
#include <malloc.h>

/* macros =================================================================== */
/* constants ================================================================ */
/* types ==================================================================== */
typedef struct _GMarkupDomContext GMarkupDomContext;

/* structures =============================================================== */
struct _GMarkupDomContext
{
  gint level;
  gint item;
  GMarkupDomNode *root;
  GMarkupDomNode *current;
};

/* private variables ======================================================== */
/* private functions ======================================================== */
gchar * str_isspace (const gchar *s)
{
  gint i;
  gchar * ch;

  if (!s) return 0;
  for (; *s && isspace (*s); s++);
  if (*s)
  {
      for(i=0;*(s+i);i++);
      for(;isspace(s[i]);i--);

      ch=g_strdup(s);
      ch[i+1]=0;

      return ch;
  }
  return NULL;;
}

void xml_start_element (GMarkupParseContext *context, const gchar *element_name,
                        const gchar **attribute_names,
                        const gchar **attribute_values, gpointer user_data,
                        GError **error)
{
  gint i;
  GMarkupDomContext *dom_context = user_data;
  GMarkupDomNode *node = NULL, * parent;

  g_return_if_fail (dom_context != NULL); /* déclaration et initialisation du nouveau élément  */

 /* déclaration et initialisation du nouveau élément  */

  if (dom_context->root) /* si ce n'est pas la première node, la node mère du système */
  {
      for (parent=dom_context->current;dom_context->level<parent->level;parent=parent->parent)
      {
       printf("%s\n",parent->name);
      }
      parent->fils++;                 /* un for car il est possible qu'on vienne de descendre de niveaux */
      parent->child=realloc(parent->child,sizeof (*node)*parent->fils); /* on rajoute un fils au père    */
      node=parent->child+parent->fils-1; /* on récupère le lien vers la nouvelle node crée chez le père  */
      node->parent=parent;            /* j'informe sur le parent de cette node                           */
      node->item=dom_context->item;   /* j'informe sur l'ordre de cet élément dans l'ensemble des        */
  }                                   /* éléments (texte, node et commntaires) du père                   */
  else /* si c'est le premier élément */
  {
    node = g_malloc (sizeof (*node));
    dom_context->root = node;
    node->item=0;
  }
  dom_context->current = node;

  /* variable générale de l'état actuel du système  */
  /* la construction d'un nouveau élément est toujours demandé par le père donc on */
  dom_context->level++;          /* vient de monter le niveau de 1 . mise dans  la */
  dom_context->item=0;           /* pas encore d'entrée dans cette node            */


  /* initiaisation de la node */
  node->name = g_strdup (element_name);
  node->content = NULL;
  node->texte   = 0;     /* pas encore de texte        */
  node->com = NULL;
  node->nb_com  = 0;     /* pas encore de commentaires */
  node->child = NULL;
  node->fils=0;          /* pas encore de fils         */
  node->level = dom_context->level; /* niveau du noeud */

  /* Copy attributs */
  for (i = 0; attribute_names[i] != NULL; i++); /* on compte le nombre d'attributs */
  node->attributs = g_malloc (sizeof (*node->attributs)*i);
  node->nb_att=i;
  for (i = 0; attribute_names[i] != NULL; i++) /* on parcourt les attributs        */
  {
                                            /* on informe leur nom et valeur       */
    node->attributs[i].name = g_strdup (attribute_names[i]);
    node->attributs[i].value = g_strdup (attribute_values[i]);
  }

  /* Unused parameters */
  (void)context;
  (void)error;
}



static void xml_end_element (GMarkupParseContext *context,
                             const gchar *element_name, gpointer user_data,
                             GError **error)
{
  GMarkupDomContext *dom_context = user_data;

  g_return_if_fail (dom_context != NULL);
  g_return_if_fail (dom_context->current != NULL);

  /* je rends la main au père */
  dom_context->item=dom_context->current->item+1;        /* je passe à l'élément suivant du père */
  dom_context->level--;                                  /* le père a un niveau de moins         */
  dom_context->current   = dom_context->current->parent; /* la node courante se ra celle du père */

  /* Unused parameters */
  (void)context;
  (void)element_name;
  (void)error;
}

static void xml_text (GMarkupParseContext *context, const gchar *text,
                      gsize text_len, gpointer user_data, GError **error)
{
  GMarkupDomContext *dom_context = user_data;

  g_return_if_fail (dom_context != NULL);
  g_return_if_fail (dom_context->current != NULL);
  gchar * ch;

  if ((ch=str_isspace (text)))/* si le texte n'est pas vide (saut de ligne ou espace ou tabulation */
  {                       /* on rajoute cet élément (voir rajout d'une node : c'est pareil     */
    dom_context->current->texte++;
    dom_context->current->content=realloc(dom_context->current->content,dom_context->current->texte*sizeof(contenu));
    dom_context->current->content[dom_context->current->texte-1].item=dom_context->item;
    dom_context->item++;
    dom_context->current->content[dom_context->current->texte-1].texte = ch;
  }

  /* Unused parameters */
  (void)context;
  (void)text_len;
  (void)error;
}

static void xml_com (GMarkupParseContext *context, const gchar *text,
                      gsize text_len, gpointer user_data, GError **error)
{
  GMarkupDomContext *dom_context = user_data;

  g_return_if_fail (dom_context != NULL);
  g_return_if_fail (dom_context->current != NULL);
  gchar * ch;

  if ((ch=str_isspace (text))) /* si le texte n'est pas vide (saut de ligne ou espace ou tabulation */
  if ((ch=str_isspace (text))) /* si le texte n'est pas vide (saut de ligne ou espace ou tabulation */
  {                        /* on rajoute cet élément (voir rajout d'une node : c'est pareil     */
    dom_context->current->nb_com++;
    dom_context->current->com=realloc(dom_context->current->com,dom_context->current->nb_com*sizeof(contenu));
    dom_context->current->com[dom_context->current->nb_com-1].item=dom_context->item;
    dom_context->item++;
    dom_context->current->com[dom_context->current->nb_com-1].texte = ch;
  }

  /* Unused parameters */
  (void)context;
  (void)text_len;
  (void)error;
}

/* internal public functions ================================================ */
/* entry points ============================================================= */
/**
 * g_markup_dom_new:
 * @file_name: name of a file to parse contents from.
 * @error: return location for a #GError, or %NULL.
 *
 * Create a dom tree of @filename content.
 *
 * Return value: a new #GMarkupDomNode.
 **/
GMarkupDomNode *g_markup_dom_new (const gchar *filename, GError **error)
{
  GMarkupParseContext *markup_parse_context;
  GMarkupDomNode * pere = (GMarkupDomNode *)malloc(sizeof(GMarkupDomNode));
   /* pere doit être alloué dynamiquement car il est utilisé par la fonction appelante */
  GMarkupDomContext context={0,0,pere,pere};
   /* ce n'est pas le cas de context qui n'est utilisé que les les fonctions appelées  */

  pere->name=NULL; /* initialisation de la racine du fichier xml */
  pere->level=0;
  pere->item=0;
  pere->content=NULL;
  pere->texte=0;
  pere->attributs=NULL;
  pere->nb_att=0;
  pere->com=NULL;
  pere->nb_com=0;
  pere->parent=NULL;
  pere->child=NULL;
  pere->fils=0;

  g_return_val_if_fail (filename != NULL, context.root);
                  /* sort de la fonction si filename ne pointe pas sur une chaîne de caractères */

  {         /* création du contexte : fonctions à appeler suivant la nature de l'élément traité */
    GMarkupParser markup_parser; /* http://library.gnome.org/devel/glib/stable/glib-Simple-XML-Subset-Parser.html#GMarkupParser */

    markup_parser.start_element = xml_start_element; /* cas : ouverture de balise               */
    markup_parser.end_element = xml_end_element;     /* cas : fermeture de balise               */
    markup_parser.text = xml_text;                   /* cas : texte entre les balises           */
    markup_parser.passthrough = xml_com;             /* cas : commentaires entre les balises    */
    markup_parser.error = NULL;                      /* cas : erreur dans le fichier xml        */
    markup_parse_context = g_markup_parse_context_new (&markup_parser, 0,
                                                       &context, NULL);  /* http://library.gnome.org/devel/glib/stable/glib-Simple-XML-Subset-Parser.html#g-markup-parse-context-new */
  } /* maintenant, on sait comment parcourir le fichier */
  {
    gchar *text = NULL;
    gsize length = -1;

    g_file_get_contents (filename, &text, &length, error);
    /* passe tout le fichier filename dans la chaîne de caractères text
                                                        dont la longueur sera dans lenght */
    if (text != NULL) /* si on a récupéré le fichier */
    {
      g_markup_parse_context_parse (markup_parse_context, text, length, error);  /* on lance le parsage du fichier xml */
      g_free (text), text = NULL;                                                /* on lance le parsage du fichier xml */
    }
    g_free (markup_parse_context), markup_parse_context = NULL;
  }
  return pere;
}

/**
 * g_markup_dom_free:
 * @node: a #GMarkupDomNode.
 *
 * Frees a #GMarkupDomNode.
 **/
void g_markup_dom_free (GMarkupDomNode *node)
{
  if (node != NULL)
  {
    gint i;
    g_free (node->name), node->name = NULL;
    for (i = 0; node->nb_att>i; i++)
    {
      g_free (node->attributs[i].name), node->attributs[i].name = NULL;
      g_free (node->attributs[i].value), node->attributs[i].value = NULL;
    }
    g_free (node->attributs), node->attributs = NULL;
    for (i = 0; node->texte>i; i++)
    {
      g_free (node->content[i].texte);
    }
    g_free(node->content);
    for (i = 0; node->nb_com>i; i++)
    {
      g_free (node->com[i].texte);
    }
    g_free(node->com);
    for (i = 0; node->fils>i; i++)
    {
      g_markup_dom_free (node->child+i);
    }
    g_free(node->child), node->child = NULL;
  }
}

/* public variables ========================================================= */
