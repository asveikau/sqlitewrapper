#include <sqlitewrapper.h>
#include <common/misc.h>
#include <string.h>

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

   close();

   rc = sqlite3_open_v2(filename, &db, flags, vfs);
   if (rc)
      ERROR_SET(err, sqlite, rc);
exit:;
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
      ERROR_SET(err, sqlite, error_code_args(db, rc));

   if (tail && *tail)
      ERROR_SET(err, unknown, "Unused junk at the end of sql");

exit:;
}