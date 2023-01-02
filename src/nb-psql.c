#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nb-psql.h"
#include "conf.h"

struct nb_psql_parameters nb_psql;

pthread_mutex_t lock_conn = PTHREAD_MUTEX_INITIALIZER;

int8_t nb_psql_connect(PGconn **conn)
{
  pthread_mutex_lock(&lock_conn);
  char *info;
  int status;
  info = (char *)malloc(256 * sizeof(char));
  if (info == NULL)
  {
    printf("failed to allocate memory\n");
    pthread_mutex_unlock(&lock_conn);
    return -1;
  }
  memset(info, 0x00, 256 * sizeof(char));

  sprintf(info, "hostaddr=%s port=%i dbname=%s user=%s password=%s connect_timeout=%i",
          nb_psql.address, nb_psql.port, nb_psql.db, nb_psql.username, nb_psql.password, nb_psql.timeout);

  *conn = PQconnectdb(info);
  status = PQstatus(*conn);
  free(info);
  info = NULL;
  if (status != CONNECTION_OK)
  {
    printf("%s\n", PQerrorMessage(*conn));
    pthread_mutex_unlock(&lock_conn);
    return -1;
  }
  pthread_mutex_unlock(&lock_conn);
  return 0;
}

void nb_psql_close(PGconn **conn)
{
  if (conn)
  {
    PQfinish(*conn);
    *conn = NULL;
  }
}

int8_t nb_psql_send_logfile(const char *top_tmp, const char *msg_tmp)
{
  PGconn *conn = NULL;
  if (nb_psql_connect(&conn) != 0)
  {
    debug(__func__, "ERROR", "psql send aborted!");
    return -1;
  }

  char *cmd;
  int status;
  float value_msg = 0;
  cmd = (char *)malloc(S_PSQL_MAX_CMD * sizeof(char));
  if (cmd == NULL)
  {
    debug(__func__, "ERROR", "Failed to allocate memory");
    return -2;
  }
  value_msg = atof(msg_tmp);
  if (value_msg <= 0)
  {
    debug(__func__, "WARNING", "Value not numeric type");
    return -3;
  }

  sprintf(cmd, "insert into mqdata (topic, message) values('%s', %.2f);", top_tmp, value_msg);
  PGresult *result;
  result = PQexec(conn, cmd);
  status = PQresultStatus(result);
  free(cmd);
  cmd = NULL;
  if (status != PGRES_COMMAND_OK)
  {
    if (strstr(PQresultErrorMessage(result), "duplicate key value") == NULL)
    {
      debug(__func__, "ERROR", "SQL ERROR -> [%d]", status);
      printf("%s\n", PQresultErrorMessage(result));
      if (result)
      {
        PQclear(result);
        result = NULL;
      }
      nb_psql_close(&conn);
      return -1;
    }
  }
  if (result)
  {
    PQclear(result);
    result = NULL;
  }
  nb_psql_close(&conn);
  return 0;
}

int8_t nb_psql_init()
{
  memcpy(nb_psql.username, "nobutech", strlen("nobutech"));
  memcpy(nb_psql.password, "nobutech123", strlen("nobutech123"));
  memcpy(nb_psql.db, "dbnobutech", strlen("dbnobutech"));
  memcpy(nb_psql.address, "127.0.0.1", strlen("127.0.0.1"));
  nb_psql.port = 5432;
  nb_psql.timeout = 3;
  return 0;
}

int8_t nb_psql_get_calib_value(char *top_tmp, calib_value *dev_sen_tmp)
{
  PGconn *conn = NULL;
  if (nb_psql_connect(&conn) != 0)
  {
    debug(__func__, "ERROR", "psql send aborted!");
    return -1;
  }

  char *cmd;
  int status;
  cmd = (char *)malloc(S_PSQL_MAX_CMD * sizeof(char));
  if (cmd == NULL)
  {
    debug(__func__, "ERROR", "Failed to allocate memory");
    return -2;
  }

  sprintf(cmd, "select aval, bval from calib where topic = '%s' order by instime desc;", top_tmp);
  PGresult *result;
  result = PQexec(conn, cmd);
  status = PQresultStatus(result);
  free(cmd);
  cmd = NULL;

  if (status == PGRES_TUPLES_OK)
  {
    int8_t rows = (int8_t)PQntuples(result);
    if (rows == 0)
    {
      debug(__func__, "ERROR", "Calibration value not found\n");
      if (result)
      {
        PQclear(result);
        result = NULL;
      }
      nb_psql_close(&conn);
      return -3;
    }
    else
    {
      char aval_tmp[8];
      char bval_tmp[8];
      memset(aval_tmp, 0x00, sizeof(aval_tmp));
      memset(bval_tmp, 0x00, sizeof(bval_tmp));

      strcpy(aval_tmp, PQgetvalue(result, 0, 0));
      strcpy(bval_tmp, PQgetvalue(result, 0, 1));

      dev_sen_tmp->aval = atof(aval_tmp);
      dev_sen_tmp->bval = atof(bval_tmp);

      if (result)
      {
        PQclear(result);
        result = NULL;
      }
      nb_psql_close(&conn);
      return 0;
    }
  }
  else
  {
    debug(__func__, "WARNING", "SQL ERROR -> [%d] \n", status);
    printf("%s\n", PQresultErrorMessage(result));
    if (result)
    {
      PQclear(result);
      result = NULL;
    }
    nb_psql_close(&conn);
    return -4;
  }
  return 0;
}