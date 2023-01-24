#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nb-mosquitto.h"
#include "nb-conf.h"
#include "nb-psql.h"

#define CLIENT_ID "nb-device"

extern struct core_conf_info mq_conf;
extern struct core_topic_info mq_sub;

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
  debug(__func__, "INFO", "ID: %d", *(int *)obj);
  if (rc)
  {
    debug(__func__, "ERROR", "Error with result code %d", rc);
    exit(-1);
  }
  if (mq_conf.total_topic > 0)
  {
    for (int i = 1; i <= mq_conf.total_topic; i++)
    {
      mosquitto_subscribe(mosq, NULL, mq_sub.topic1 + ((i - 1) * 50), 0);
    }
  }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
  debug(__func__, "INFO", "New Message with topic %s : %s", msg->topic, (char *)msg->payload);
  nb_calib_save_data((char *)msg->payload, msg->topic);

  // float value = atof((char *)msg->payload);
  // if (value > 50.0 && !memcmp(msg->topic, "/area1/temp1", strlen("/area1/temp1"))) // todo: nanti akan dibisa diconfig
  // {
  //   system("node ../notif_gmail/app.js");
  // }
}

int nb_mosquitto_init(struct mosquitto *mosq_tmp)
{
  debug(__func__, "INFO", "Init Mosquitto Protocol");
  int rc, id = 12;
  mosquitto_lib_init();
  mosq_tmp = mosquitto_new(CLIENT_ID, true, &id);
  mosquitto_connect_callback_set(mosq_tmp, on_connect);
  mosquitto_message_callback_set(mosq_tmp, on_message);
  mosquitto_username_pw_set(mosq_tmp, mq_conf.username, mq_conf.passwd);
  rc = mosquitto_connect(mosq_tmp, mq_conf.ip_addr, mq_conf.ip_port, 10);
  if (rc)
  {
    debug(__func__, "ERROR", "Could not connect to broker with return code %d", rc);
    return -1;
  }

  mosquitto_loop_start(mosq_tmp);
  return 0;
}

int nb_mosquitto_stop(struct mosquitto *mosq_tmp)
{
  mosquitto_loop_stop(mosq_tmp, true);
  mosquitto_disconnect(mosq_tmp);
  mosquitto_destroy(mosq_tmp);
  mosquitto_lib_cleanup();
  return 0;
}
