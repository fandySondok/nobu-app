#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nb-calibration.h"
#include "conf.h"
#include "nb-psql.h"

calib_param dev_sensor;
extern struct core_topic_info mq_sub;

int8_t nb_calib_save_data(char *msg_tmp, char *topic_tmp)
{
  double msg_aft_calib_db = 0;
  char msg_aft_calib[10];
  memset(msg_aft_calib, 0x00, sizeof(msg_aft_calib));
  if (memcmp(mq_sub.topic1, topic_tmp, strlen(topic_tmp)) == 0) // temp
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.temp);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.temp.aval + dev_sensor.temp.bval;
  }
  else if (memcmp(mq_sub.topic2, topic_tmp, strlen(topic_tmp)) == 0) // hum
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.hum);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.hum.aval + dev_sensor.hum.bval;
  }
  else if (memcmp(mq_sub.topic3, topic_tmp, strlen(topic_tmp)) == 0) // co2
  {
    nb_psql_get_calib_value(topic_tmp, &dev_sensor.co2);
    msg_aft_calib_db = (atof(msg_tmp)) * dev_sensor.co2.aval + dev_sensor.co2.bval;
  }
  snprintf(msg_aft_calib, sizeof(msg_aft_calib), "%.2f", msg_aft_calib_db);
  nb_psql_send_logfile(topic_tmp, msg_aft_calib);
  return 0;
}