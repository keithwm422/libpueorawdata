#include "pueo/rawdata.h"
#include "pueo/rawio.h"
#include <stdbool.h>


int main(int nargs, char ** args)
{
  pueo_handle_t h;
  printf("}1\n");
  pueo_handle_init(&h, nargs > 1 ? args[1] : "test-sensors.sns","r");
  printf("}2\n");
  pueo_db_handle_t * db = NULL;
  printf("}3\n");
  if (nargs > 2) db = pueo_db_handle_open(args[2], 0);
  pueo_sensors_telem_t t = {0};
  printf("}4\n");
  while (true)
  {
    printf("}5\n");
    int read = pueo_read_sensors_telem(&h, &t);
    printf("}6\n");
    printf("read: %d\n", read);
    if (read <= 0) break;
    if (db) pueo_db_insert_sensors_telem(db, &t);
    pueo_dump_sensors_telem(stdout,&t);
  }
  pueo_handle_close(&h);
  if (db) pueo_db_handle_close(&db);
  printf("}\n");

  return 0;
}

