#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "nb-sensor.h"
#include "nb-serial.h"
#include "conf.h"

char reg_get_temp_md02[6] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x01};
char reg_get_hum_md02[6] = {0x01, 0x04, 0x00, 0x02, 0x00, 0x01};
static unsigned char reg_get_temp_hum_md02[6] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x02};

static long convert_hex_to_long(unsigned char *t_hex_source, int t_hex_size);
extern struct core_conf_info mq_conf;

int nb_sensor_get_temp_hum_md02(char *temp_value, char *hum_value)
{
  debug(__func__, "INFO", "Get Temp and Hum data");
  unsigned char *resp = NULL;

  double temp_value_double = 0;
  double hum_value_double = 0;

  int8_t tryop = 0;
  nb_serial_config_s conf_serial;
  conf_serial.port = mq_conf.port_channel1;
  conf_serial.baudrate = 9600;
  conf_serial.fdesc = -1;

tryop:;
  if (nb_serial_open(&conf_serial) != 0)
  {
    usleep(10000);
    tryop++;
    if (tryop >= 5)
      return -1;
    goto tryop;
  }

  int ret = nb_serial_write(&conf_serial, reg_get_temp_hum_md02, 6);

  if (ret > 0)
    debug(__func__, "INFO", "success sent %d byte data", ret);

  int resp_len = 0;
  resp = nb_serial_read(&conf_serial, &resp_len);
  nb_serial_close(&conf_serial);

  if (resp)
  {
    temp_value_double = nb_sensor_parsing_temp_md02(resp);
    hum_value_double = nb_sensor_parsing_hum_md02(resp);
    free(resp);
    resp = NULL;
    sprintf(temp_value, "%f", temp_value_double);
    sprintf(hum_value, "%f", hum_value_double);
    return 0;
  }
  return 1;
}

double nb_sensor_parsing_temp_md02(unsigned char *t_reg_recv)
{
  double t_temp = 0;
  unsigned char t_hex_temp[2] = {0, 0};
  memcpy(t_hex_temp, t_reg_recv + 4, 2);
  t_temp = convert_hex_to_long(t_hex_temp, 2);
  t_temp = t_temp / 10;
  printf("temperature = %f\n", t_temp);
  return t_temp;
}

double nb_sensor_parsing_hum_md02(unsigned char *t_reg_recv)
{
  double t_hum = 0;
  unsigned char t_hex_hum[2] = {0, 0};
  memcpy(t_hex_hum, t_reg_recv + 6, 2);
  t_hum = convert_hex_to_long(t_hex_hum, 2);
  t_hum = t_hum / 10;
  printf("temperature = %f\n", t_hum);
  return t_hum;
}

static long convert_hex_to_long(unsigned char *t_hex_source, int t_hex_size)
{
  unsigned char hex_tmp[t_hex_size + 1];
  memset(hex_tmp, 0x00, (t_hex_size + 1) * sizeof(char));
  uint8_t i = 0;
  for (i = 0; i < t_hex_size; i += 2)
  {
    hex_tmp[i] = t_hex_source[t_hex_size - 2 - i];
    hex_tmp[i + 1] = t_hex_source[t_hex_size - 1 - i];
  }
  long long dec = 0;
  for (i = 0; i < t_hex_size; i++)
  {
    uint32_t pkt = 1;
    uint8_t j = 0;
    for (j = (t_hex_size - 1) - i; j > 0; j--)
      pkt = pkt * 16;
    if (hex_tmp[i] <= '9' && hex_tmp[i] >= '0')
      dec = dec + (hex_tmp[i] - '0') * pkt;
    else if (hex_tmp[i] <= 'F' && hex_tmp[i] >= 'A')
      dec = dec + (hex_tmp[i] - 'A' + 10) * pkt;
    else if (hex_tmp[i] <= 'f' && hex_tmp[i] >= 'a')
      dec = dec + (hex_tmp[i] - 'a' + 10) * pkt;
  }
  return dec;
}
