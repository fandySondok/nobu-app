#ifndef NB_CALIBRATION_H
#define NB_CALIBRATION_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  double aval;
  double bval;
} calib_value;

typedef struct
{
  calib_value temp;
  calib_value hum;
  calib_value co2;
} calib_param;

int8_t nb_calib_save_data(char *msg_tmp, char *topic_tmp);

#endif