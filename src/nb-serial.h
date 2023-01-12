#ifndef NB_SERIAL_H
#define NB_SERIAL_H

#include <stdint.h>
#include <sys/types.h>

typedef struct
{
  int port;
  int baudrate;
  int fdesc;
} nb_serial_config_s;

int nb_serial_open(nb_serial_config_s *t_config);
int nb_serial_write(nb_serial_config_s *t_config, unsigned char *t_message, int t_size_message);
unsigned char *nb_serial_read(nb_serial_config_s *t_config, int *t_size);
void nb_serial_close(nb_serial_config_s *t_config);
unsigned char nb_serial_calcute_checksum(unsigned char *t_data, int t_len_data);

#endif