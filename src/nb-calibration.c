#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "nb-calibration.h"
#include "nb-conf.h"
#include "nb-psql.h"

pthread_mutex_t lock_calib_save = PTHREAD_MUTEX_INITIALIZER;

calib_param dev_sensor;

int8_t nb_calib_save_data(char *msg_tmp, char *topic_tmp)
{
  pthread_mutex_lock(&lock_calib_save);
  double msg_aft_calib_db = 0;
  char msg_aft_calib[10];
  memset(msg_aft_calib, 0x00, sizeof(msg_aft_calib));
  if (memcmp("channel1", topic_tmp, strlen(topic_tmp)) == 0) // temp
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.channel1);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.channel1.aval + dev_sensor.channel1.bval;
  }
  else if (memcmp("channel2", topic_tmp, strlen(topic_tmp)) == 0) // hum
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.channel2);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.channel2.aval + dev_sensor.channel2.bval;
  }
  else if (memcmp("channel3", topic_tmp, strlen(topic_tmp)) == 0) // co2
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.channel3);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.channel3.aval + dev_sensor.channel3.bval;
  }
  else if (memcmp("channel4", topic_tmp, strlen(topic_tmp)) == 0)
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.channel4);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.channel4.aval + dev_sensor.channel4.bval;
  }
  else if (memcmp("channel5", topic_tmp, strlen(topic_tmp)) == 0)
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.channel5);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.channel5.aval + dev_sensor.channel5.bval;
  }
  snprintf(msg_aft_calib, sizeof(msg_aft_calib), "%.2f", msg_aft_calib_db);
  nb_psql_send_logfile(topic_tmp, msg_aft_calib);
  pthread_mutex_unlock(&lock_calib_save);
  return 0;
}