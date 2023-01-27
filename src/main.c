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
  nb_sensor_start_get_setup_thread();

  nb_sensor_setup_param channel1;
  nb_sensor_setup_param channel2;
  nb_sensor_setup_param channel3;
  nb_sensor_setup_param channel4;
  nb_sensor_setup_param channel5;

  // sensor_s main_sensor;
  time_t prev_tm_chan1, prev_tm_chan2, prev_tm_chan3, prev_tm_chan4, prev_tm_chan5, pres_tm = 0;
  time(&pres_tm);
  prev_tm_chan1 = pres_tm;
  prev_tm_chan2 = pres_tm;
  prev_tm_chan3 = pres_tm;
  prev_tm_chan4 = pres_tm;
  prev_tm_chan5 = pres_tm;

  while (1)
  {
    time(&pres_tm);
    memset(&channel1, 0x00, sizeof(nb_sensor_setup_param));
    nb_sensor_get_setup_channel(CHANNEL1, &channel1);
    if (channel1.state && pres_tm - prev_tm_chan1 >= channel1.interval)
    {
      prev_tm_chan1 = pres_tm;
      debug(__func__, "INFO", "Get data channel 1");
      debug(__func__, "INFO", "Nama Channel %s, Satuan %s, dan interval %i", channel1.name,
            channel1.satuan, channel1.interval);
    }
    memset(&channel2, 0x00, sizeof(nb_sensor_setup_param));
    nb_sensor_get_setup_channel(CHANNEL2, &channel2);
    if (channel2.state && pres_tm - prev_tm_chan2 >= channel2.interval)
    {
      prev_tm_chan2 = pres_tm;
      debug(__func__, "INFO", "Get data channel 2");
      debug(__func__, "INFO", "Nama Channel %s, Satuan %s, dan interval %i", channel2.name,
            channel2.satuan, channel2.interval);
    }
    memset(&channel3, 0x00, sizeof(nb_sensor_setup_param));
    nb_sensor_get_setup_channel(CHANNEL3, &channel3);
    if (channel3.state && pres_tm - prev_tm_chan3 >= channel3.interval)
    {
      prev_tm_chan3 = pres_tm;
      debug(__func__, "INFO", "Get data channel 3");
      debug(__func__, "INFO", "Nama Channel %s, Satuan %s, dan interval %i", channel3.name,
            channel3.satuan, channel3.interval);
    }
    memset(&channel4, 0x00, sizeof(nb_sensor_setup_param));
    nb_sensor_get_setup_channel(CHANNEL4, &channel4);
    if (channel4.state && pres_tm - prev_tm_chan4 >= channel4.interval)
    {
      prev_tm_chan4 = pres_tm;
      debug(__func__, "INFO", "Get data channel 4");
      debug(__func__, "INFO", "Nama Channel %s, Satuan %s, dan interval %i", channel4.name,
            channel4.satuan, channel4.interval);
    }
    memset(&channel5, 0x00, sizeof(nb_sensor_setup_param));
    nb_sensor_get_setup_channel(CHANNEL5, &channel5);
    if (channel5.state && pres_tm - prev_tm_chan5 >= channel5.interval)
    {
      prev_tm_chan5 = pres_tm;
      debug(__func__, "INFO", "Get data channel 5");
      debug(__func__, "INFO", "Nama Channel %s, Satuan %s, dan interval %i", channel5.name,
            channel5.satuan, channel5.interval);
    }
    usleep(10000);
    // time(&pres_tm);
    // if (pres_tm - prev_tm >= 5)
    // {
    //   prev_tm = pres_tm;
    //   memset(&main_sensor, 0x00, sizeof(main_sensor));
    //   int ret = nb_sensor_get_temp_hum_md02(main_sensor.temperature, main_sensor.humidity);
    //   if (!ret)
    //   {
    //     nb_calib_save_data(main_sensor.temperature, "channel1");
    //     nb_calib_save_data(main_sensor.humidity, "channel2");
    //   }
    // }
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
