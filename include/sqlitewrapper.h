/*
 Copyright (C) 2017, 2018 Andrew Sveikauskas

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
*/

#ifndef sqlitewrapper_h_
#define sqlitewrapper_h_

// XXX
// This lets us get away with not pulling in the sqlite3.h header
// if we don't want it.
//
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

#include <common/error.h>
#include <stdint.h>
#include <string.h>

#include <string>
#include <vector>

#define SOURCE_SQLITE \
    (error_source)((('s' << 24) | ('q' << 16)  | ('l' << 8) | ('t')))

namespace sqlite {

class statement;
struct error_code_args;

class sqlite
{
   sqlite3 *db;

   void
   check_open(error *err);

   void
   error_set_sqlite_impl(error *err, error_code_args args);

   friend class statement;

public:
   sqlite();
   sqlite(const sqlite& other) = delete;
   ~sqlite();

   void 
   open(const char *filename, int flags, const char *vfs, error *err);

   void 
   open(const char *filename, int flags, error *err)
   {
      open(filename, flags, nullptr, err);
   }

   void
   open(const char *filename, error *err);

   sqlite3 *
   get_handle() const { return db; }

   bool
   is_open(void) const { return db ? true : false; }

   bool
   is_read_only();

   void
   close();

   void
   prepare(
      const char *sql,
      statement &stmt,
      error *err
   );

   // Like prepare, but calling step() once and discarding
   // results.
   //
   void
   exec(const char *sql, error *err);

   // Same pattern, but NULL terminated for multiple statements.
   //
   void
   exec(const char *const *sql_z, error *err);

   // Same as above, but varags...
   //
   void
   exec(error *err, const char *sql0, ...);

   bool
   table_exists(const char *name, error *err);

   bool
   column_exists(const char *table, const char *column, error *err);

   int64_t
   last_insert_rowid();
};

class statement
{
   sqlite3_stmt *stmt;
   sqlite *db;
   friend class sqlite;

   void
   check_open(error *err);

   void
   check_column(int idx, error *err);

   void
   error_set_sqlite_impl(error *err, error_code_args args);

public:
   statement();
   statement(const statement &other) = delete;
   ~statement();

   sqlite3_stmt *
   get_handle() const { return stmt; }

   void close();

   // Returns true if there is column data [i.e. SQLITE_ROW],
   // false otherwise
   //
   bool step(error *err);

   void reset(error *err);

   int
   bind_count();

   //
   // Note that, in the wrapper, bind() uses 0-based indices,
   // whereas sqlite3 itself uses 1-based.
   //

   void
   bind(int idx, error *err);

   void
   bind(int idx, const void *buf, size_t len, error *err);

   void
   bind(int idx, const char *str, error *err);

   void
   bind(int idx, const std::string &str, error *err)
   {
      bind(idx, str.c_str(), err);
   }

   void
   bind(int idx, const char *str, size_t len, error *err);

   void
   bind(int idx, int64_t i, error *err);

   void
   bind(int idx, double d, error *err);

   void
   bind(int idx, uint64_t u, error *err)
   {
      bind(idx, (int64_t)u, err);
   }

   template<typename T, typename... Args>
   void
   bind_multi(error *err, int idx, const T &first, const Args& ... rest)
   {
      bind_multi<T>(err, idx, first);
      if (ERROR_FAILED(err))
         return;
      if (sizeof...(rest) != 0)
         bind_multi(err, idx+1, rest...);
   }

   template<typename T>
   void
   bind_multi(error *err, int idx, const T &first)
   {
      bind(idx, first, err);
   }

   int
   column_count();

   const char *
   column_name(int idx, error *err);

   int
   column_type(int idx, error *err);

   void
   column(int idx, int64_t &i, error *err);

   void
   column(int idx, double &d, error *err);

   void
   column(int idx, const char *&string, size_t &len, error *err);

   void
   column(int idx, std::string &str, error *err);

   void
   column(int idx, const void *&blob, size_t &len, error *err);

   // Assumes T is a POD type...
   //
   template<typename T>
   void
   column(int idx, std::vector<T> &blob, error *err)
   {
      const void *blobp = nullptr;
      size_t len = 0;

      column(idx, blobp, len, err);
      ERROR_CHECK(err);

      len /= sizeof(T);

      try
      {
         blob.resize(len);
      }
      catch (const std::bad_alloc&)
      {
         ERROR_SET(err, nomem);
      }

      if (len)
         memcpy(blob.data(), blobp, len * sizeof(T));
   exit:;
   }

   void
   column(int idx, uint64_t &i, error *err)
   {
      int64_t t = 0;
      column(idx, t, err);
      i = t;
   }

   int64_t
   column_int(int idx, error *err)
   {
      int64_t i = 0;
      column(idx, i, err);
      return i;
   }

   double
   column_double(int idx, error *err)
   {
      double d = 0.0;
      column(idx, d, err);
      return d;
   }

   const char *
   column_cstr(int idx, error *err)
   {
      const char *str = nullptr;
      size_t discard = 0;

      column(idx, str, discard, err);

      return str;
   }

   std::string
   column_str(int idx, error *err)
   {
      std::string s;
      column(idx, s, err);
      return s;
   }

   template<typename T>
   std::vector<T>
   column_blob(int idx, error err)
   {
      std::vector<T> blob;
      column(idx, blob, err);
      return blob;
   }

   template<typename T, typename... Args>
   void
   column_multi(error *err, int idx, T& first, Args&... rest)
   {
      column_multi<T>(err, idx, first);
      if (ERROR_FAILED(err))
         return;
      if (sizeof...(rest) != 0)
         column_multi(err, idx+1, rest...);
   }

   template<typename T>
   void
   column_multi(error *err, int idx, T &first)
   {
      column(idx, first, err);
   }
};

} // end namespace

#endif
