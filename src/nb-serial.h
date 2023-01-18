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

/*
 * #define CRC_POLY_xxxx
 *
 * The constants of the form CRC_POLY_xxxx define the polynomials for some well
 * known CRC calculations.
 */

#define CRC_POLY_16 0xA001
#define CRC_POLY_32 0xEDB88320ul
#define CRC_POLY_64 0x42F0E1EBA9EA3693ull
#define CRC_POLY_CCITT 0x1021
#define CRC_POLY_DNP 0xA6BC
#define CRC_POLY_KERMIT 0x8408
#define CRC_POLY_SICK 0x8005

/*
 * #define CRC_START_xxxx
 *
 * The constants of the form CRC_START_xxxx define the values that are used for
 * initialization of a CRC value for common used calculation methods.
 */

#define CRC_START_8 0x00
#define CRC_START_16 0x0000
#define CRC_START_MODBUS 0xFFFF
#define CRC_START_XMODEM 0x0000
#define CRC_START_CCITT_1D0F 0x1D0F
#define CRC_START_CCITT_FFFF 0xFFFF
#define CRC_START_KERMIT 0x0000
#define CRC_START_SICK 0x0000
#define CRC_START_DNP 0x0000
#define CRC_START_32 0xFFFFFFFFul
#define CRC_START_64_ECMA 0x0000000000000000ull
#define CRC_START_64_WE 0xFFFFFFFFFFFFFFFFull

int nb_serial_open(nb_serial_config_s *t_config);
int nb_serial_write(nb_serial_config_s *t_config, unsigned char *t_message, int t_size_message);
unsigned char *nb_serial_read(nb_serial_config_s *t_config, int *t_size);
void nb_serial_close(nb_serial_config_s *t_config);
unsigned char nb_serial_calcute_checksum(unsigned char *t_data, int t_len_data);

#endif