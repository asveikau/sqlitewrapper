#ifndef sqlitewrapper_h_
#define sqlitewrapper_h_

#include <sqlite3.h>
#include <common/error.h>

namespace sqlite {

class statement;

class sqlite
{
   sqlite3 *db;
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
   prepare(
      const char *sqlite,
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

   bool step(error *err);
};

} // end namespace

#endif
