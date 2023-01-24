#ifndef S_JSON_H
#define S_JSON_H

#include <json-c/json.h>

int nb_parsing_json_string(json_object *obj_tmp, const char *field_json, char *target_dat);
int32_t nb_parsing_json_int(json_object *obj_tmp, const char *field_json);
int nb_parsing_json_object_string(json_object *obj_tmp, const char *field_json, char *target_dat, char *obj_data);
int32_t nb_parsing_json_object_int(json_object *obj_tmp, const char *field_json, char *obj_data);

#endif