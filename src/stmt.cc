#include <sqlitewrapper.h>

sqlite::statement::statement()
   : stmt(nullptr)
{
}

sqlite::statement::~statement()
{
   if (stmt)
      sqlite3_finalize(stmt);
}
