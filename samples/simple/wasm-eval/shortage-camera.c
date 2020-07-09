// Eval file.
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define uint8 uint8_t

void aerogel_sensor_native(
  uint8* sensor_name_list, // Name with '\t' separation and '\0' end.
  int len_sensor_name_list,
  uint8* frequency, // This nneds to be cast to uint32
  int len_frequency,
  uint8* duration, // This needs to be cast to uint32
  int len_duration,
  uint8* ret_list, // return value of each sensor. Let's just flatten it.
  int len_ret_list);

int main(int argc, char **argv)
{
  printf("======Shortage-Camera-Power starts======\n");
  char* sensor_name = "home_camera_image";
  int len_sensor_name_list = strlen(sensor_name) + 1;

  uint32_t frequency[] = {100};
  int len_frequency = sizeof(uint32_t) * 1;
  uint32_t duration[] = {1000000};
  int len_duration = sizeof(uint32_t) * 1;
  uint8* ret_list = malloc(1024);
  memset(ret_list, 0, 1024);
  int len_ret_list = 1024;
  aerogel_sensor_native((uint8*)sensor_name, len_sensor_name_list, (uint8*)frequency,
    len_frequency, (uint8*)duration, len_duration, (uint8*)ret_list, len_ret_list);

  printf("======Shortage-Camera-Power ends======\n");

  return 0;
}
