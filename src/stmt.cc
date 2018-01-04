#include <sqlitewrapper.h>

sqlite::statement::statement()
   : stmt(nullptr)
{
}

sqlite::statement::~statement()
{
   close();
}

void
sqlite::statement::close()
{
   if (stmt)
   {
      sqlite3_finalize(stmt);
      stmt = nullptr;
   }
}

void
sqlite::statement::check_open(error *err)
{
   if (!stmt)
      ERROR_SET(err, unknown, "Method called on null statement object");
exit:;
}

void
sqlite::statement::bind(int idx, error *err)
{
   int rc;

   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_null(stmt, idx+1)))
      ERROR_SET(err, sqlite, error_code_args(stmt, rc));
exit:;
}

void
sqlite::statement::bind(int idx, const void *buf, size_t len, error *err)
{
   int rc;

   if (!buf)
   {
      bind(idx, err);
      goto exit;
   }

   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_blob64(stmt, idx+1, buf, len, nullptr)))
      ERROR_SET(err, sqlite, error_code_args(stmt, rc));
exit:;
}

void
sqlite::statement::bind(int idx, const char *str, error *err)
{
   if (!str)
   {
      bind(idx, err);
   }
   else
   {
      bind(idx, str, strlen(str), err);
   }
}

void
sqlite::statement::bind(int idx, const char *str, size_t len, error *err)
{
   int rc;

   if (!str)
   {
      bind(idx, err);
      goto exit;
   }

   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_text(stmt, idx+1, str, len, nullptr)))
      ERROR_SET(err, sqlite, error_code_args(stmt, rc));
exit:;
}

void
sqlite::statement::bind(int idx, int64_t i, error *err)
{
   int rc;

   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_int64(stmt, idx+1, i)))
      ERROR_SET(err, sqlite, error_code_args(stmt, rc));
exit:;
}

void
sqlite::statement::bind(int idx, double d, error *err)
{
   int rc;
   
   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_double(stmt, idx+1, d)))
      ERROR_SET(err, sqlite, error_code_args(stmt, rc));
exit:;
}