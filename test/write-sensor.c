#include "pueo/rawdata.h"
#include "pueo/rawio.h"


int main(int nargs, char ** args)
{

  pueo_sensors_telem_t sens =  {.timeref_secs = 123, .sensor_id_magic = 0x2525, .num_packets = 238};

  for (int i = 0; i < 238;i++){
    sens.sensors[i].sensor_id = (uint16_t) (i);
    sens.sensors[i].relsecs = (int16_t)(i % 60);
    sens.sensors[i].val.fval = (_Float16)(i);
  }
  pueo_handle_t h;
  pueo_handle_init(&h, nargs > 1 ? args[1] : "test-sensors.sns","w");
  pueo_write_sensors_telem(&h, &sens);
  pueo_handle_close(&h);
  printf("{");
  pueo_dump_sensors_telem(stdout, &sens);
  printf("}\n");
  return 0;
}

