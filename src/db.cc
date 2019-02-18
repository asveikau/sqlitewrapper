/*
 Copyright (C) 2017, 2018 Andrew Sveikauskas

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
*/

#include <common/misc.h>
#include <common/path.h>
#include <string.h>

#include "internal.h"

sqlite::sqlite::sqlite()
  : db(nullptr)
{
}

sqlite::sqlite::~sqlite()
{
   close();
}

void
sqlite::sqlite::error_set_sqlite_impl(error *err, error_code_args args)
{
   ::sqlite::error_set_sqlite_impl(err, args);
}

void 
sqlite::sqlite::open(const char *filename, int flags, const char *vfs, error *err)
{
   int rc;
   char *walFile = nullptr;

   init_library(err);
   ERROR_CHECK(err);

   close();

   rc = sqlite3_open_v2(filename, &db, flags, vfs);
   if (rc)
   {
      // Sometimes we fail to open if there was an interrupted write,
      // and a writeable handle seems to do ok.  Try to hack around
      // this by re-trying for writing.
      //
      if (rc == SQLITE_CANTOPEN &&
          !(flags & SQLITE_OPEN_READWRITE))
      {
         static const char suffix[] = "-wal";
         size_t len = strlen(filename);

         walFile = (char*)malloc(len + sizeof(suffix));
         if (walFile)
         {
            memcpy(walFile, filename, len);
            memcpy(walFile + len, suffix, sizeof(suffix));

            #define exit cont
            if (path_exists(walFile, err))
            {
               open(filename, flags | SQLITE_OPEN_READWRITE, vfs, err);
               ERROR_CHECK(err);

               close();

               int rc2 = sqlite3_open_v2(filename, &db, flags, vfs);

               if (!rc2)
               {
                  rc = rc2;
                  goto skip;
               }
            }
            else
            {
               ERROR_CHECK(err);
            }
            #undef exit
         }
      }
   cont:
      error_clear(err);
      ERROR_SET(err, sqlite, rc);
   skip:;
   }

   if ((flags & SQLITE_OPEN_READWRITE))
   {
      static const char * const openingStatements[] =
      {
         "PRAGMA journal_mode = WAL",
         "PRAGMA synchronous = FULL",
         nullptr
      };
      exec(openingStatements, err);
      ERROR_CHECK(err);
   }
exit:
   free(walFile);
}

void
sqlite::sqlite::open(const char *filename, error *err)
{
   open(filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, err);
}

void
sqlite::sqlite::close()
{
   if (db)
   {
      sqlite3_close(db);
      db = nullptr;
   }
}

void
sqlite::sqlite::check_open(error *err)
{
   if (!db)
      ERROR_SET(err, unknown, "Database is not open");
exit:;
}

void
sqlite::sqlite::prepare(
   const char *sql,
   statement &stmt,
   error *err
)
{
   int rc;
   int len = strlen(sql);
   const char *tail = nullptr;

   check_open(err);
   ERROR_CHECK(err);

   stmt.close();

   rc = sqlite3_prepare_v2(
      db,
      sql,
      len,
      &stmt.stmt,
      &tail
   );
   if (rc)
      ERROR_SET(err, sqlite, db, rc);

   if (tail && *tail)
      ERROR_SET(err, unknown, "Unused junk at the end of sql");

   stmt.db = this;
exit:;
}

void
sqlite::sqlite::exec(const char *sql, error *err)
{
   statement stmt;

   prepare(sql, stmt, err);
   ERROR_CHECK(err);

   stmt.step(err);
   ERROR_CHECK(err);
exit:;
}

void
sqlite::sqlite::exec(const char * const *p, error *err)
{
   statement stmt;

   while (*p)
   {
      prepare(*p++, stmt, err);
      ERROR_CHECK(err);

      stmt.step(err);
      ERROR_CHECK(err);
   }
exit:;
}

void
sqlite::sqlite::exec(error *err, const char *sql0, ...)
{
   if (!sql0)
      return;

   statement stmt;
   va_list ap;
   const char *sql;

   va_start(ap, sql0);

   for (sql = sql0; sql; sql = va_arg(ap, const char *))
   {
      prepare(sql, stmt, err);
      ERROR_CHECK(err);

      stmt.step(err);
      ERROR_CHECK(err);
   }

exit:
   va_end(ap);
}

bool
sqlite::sqlite::table_exists(const char *name, error *err)
{
   statement stmt;
   bool r = false;

   check_open(err);
   ERROR_CHECK(err);

   prepare("SELECT 1 from sqlite_master where type='table' and name = ?", stmt, err);
   ERROR_CHECK(err);

   stmt.bind(0, name, err);
   ERROR_CHECK(err);

   r = stmt.step(err);
   ERROR_CHECK(err);

exit:
   return r;
}
