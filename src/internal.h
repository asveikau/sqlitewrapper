#ifndef internal_h_
#define internal_h_

#include <sqlitewrapper.h>
#include <sqlite3.h>

namespace sqlite {

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

} // end namespace
#endif