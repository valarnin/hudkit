#include <json.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TITLE_MAX_LENGTH 1024
#define URL_MAX_LENGTH 1024 * 16
#define HOTKEY_MAX_LENGTH 1024

char *configFilePath;

int x, y, width, height;
char title[TITLE_MAX_LENGTH];
char url[URL_MAX_LENGTH];
char hotkey[HOTKEY_MAX_LENGTH];
json_object *config;

void set_config_value_int(json_object *object, char *key, int val) {
  json_object *prop = json_object_object_get(object, key);
  json_object_set_int(prop, val);
}

int get_config_value_int(json_object *object, char *key) {
  json_object *prop;
  bool result;
  json_type type;

  result = json_object_object_get_ex(object, key, &prop);
  if (!result) {
    fprintf(stderr, "No property \"%s\" in config object, aborting.\n\n", key);
    exit(1);
  }

  type = json_object_get_type(prop);
  if (type != json_type_int) {
    fprintf(stderr, "Property \"%s\" was %s, expected int, aborting.\n\n", key,
            json_type_to_name(type));
    exit(1);
  }

  return json_object_get_int(prop);
}

void get_config_value_string(json_object *object, char *key, char *buffer,
                             int length) {
  json_object *prop;
  bool result;
  json_type type;

  result = json_object_object_get_ex(object, key, &prop);
  if (!result) {
    fprintf(stderr, "No property \"%s\" in config object, aborting.\n\n", key);
    exit(1);
  }

  type = json_object_get_type(prop);
  if (type != json_type_string) {
    fprintf(stderr, "Property \"%s\" was %s, expected string, aborting.\n\n",
            key, json_type_to_name(type));
    exit(1);
  }

  const char *str = json_object_get_string(prop);
  int strLen = json_object_get_string_len(prop);
  strncpy(buffer, str, length);
}
void read_config(char *src) {
  configFilePath = src;
  FILE *f;
  if ((f = fopen(src, "r"))) {
    fclose(f);
    // Read our config file
    fprintf(stdout, "Parsing config file at %s...\n", src);
    config = json_object_from_file(src);

    if (((int)config) == -1) {
      fprintf(stderr, "Failed to parse config file at %s!\n\n", src);
      fprintf(stderr, "Error returned:\n");
      fprintf(stderr, "%s", json_util_get_last_err());
      exit(1);
    }

    x = get_config_value_int(config, "x");
    y = get_config_value_int(config, "y");
    width = get_config_value_int(config, "width");
    height = get_config_value_int(config, "height");

    get_config_value_string(config, "title", title, TITLE_MAX_LENGTH);
    get_config_value_string(config, "url", url, URL_MAX_LENGTH);
    get_config_value_string(config, "hotkey", hotkey, HOTKEY_MAX_LENGTH);

    fprintf(stdout,
            "Opening overlay \"%s\" at %i,%i,%i,%i with hotkey \"%s\" and "
            "URL:\n%s\n",
            title, x, y, width, height, hotkey, url);
  } else {
    fprintf(stderr, "Config file does not exist!\n");
    if (!access(src, W_OK)) {
      fprintf(stderr,
              "No write access to directory of config file (%s), aborting!\n",
              realpath(dirname(src), NULL));
      exit(1);
    }
    fprintf(stderr,
            "Creating a new default config file for you to customize at:\n%s\n",
            src);
    FILE *f = fopen(src, "w");
    fprintf(f, "{\"title\":\"example "
               "overlay\",\"url\":\"file:///path/to/file.html?OVERLAY_WS=ws://"
               "127.0.0.1:10501/"
               "ws\",\"x\":100,\"y\":100,\"width\":200,\"height\":200,"
               "\"hotkey\":\"<Ctrl>L\"}");
    fflush(f);
    fclose(f);
    exit(1);
  }
}

void write_config() {
  set_config_value_int(config, "x", x);
  set_config_value_int(config, "y", y);
  set_config_value_int(config, "width", width);
  set_config_value_int(config, "height", height);

  json_object_to_file(configFilePath, config);
}
