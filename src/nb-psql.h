#ifndef NB_PSQL_H
#define NB_PSQL_H
#include <stdint.h>
#include "/usr/include/postgresql/libpq-fe.h"

#include "nb-calibration.h"

#define S_PSQL_MAX_CONF_BUFF 25
#define S_PSQL_MAX_CMD 1024

struct nb_psql_parameters
{
  char address[S_PSQL_MAX_CONF_BUFF];
  char db[S_PSQL_MAX_CONF_BUFF];
  char username[S_PSQL_MAX_CONF_BUFF];
  char password[S_PSQL_MAX_CONF_BUFF];
  uint16_t port;
  int8_t timeout;
};

int8_t nb_psql_init();
int8_t nb_psql_send_logfile(const char *top_tmp, const char *msg_tmp);
int8_t nb_psql_get_calib_value(char *top_tmp, calib_value *dev_sen_tmp);

#endif