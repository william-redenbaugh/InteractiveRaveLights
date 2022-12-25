#ifndef _HSV2RGB_H
#define _HSV2RGB_H
#include "stdio.h"

typedef struct {
  unsigned char h;
  unsigned char s;
  unsigned char v;
} hsv_color;

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} rgb_color;

rgb_color hsv2rgb(hsv_color hsv);

#endif