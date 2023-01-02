#ifndef NB_MOSQUITTO_H
#define NB_MOSQUITTO_H

#include <mosquitto.h>

int nb_mosquitto_init(struct mosquitto *mosq_tmp);
int nb_mosquitto_stop(struct mosquitto *mosq_tmp);

#endif