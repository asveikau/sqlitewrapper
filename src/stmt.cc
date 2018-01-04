#include <sqlitewrapper.h>
#include <string.h>

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
sqlite::statement::check_column(int idx, error *err)
{
   // We could check stmt separately, but this method gives a better error
   // message.
   //
   check_open(err);
   ERROR_CHECK(err);

   if (idx < 0 || idx >= sqlite3_column_count(stmt))
      ERROR_SET(err, unknown, "Column index out of bounds");
exit:;
}

bool
sqlite::statement::step(error *err)
{
   int rc = 0;
   bool r = false;

   check_open(err);
   ERROR_CHECK(err);

   rc = sqlite3_step(stmt);
   switch (rc)
   {
   case SQLITE_ROW:
      r = true;
      break;
   case SQLITE_DONE:
      break;
   default:
      ERROR_SET(err, sqlite, error_code_args(stmt, rc));
   }

exit:
   return r;
}

int
sqlite::statement::bind_count()
{
   int r = 0;
   if (stmt)
      r = sqlite3_bind_parameter_count(stmt);
   return r;
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

int
sqlite::statement::column_count()
{
   int r = 0;
   if (stmt)
      r = sqlite3_column_count(stmt);
   return r;
}

const char *
sqlite::statement::column_name(int idx, error *err)
{
   const char *r = nullptr;

   check_column(idx, err);

   r = sqlite3_column_name(stmt, idx);
   if (!r)
      ERROR_SET(err, sqlite, error_code_args(stmt));

exit:
   return r;
}


int
sqlite::statement::column_type(int idx, error *err)
{
   int r = 0;

   check_column(idx, err);
   ERROR_CHECK(err);

   r = sqlite3_column_type(stmt, idx);
exit:
   return r;
}

void
sqlite::statement::column(int idx, int64_t &i, error *err)
{
   check_column(idx, err);
   ERROR_CHECK(err);

   i = sqlite3_column_int64(stmt, idx);
exit:;
}

void
sqlite::statement::column(int idx, double &d, error *err)
{
   check_column(idx, err);
   ERROR_CHECK(err);

   d = sqlite3_column_double(stmt, idx);
exit:;
}

void
sqlite::statement::column(int idx, const char *&string, size_t &len, error *err)
{
   const unsigned char *r = nullptr;
   int n = 0;

   check_column(idx, err);
   ERROR_CHECK(err);

   if (sqlite3_column_type(stmt, idx) == SQLITE_NULL)
   {
      string = nullptr;
      len = 0;
      goto exit;
   }

   //
   // Per sqlite documentation, the right way to do this is to get the text
   // pointer first, then ask about length.
   //

   r = sqlite3_column_text(stmt, idx);
   if (!r)
      ERROR_SET(err, sqlite, error_code_args(stmt));
   string = (const char*)r;
   n = sqlite3_column_bytes(stmt, idx);
exit:;
}

void
sqlite::statement::column(int idx, const void *&blob, size_t &len, error *err)
{
   check_column(idx, err);
   ERROR_CHECK(err);

   if (sqlite3_column_type(stmt, idx) == SQLITE_NULL)
   {
      blob = nullptr;
      len = 0;
      goto exit;
   }

   blob = sqlite3_column_blob(stmt, idx);
   if (!blob)
      ERROR_SET(err, sqlite, error_code_args(stmt));
   len = sqlite3_column_bytes(stmt, idx);

exit:;
}

void
sqlite::statement::column(int idx, std::string &str, error *err)
{
   try
   {
      const char *cstr = nullptr;
      size_t len = 0;

      column(idx, cstr, len, err);
      ERROR_CHECK(err);

      str.resize(0);
      str.append(cstr, len);
   }
   catch (std::bad_alloc)
   {
      ERROR_SET(err, errno, ENOMEM);
   }
exit:;
}