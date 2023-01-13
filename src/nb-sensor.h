#ifndef NB_SENSOR_H
#define NB_SENSOR_H

int nb_sensor_get_temp_hum_md02(char *temp_value, char *hum_value);
double nb_sensor_parsing_temp_md02(unsigned char *t_reg_recv);
double nb_sensor_parsing_hum_md02(unsigned char *t_reg_recv);

#endif