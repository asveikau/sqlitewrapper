#include <common/misc.h>
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
sqlite::sqlite::open(const char *filename, int flags, const char *vfs, error *err)
{
   int rc;

   init_library(err);
   ERROR_CHECK(err);

   close();

   rc = sqlite3_open_v2(filename, &db, flags, vfs);
   if (rc)
      ERROR_SET(err, sqlite, rc);

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
exit:;
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
