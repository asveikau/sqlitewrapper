#ifndef sqlitewrapper_h_
#define sqlitewrapper_h_

#include <sqlite3.h>
#include <common/error.h>

namespace sqlite {

class statement;

struct
error_code_args
{
   sqlite3 *db;
   int rc;

   error_code_args(sqlite3 *db_, int rc_) : db(db_), rc(rc_) {}
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
public:
   statement();
   statement(const statement &other) = delete;
   ~statement();

   void close();

   bool step(error *err);
};

} // end namespace

#endif
