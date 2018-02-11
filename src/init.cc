#include "internal.h"
#include <common/lazy.h>
#include <common/logger.h>

void
lazy_init(
  lazy_init_state *state,
  void (*fn)(void *context, error *err),
  void *context,
  error *err
);

void
sqlite::init_library(error *err)
{
   static lazy_init_state state = {0};
   lazy_init(
      &state,
      [] (void *context, error *err) -> void
      {
         int rc;

         void (*logfn)(void *, int, const char*) =
            [] (void *context, int err, const char *msg) -> void
            {
               log_printf("sqlite: %.8x %s", err, msg);               
            };

         sqlite3_config(SQLITE_CONFIG_LOG, logfn, context);

         rc = sqlite3_initialize();
         if (rc)
            ERROR_SET(err, sqlite, rc);

      exit:;
      },
      nullptr,
      err
   );
   ERROR_CHECK(err);
exit:;
}