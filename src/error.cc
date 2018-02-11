#include <string.h>
#include <stdlib.h>

#include "internal.h"

void
sqlite::error_set_sqlite_impl(error *err, error_code_args args)
{
   const char *msg = nullptr;
   char *duped = nullptr;

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
