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

#include <stdlib.h>
#include <glib.h>
#include <stdio.h>

/* macros =================================================================== */
/* constants ================================================================ */
/* types ==================================================================== */
/* structures =============================================================== */
/* private variables ======================================================== */
/* private functions ======================================================== */
/* internal public functions ================================================ */
static void dom_print (GMarkupDomNode *root,FILE * sortie,gint pos)
{
#define INDENT(niv,sortie) \
  do                                \
  {                                 \
    gint i;                         \
                                    \
    for (i = 1; i < (niv) - 1; i++) \
    {                               \
      fprintf(sortie,"\t");         \
      g_print("\t");                \
    }                               \
  } while (0)

  gint texte=0;
  gint fils=0;
  gint com=0;

  if (root != NULL)
  {

    if (root->level!=0)
    {
      gint i = 0;

      INDENT (root->level,sortie);
      fprintf(sortie,"<%s", root->name);
      g_print("<%s", root->name);

      while (root->nb_att>i)
      {
        fprintf(sortie," %s=\"%s\"", root->attributs[i].name,
                               root->attributs[i].value);
        g_print(" %s=\"%s\"", root->attributs[i].name,
                               root->attributs[i].value);
        i++;
      }
    }
    if (root->fils+root->texte+root->nb_com!=0)
    {
      if (root->level!=0) fprintf(sortie,">\n");
      while (texte<root->texte || fils<root->fils || com<root->nb_com)
      {
        if (root->nb_com>com) if (root->com[com].item==(texte+fils+com))
        {
            INDENT (root->level + 1,sortie);
            fprintf(sortie,"%s\n", root->com[com].texte);
            g_print("%s\n", root->com[com].texte);
            com++;
        }
        if (root->texte>texte) if (root->content[texte].item==(texte+fils+com))
        {
            INDENT (root->level + 1,sortie);
            fprintf(sortie,"%s\n", root->content[texte].texte);
            g_print("%s\n", root->content[texte].texte);
            texte++;
        }
        if (root->fils>fils) if (root->child[fils].item==(fils+texte+com))
        {
            dom_print (root->child+fils,sortie,pos+1);
            fils++;
        }
      }
      if (root->level)
      {
       INDENT (root->level,sortie);
       fprintf(sortie,"</%s>\n", root->name);
       g_print("</%s>\n", root->name);
      }
    }
    else
      if (root->level)
      {
        fprintf(sortie,"/>\n");
        g_print("/>\n");
      }
  }
  #undef INDENT
}

/* entry points ============================================================= */
int main (int argc, char **argv)
{
  FILE * sortie=fopen("sortie.xml","w");

  if (argc == 2)
  {
    GMarkupDomNode *node = g_markup_dom_new (argv[1], NULL);
    dom_print (node,sortie,0);
    g_markup_dom_free (node);node=NULL;
  }
  else
  {
    g_error ("Usage : %s test.xml\n", argv[0]);
  }

  fclose(sortie);
  return EXIT_SUCCESS;
}

/* public variables ========================================================= */
