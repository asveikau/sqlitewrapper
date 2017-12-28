#include <sqlitewrapper.h>

sqlite::sqlite::sqlite()
  : db(nullptr)
{
}

sqlite::sqlite::~sqlite()
{
   if (db)
      sqlite3_close(db);
}

