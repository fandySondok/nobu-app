#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "nb-conf.h"
#include "config-tools.h"

#define CORE_CONFIG "conf/core.conf"
#define TOPIC_CONFIG "conf/topic.conf"
#define CHANNEL_OPT "conf/channel_opt.conf"

#define MAX_CORE_BUFF 64
struct core_conf_info mq_conf;
struct core_topic_info mq_sub;

static pthread_mutex_t lockDebug = PTHREAD_MUTEX_INITIALIZER;

int debug(const char *function_name, char *debug_type, char *debug_msg, ...)
{
  pthread_mutex_lock(&lockDebug);

  char *tmp_debug_msg = NULL;
  tmp_debug_msg = (char *)calloc(500, sizeof(char));
  if (tmp_debug_msg == NULL)
  {
    pthread_mutex_unlock(&lockDebug);
    return -1;
  }

  struct timeval tv;
  gettimeofday(&tv, NULL); // timezone should be NULL
  struct tm *d_tm;
  d_tm = localtime(&tv.tv_sec);
  uint16_t msec = tv.tv_usec / 1000;
  char time_str[24];
  memset(time_str, 0x00, 24 * sizeof(char));
  snprintf(time_str, 24, "%04d-%02d-%02d %02d:%02d:%02d.%.3i", d_tm->tm_year + 1900, d_tm->tm_mon + 1, d_tm->tm_mday,
           d_tm->tm_hour, d_tm->tm_min, d_tm->tm_sec,
           msec);

  va_list aptr;
  va_start(aptr, debug_msg);
  vsprintf(tmp_debug_msg, debug_msg, aptr);
  va_end(aptr);
  if (strstr(debug_type, "INFO"))
  {
    printf("%sI:%s %s %s%s:%s %s\n", BGgreen, KNRM, time_str, BOLDGREEN, function_name, KNRM, tmp_debug_msg);
  }
  else if (strstr(debug_type, "ERROR"))
  {
    printf("%sE:%s %s %s%s:%s %s\n", BGred, KNRM, time_str, BGred, function_name, KNRM, tmp_debug_msg);
  }
  else if (strstr(debug_type, "WARNING"))
  {
    printf("%sW:%s %s %s%s:%s %s\n", BGyellow, KNRM, time_str, BOLDYELLOW, function_name, KNRM, tmp_debug_msg);
  }
  free(tmp_debug_msg);
  tmp_debug_msg = NULL;
  pthread_mutex_unlock(&lockDebug);
  return 0;
}

int8_t enco_tools_terminal_size(unsigned short *__col__, unsigned short *__lines__)
{
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
  {
    *__col__ = w.ws_col;
    *__lines__ = w.ws_row;
    return 0;
  }
  return errno;
}

void debug_hex(const char *kalimat, unsigned char *array, int ukuran)
{
  unsigned short col, row = 0;
  if (!enco_tools_terminal_size(&col, &row))
  {
    int8_t max_print_dash = 17;
    if ((max_print_dash * 2) + strlen(kalimat) + 3 > col)
    {
      max_print_dash = 1;
    }

    int8_t len_msg = strlen(kalimat) + max_print_dash * 2 + 3;
    char msg[len_msg];
    memset(msg, 0, len_msg * sizeof(char));
    for (int i = 0; i < max_print_dash; i++)
    {
      sprintf(msg + strlen(msg), "-");
    }
    sprintf(msg + strlen(msg), " %s ", kalimat);
    for (int i = 0; i < max_print_dash; i++)
    {
      sprintf(msg + strlen(msg), "-");
    }
    printf("%s\n", msg);

    int i = 0;
    int8_t separator = 0;
    int8_t max_separator = 0;
    (max_separator > col) ? (max_separator = 8) : (max_separator = 16);
    printf("%s", KCYN);
    do
    {
      printf("%02X ", array[i]);
      i++;
      separator++;
      if (separator == max_separator && i < ukuran)
      {
        printf("\n");
        separator = 0;
      }
    } while (i <= ukuran);
    printf("%s\n", KNRM);
    for (i = 0; i < (len_msg - 1); i++)
      printf("-");
    printf("\n");
  }
}

int conf_core_init()
{
  sconf_setup(SCONF_SET_MAX_BUFFER, MAX_CORE_BUFF);
  sconf_setup(SCONF_SET_MAX_LINE_LENGTH, 1024);
  sconf_setup(SCONF_SET_OPEN_TRY_TIMES, 10);
  sconf_setup(SCONF_SET_DEBUG_MODE, SCONF_DEBUG_OFF);
  printf("starting conf\n");

  if (conf_get_core_config() != 0)
    return -1;

  printf("ip addr     = %s\n", mq_conf.ip_addr);
  printf("ip port     = %i\n", mq_conf.ip_port);
  printf("uname       = %s\n", mq_conf.username);
  printf("passwd      = %s\n", mq_conf.passwd);
  printf("total topic = %i\n", mq_conf.total_topic);

  printf("P channel 1 = %i\n", mq_conf.port_channel1);
  printf("P channel 2 = %i\n", mq_conf.port_channel2);
  printf("P channel 3 = %i\n", mq_conf.port_channel3);
  printf("P channel 4 = %i\n", mq_conf.port_channel4);
  printf("P channel 5 = %i\n", mq_conf.port_channel5);

  if (mq_conf.total_topic > 0)
  {
    for (int i = 1; i <= mq_conf.total_topic; i++)
    {
      printf("topic%i = %s\n", i, mq_sub.topic1 + ((i - 1) * 50));
    }
  }

  return 0;
}

int conf_get_core_config()
{
  memset(&mq_conf, 0x00, sizeof(struct core_conf_info));
  char *buff = NULL;
  buff = (char *)malloc(20 * sizeof(char));
  if (buff == NULL)
  {
    printf("[ERROR] Failed to allocate memory\n");
    return -2;
  }
  memset(buff, 0, sizeof(20 * sizeof(char)));
  if (sconf_get_config(CORE_CONFIG, "ip_addr", mq_conf.ip_addr) != 0)
  {
    printf("[WARNING] ip_addr is missing\n");
    free(buff);
    return -1;
  }
  if (sconf_get_config(CORE_CONFIG, "ip_port", buff) != 0)
  {
    printf("[WARNING] ip port is missing\n");
    free(buff);
    return -1;
  }
  mq_conf.ip_port = atoi(buff);
  memset(buff, 0, sizeof(20 * sizeof(char)));

  if (sconf_get_config(CORE_CONFIG, "username", mq_conf.username) != 0)
  {
    printf("[WARNING] username is missing\n");
    free(buff);
    return -1;
  }
  if (sconf_get_config(CORE_CONFIG, "passwd", mq_conf.passwd) != 0)
  {
    printf("[WARNING] passwd is missing\n");
    free(buff);
    return -1;
  }

  if (sconf_get_config(CORE_CONFIG, "port_channel1", buff) != 0)
  {
    printf("[WARNING] port_channel1 is missing\n");
    free(buff);
    return -1;
  }
  mq_conf.port_channel1 = atoi(buff);
  memset(buff, 0, sizeof(20 * sizeof(char)));

  if (sconf_get_config(CORE_CONFIG, "port_channel2", buff) != 0)
  {
    printf("[WARNING] port_channel2 is missing\n");
    free(buff);
    return -1;
  }
  mq_conf.port_channel2 = atoi(buff);
  memset(buff, 0, sizeof(20 * sizeof(char)));

  if (sconf_get_config(CORE_CONFIG, "port_channel3", buff) != 0)
  {
    printf("[WARNING] port_channel3 is missing\n");
    free(buff);
    return -1;
  }
  mq_conf.port_channel3 = atoi(buff);
  memset(buff, 0, sizeof(20 * sizeof(char)));

  if (sconf_get_config(CORE_CONFIG, "port_channel4", buff) != 0)
  {
    printf("[WARNING] port_channel4 is missing\n");
    free(buff);
    return -1;
  }
  mq_conf.port_channel4 = atoi(buff);
  memset(buff, 0, sizeof(20 * sizeof(char)));

  if (sconf_get_config(CORE_CONFIG, "port_channel5", buff) != 0)
  {
    printf("[WARNING] port_channel5 is missing\n");
    free(buff);
    return -1;
  }
  mq_conf.port_channel5 = atoi(buff);
  memset(buff, 0, sizeof(20 * sizeof(char)));

  mq_conf.total_topic = sconf_check_line_conf(TOPIC_CONFIG);
  if (mq_conf.total_topic > 0)
    conf_get_topic_config(mq_conf.total_topic);

  free(buff);
  return 0;
}

int conf_get_topic_config(int total_topic_tmp)
{
  memset(&mq_sub, 0x00, sizeof(struct core_topic_info));
  char *buff = NULL;
  buff = (char *)malloc(50 * sizeof(char));
  if (buff == NULL)
  {
    printf("[ERROR] Failed to allocate memory\n");
    return -2;
  }

  for (int i = 1; i <= total_topic_tmp; i++)
  {
    memset(buff, 0, sizeof(50 * sizeof(char)));
    snprintf(buff, 50 * sizeof(char), "topic%i", i);
    if (sconf_get_config(TOPIC_CONFIG, buff, mq_sub.topic1 + ((i - 1) * 50)) != 0)
    {
      printf("[ERROR] %s is missing\n", buff);
      free(buff);
      return -1;
    }
  }
  free(buff);
  return 0;
}

int8_t nb_conf_read_file(char *t_file_name, char *t_file_contain)
{
  FILE *buffer = NULL;
  uint8_t try_times = 5;

  do
  {
    buffer = fopen(t_file_name, "r");
    try_times--;
  } while (buffer == NULL && try_times > 0);

  if (buffer == NULL)
  {
    debug(__func__, "ERROR", "failed to open file %s", t_file_name);
    return -1;
  }

  char buffer_fcon[512];
  char s_character = 0;
  uint16_t len_schar = 0;
  memset(buffer_fcon, 0x00, sizeof(buffer_fcon));

  while ((s_character = fgetc(buffer)) != EOF)
  {
    if (s_character > 127 || s_character < 9 || len_schar == (sizeof(buffer_fcon) - 1))
      break;
    buffer_fcon[len_schar] = s_character;
    len_schar++;
  }
  strcpy(t_file_contain, buffer_fcon);
  fclose(buffer);
  return 0;
}