#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>

#include "nb-serial.h"
#include "uart-tools.h"
#include "conf.h"

static unsigned char STOP_BYTES[2] = {0x0D, 0x0A};

typedef struct
{
  fd_set fds;
  struct timeval tv;
  int check;
  int state;
  int err;
} nb_serial_timeout_s;

int nb_serial_open(nb_serial_config_s *t_config)
{
  char port[25] = {0};
  snprintf(port, "/dev/ttyUSB%d", t_config->port);
  t_config->fdesc = suart_open(port, t_config->baudrate, 1, 2);
  if (t_config->fdesc > 0)
  {
    debug(__func__, "INFO", "open %s success at (%d)", port, t_config->fdesc);
    return 0;
  }
  return -1;
}

void nb_serial_close(nb_serial_config_s *t_config)
{
  suart_close(t_config->fdesc);
  t_config->fdesc = -1;
}

int nb_serial_timeout(nb_serial_config_s *t_config, nb_serial_timeout_s *t_data)
{
  int ret = 0;
  if (t_data->state == 0)
  {
    t_data->state = 1;
    t_data->check = 0;
    t_data->tv.tv_sec = 5;
    t_data->tv.tv_usec = 0;
    FD_ZERO(&t_data->fds);
    FD_SET(t_config->fdesc, &t_data->fds);
  }
  else
  {
    ret = select(t_config->fdesc + 1, &t_data->fds, NULL, NULL, &t_data->tv);
    t_data->err = errno;
  }
  return ret;
}

unsigned char *nb_serial_read(nb_serial_config_s *t_config, int *t_size)
{
  unsigned char *ret;
  unsigned char *recv;

  int max_recv = 1024;
  ret = (unsigned char *)calloc(1024, sizeof(unsigned char));
  if (ret == NULL)
    return NULL;
  int cnt = 0;
  int cnt_0 = 0;

#define SELECT_AKTIF
#ifdef SELECT_AKTIF
  nb_serial_timeout_s tm_out;
  memset(&tm_out, 0, sizeof(nb_serial_timeout_s));
  nb_serial_timeout(t_config, &tm_out);
  int ret_select = 0;
#endif

  debug(__func__, "INFO", "SERIAL READ");
  while (1)
  {
#ifdef SELECT_AKTIF
    if (cnt == 0)
    {
      ret_select = nb_serial_timeout(t_config, &tm_out);
      if (!ret_select)
      {
        cnt = -2;
        break;
      }
    }
#endif
    recv = suart_getchar(t_config->fdesc);
    if (recv >= 0)
    {
      if (cnt > 1024)
      {
        max_recv = cnt + 1;
        ret = realloc(ret, max_recv * sizeof(unsigned char));
      }
      ret[cnt] = recv;
      cnt++;
      if (!memcmp(ret + cnt - 2, STOP_BYTES, 2))
      {
        cnt -= 2;
        ret[cnt] = '\0';
        break;
      }

      ret_select = nb_serial_timeout(t_config, &tm_out);
      if (!ret_select)
      {
        cnt = -2;
        break;
      }

      if (recv == 0)
      {
        cnt_0++;
        if (cnt_0 > 100)
        {
          free(ret);
          ret = NULL;
          cnt = -1;
          break;
        }
      }
    }
    else
    {
      free(ret);
      ret = NULL;
      cnt = -1;
      break;
    }
  }
  *t_size = cnt;
  if (!ret_select)
    debug(__func__, "WARNING:", "Read timeout!");
  if (cnt < 1)
  {
    free(ret);
    ret = NULL;
    return NULL;
  }
  return ret;
}
