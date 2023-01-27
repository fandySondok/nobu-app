#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "nb-json.h"
#include "nb-conf.h"

int nb_parsing_json_string(json_object *obj_tmp, const char *field_json, char *target_dat)
{
  if (sizeof(field_json) == 0)
  {
    debug(__func__, "ERROR", "field not valid");
    return -1;
  }
  json_object *new_obj = NULL;
  new_obj = json_object_object_get(obj_tmp, field_json);
  if (new_obj == NULL)
  {
    // debug(__func__, "ERROR", "field %s does not exist", field_json);
    // debug(__func__, "INFO", "resp\n%s\n", json_object_get_string(obj_tmp));
    return -2;
  }
  strcpy(target_dat, json_object_get_string(new_obj));
  return 0;
}

int nb_parsing_json_object_string(json_object *obj_tmp, const char *field_json, char *target_dat, char *obj_data)
{
  if (sizeof(field_json) == 0)
  {
    debug(__func__, "ERROR", "field not valid");
    return -1;
  }
  json_object *new_obj = NULL;
  new_obj = json_object_object_get(obj_tmp, obj_data);
  if (new_obj == NULL)
  {
    debug(__func__, "ERROR", "field %s does not exist", obj_data);
    return -2;
  }
  new_obj = json_object_object_get(new_obj, field_json);
  if (new_obj == NULL)
  {
    debug(__func__, "ERROR", "field %s does not exist", field_json);
    return -2;
  }

  strcpy(target_dat, json_object_get_string(new_obj));
  return 0;
}

int32_t nb_parsing_json_int(json_object *obj_tmp, const char *field_json)
{
  if (sizeof(field_json) == 0)
  {
    debug(__func__, "ERROR", "field not valid");
    return -1;
  }
  json_object *new_obj = NULL;
  if (!json_object_object_get_ex(obj_tmp, field_json, &new_obj))
  {
    // debug(__func__, "ERROR", "field %s does not exist", field_json);
    return -2;
  }
  return json_object_get_int(new_obj);
}

int32_t nb_parsing_json_object_int(json_object *obj_tmp, const char *field_json, char *obj_data)
{
  if (sizeof(field_json) == 0)
  {
    debug(__func__, "ERROR", "field not valid");
    return -1;
  }
  json_object *new_obj = NULL;
  new_obj = json_object_object_get(obj_tmp, obj_data);
  if (new_obj == NULL)
  {
    debug(__func__, "ERROR", "field %s does not exist", obj_data);
    return -2;
  }
  new_obj = json_object_object_get(new_obj, field_json);
  if (new_obj == NULL)
  {
    debug(__func__, "ERROR", "field %s does not exist", field_json);
    return -2;
  }

  return json_object_get_int(new_obj);
}

int nb_parsing_json_boolean(json_object *obj_tmp, const char *field_json, bool *target_dat)
{
  if (sizeof(field_json) == 0)
  {
    debug(__func__, "ERROR", "field not valid");
    return -1;
  }
  json_object *new_obj = NULL;
  new_obj = json_object_object_get(obj_tmp, field_json);
  if (new_obj == NULL)
  {
    debug(__func__, "ERROR", "field %s does not exist", field_json);
    debug(__func__, "INFO", "resp\n%s\n", json_object_get_string(obj_tmp));
    return -2;
  }
  *target_dat = json_object_get_boolean(new_obj);
  return 0;
}