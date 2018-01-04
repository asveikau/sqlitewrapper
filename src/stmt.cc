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