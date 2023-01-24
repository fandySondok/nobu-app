#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "nb-conf.h"
#include "nb-psql.h"
#include "nb-mosquitto.h"
#include "nb-sensor.h"

#define LOG_DEVICE "data.db"
#define MAX_SQL_BUFF 2024

typedef struct
{
  char temperature[10];
  char humidity[10];
} sensor_s;

int main()
{
  debug(__func__, "INFO", "----------------- [ Starting Program ] -----------------");
  conf_core_init();
  // nb_psql_init(); // commented for testing on pc purpose

  sensor_s main_sensor;
  time_t prev_tm, pres_tm = 0;
  time(&pres_tm);
  prev_tm = pres_tm;

  while (1)
  {
    time(&pres_tm);
    if (pres_tm - prev_tm >= 5)
    {
      prev_tm = pres_tm;
      memset(&main_sensor, 0x00, sizeof(main_sensor));
      int ret = nb_sensor_get_temp_hum_md02(main_sensor.temperature, main_sensor.humidity);
      if (!ret)
      {
        nb_calib_save_data(main_sensor.temperature, "channel1");
        nb_calib_save_data(main_sensor.humidity, "channel2");
      }
    }
    usleep(10000);
  }

  return 0;
}

// int s_log_insert_log_device(char *topic_tmp, char *message_tmp)
// {
//   sqlite3 *db;
//   char *err_msg = 0;

//   if (sqlite3_open(LOG_DEVICE, &db) != SQLITE_OK)
//   {
//     printf("can't open database: %s\n", sqlite3_errmsg(db));
//     sqlite3_close(db);
//     return -1;
//   }

//   char *sql = NULL;
//   sql = (char *)malloc(MAX_SQL_BUFF * sizeof(char));
//   if (sql == NULL)
//   {
//     printf("failed to allocate memmory\n");
//     sqlite3_close(db);
//     return -2;
//   }

//   memset(sql, 0x00, MAX_SQL_BUFF * sizeof(char));
//   sprintf(sql, "INSERT INTO data("
//                "log_time,"
//                "topic,"
//                "message"
//                ") VALUES ("
//                "strftime(\"%%Y-%%m-%%d %%H:%%M:%%f\",\"now\",\"localtime\"), '%s', '%s'"
//                ");",
//           topic_tmp, message_tmp);

// tryagain1:;
//   if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK)
//   {
//     printf("can't insert data to data.db: %s\n", err_msg);
//     if (strcmp("database is locked", err_msg) == 0)
//     {
//       sqlite3_free(err_msg);
//       usleep(100000);
//       goto tryagain1;
//     }
//     if (sql)
//     {
//       free(sql);
//       sql = NULL;
//     }
//     sqlite3_free(err_msg);
//     sqlite3_close(db);
//     return -1;
//   }

//   if (sql)
//   {
//     free(sql);
//     sql = NULL;
//   }
//   sqlite3_close(db);

//   return 0;
// }
