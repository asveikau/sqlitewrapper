/*
 Copyright (C) 2017, 2018 Andrew Sveikauskas

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
*/

#include <string.h>

#include "internal.h"

sqlite::statement::statement()
   : stmt(nullptr), db(nullptr)
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

   db = nullptr;
}

void
sqlite::statement::error_set_sqlite_impl(error *err, error_code_args args)
{
   if (db)
      db->error_set_sqlite_impl(err, args);
   else
      ::sqlite::error_set_sqlite_impl(err, args);
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
      ERROR_SET(err, sqlite, stmt, rc);
   }

exit:
   return r;
}

void
sqlite::statement::reset(error *err)
{
   int rc = 0;

   check_open(err);
   ERROR_CHECK(err);

   rc = sqlite3_reset(stmt);
   if (rc)
      ERROR_SET(err, sqlite, stmt, rc);
exit:;
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
      ERROR_SET(err, sqlite, stmt, rc);
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
      ERROR_SET(err, sqlite, stmt, rc);
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
      ERROR_SET(err, sqlite, stmt, rc);
exit:;
}

void
sqlite::statement::bind(int idx, int64_t i, error *err)
{
   int rc;

   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_int64(stmt, idx+1, i)))
      ERROR_SET(err, sqlite, stmt, rc);
exit:;
}

void
sqlite::statement::bind(int idx, double d, error *err)
{
   int rc;
   
   check_open(err);
   ERROR_CHECK(err);

   if ((rc = sqlite3_bind_double(stmt, idx+1, d)))
      ERROR_SET(err, sqlite, stmt, rc);
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
      ERROR_SET(err, sqlite, stmt);

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
      ERROR_SET(err, sqlite, stmt);
   string = (const char*)r;
   len = sqlite3_column_bytes(stmt, idx);
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
      ERROR_SET(err, sqlite, stmt);
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
      ERROR_SET(err, nomem);
   }
exit:;
}