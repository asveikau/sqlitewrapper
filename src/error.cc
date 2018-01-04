#include <sqlitewrapper.h>

#include <string.h>
#include <stdlib.h>

#define SOURCE_SQLITE \
    (error_source)((('s' << 24) | ('q' << 16)  | ('l' << 8) | ('t')))

void
sqlite::error_set_sqlite(error *err, error_code_args args)
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

void
sqlite::error_set_sqlite(error *err, int rc)
{
   error_set_sqlite(err, error_code_args(rc));
}
