// Created:	Mon Nov  1 12:50:27 1999
// Author:	Andrey BETENEV
// Copyright (c) 1999-2020 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef EXPCAS_H
#define EXPCAS_H

#include <stdio.h>

#define EC_VERSION       "2.0"

struct ec_schema {       /* definition of schema */
  char *name;            /* name */
  struct ec_item *items; /* list of items */
  int num;               /* total amount of items */
  struct ec_item **array;/* array of items (indexed view of list) */
};

enum ecitem {            /* kind of schema item */
  ECIT_ENUM,             /* TYPE ENUMERATION */
  ECIT_SEL,              /* TYPE SELECT */
  ECIT_ALIAS,            /* TYPE as alias */
  ECIT_ENT               /* ENTITY */
};

struct ec_item {         /* definition of schema item */
  char *name;            /* item (class) name */
  char *package;         /* package name */
  enum ecitem kind;      /* kind of item */
  struct ec_type *alias; /* aliased type for ALIAS */
  struct ec_tlist *types;/* supertypes for ENTITY and list for SELECT and ENUMERATION */
  struct ec_field *field;/* fields for ENTITY */
  struct ec_item *next;  /* pointer to next item */
};

struct ec_tlist {        /* list of (type) names, for TYPE SELECT & ENUMERATION */
  char *name;
  struct ec_tlist *next;
};

enum ectype {            /* Types: */
  ECTYP_INT,             /* predefined type INTEGER */
  ECTYP_NUM,             /* predefined type NUMBER */
  ECTYP_DBL,             /* predefined type REAL */
  ECTYP_STR,             /* predefined type STRING */
  ECTYP_LOG,             /* predefined type LOGICAL */
  ECTYP_BOOL,            /* predefined type BOOLEAN */
  ECTYP_NAME,            /* named type (name of item in schema) */
  ECTYP_LIST,            /* LIST [] complex type */
  ECTYP_ARR,             /* ARRAY [] complex type */
  ECTYP_SET,             /* SET [] complex type */
  ECTYP_BAG              /* BAG [] complex type */
};

struct ec_type {         /* identification of type, either simple or complex */
  enum ectype kind;      /* kind of the type */
  char *name;            /* name for named type */
  int index[2];          /* start and end indices (or -1 if not defined) for complex types */
  struct ec_type *elem;  /* type of complex type elements */
};

struct ec_field {        /* field of ENTITY item */
  char *name;            /* field name */
  struct ec_type *type;  /* field type */
  int optional;          /* flag */
  struct ec_field *next; /* pointer to next field */
};

/*int yylex ( void );*/

#ifdef __cplusplus
extern "C" {
#endif

extern FILE *yyin;

int yyparse ( void );
int yyerror ( char *str );

/* Functions for creating schema representation structures */
struct ec_schema *mkschema ( char *name, struct ec_item *ilist );
struct ec_item *mkenum     ( char *name, struct ec_tlist *tlist );
struct ec_item *mkselect   ( char *name, struct ec_tlist *tlist );
struct ec_item *mkalias    ( char *name, struct ec_type *to );
struct ec_item *mkentity   ( char *name, struct ec_tlist *inherit, struct ec_field *fields );
struct ec_tlist *mktlist   ( char *name );
struct ec_type *mktstd     ( int keyword );
struct ec_type *mktname    ( char *name );
struct ec_type *mktset     ( int keyword, int ilow, int ihigh, struct ec_type *of );
struct ec_field *mkfield   ( char *name, struct ec_type *type, int optional );

/* Error reporting functions */
int ec_error ( char const *s, char const *text );
int ec_curline ( void );

/* Access to result of parsing */
char *ec_schemaname ( void );           /* returns name of schema */
int ec_nbitems ( void );                /* returns number of items in schema */
struct ec_item *ec_getitem ( int num ); /* returns item by index */
void ec_free ( void );                  /* frees all structures */

#ifdef __cplusplus
}
#endif

#endif
