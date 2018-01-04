#ifndef sqlitewrapper_h_
#define sqlitewrapper_h_

#include <sqlite3.h>
#include <common/error.h>
#include <stdint.h>

#include <string>

namespace sqlite {

class statement;

struct
error_code_args
{
   sqlite3 *db;
   int rc;

   error_code_args(int rc_) : db(nullptr), rc(rc_) {}

   error_code_args(sqlite3 *db_) : db(db_), rc(SQLITE_ERROR) {get_db_rc();}

   error_code_args(sqlite3 *db_, int rc_) : db(db_), rc(rc_) {}

   error_code_args(sqlite3_stmt *stmt_, int rc_) : db(sqlite3_db_handle(stmt_)), rc(rc_) {}

   error_code_args(sqlite3_stmt *stmt_) : db(sqlite3_db_handle(stmt_)), rc(SQLITE_ERROR) {get_db_rc();}

private:
   void
   get_db_rc()
   {
      int rc2;
      if (db && (rc2 = sqlite3_errcode(db)))
         rc = rc2;
   }
};

void
error_set_sqlite(error *err, error_code_args args);

void
error_set_sqlite(error *err, int rc);

class sqlite
{
   sqlite3 *db;

   void
   check_open(error *err);

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
   open(const char *filename, error *err)
   {
      open(filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, err);
   }

   void
   close();

   void
   prepare(
      const char *sql,
      statement &stmt,
      error *err
   );
};

class statement
{
   sqlite3_stmt *stmt;
   friend class sqlite;

   void
   check_open(error *err);

   void
   check_column(int idx, error *err);

public:
   statement();
   statement(const statement &other) = delete;
   ~statement();

   void close();

   bool step(error *err);

   int
   bind_count();

   void
   bind(int idx, error *err);

   void
   bind(int idx, const void *buf, size_t len, error *err);

   void
   bind(int idx, const char *str, error *err);

   void
   bind(int idx, const char *str, size_t len, error *err);

   void
   bind(int idx, int64_t i, error *err);

   void
   bind(int idx, double d, error *err);

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
};

} // end namespace

#endif
