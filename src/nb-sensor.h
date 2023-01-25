#ifndef NB_SENSOR_H
#define NB_SENSOR_H

#include <stdbool.h>

typedef struct
{
  int interval;
  bool state;
  char name[50];
  char satuan[20];
} nb_sensor_setup_param;

typedef enum
{
  CHANNEL1,
  CHANNEL2,
  CHANNEL3,
  CHANNEL4,
  CHANNEL5
} nb_chan_type;

int nb_sensor_get_temp_hum_md02(char *temp_value, char *hum_value);
double nb_sensor_parsing_temp_md02(unsigned char *t_reg_recv);
double nb_sensor_parsing_hum_md02(unsigned char *t_reg_recv);
int8_t nb_sensor_start_get_setup_thread();
int8_t nb_sensor_get_setup_channel(nb_chan_type t_type, nb_sensor_setup_param *t_param);

#endif