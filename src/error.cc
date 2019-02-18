/*
 Copyright (C) 2017, 2018 Andrew Sveikauskas

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
*/

#include <string.h>
#include <stdlib.h>

#include "internal.h"

void
sqlite::error_set_sqlite_impl(error *err, error_code_args args)
{
   const char *msg = nullptr;
   char *duped = nullptr;

   if (args.rc == SQLITE_NOMEM)
   {
      error_set_nomem(err);
      return;
   }

   if (args.db)
   {
      msg = sqlite3_errmsg(args.db);
   }

   if (!msg)
   {
      msg = sqlite3_errstr(args.rc);
   }

   if (msg)
      duped = strdup(msg);

   err->source = SOURCE_SQLITE;
   err->code = args.rc;
   err->context = duped;
   if (duped)
   {
      err->get_string = [] (error *r) -> const char * { return (const char*)r->context; };
      err->free_fn = free;
   }
   else
   {
      err->get_string = nullptr;
      err->free_fn = nullptr;
   }
}
