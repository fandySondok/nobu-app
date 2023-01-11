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
  calib_value channel1;
  calib_value channel2;
  calib_value channel3;
  calib_value channel4;
  calib_value channel5;
} calib_param;

int8_t nb_calib_save_data(char *msg_tmp, char *topic_tmp);

#endif