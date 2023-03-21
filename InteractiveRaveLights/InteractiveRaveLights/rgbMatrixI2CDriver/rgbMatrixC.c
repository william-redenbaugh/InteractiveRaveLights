#include "rgbMatrix.h"
#include "string.h"
#include <stdio.h>
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>
#include "rgbMatrix.h"
#include "assert.h"

#define CONFIGURE_CMD_PAGE 0XFD
#define FRAME1_PAGE 0X00
#define FRAME2_PAGE 0X01
#define FUNCTION_PAGE 0X0B
#define DETECTION_PAGE 0X0C
#define LED_VAF_PAGE 0X0D

#define CONFIGURATION_REG 0X00
#define PICTURE_DISPLAY_REG 0X01
#define DISPLAY_OPTION_REG 0X05
#define BREATH_CTL_REG 0X08
#define BREATH_CTL_REG2 0X09
#define SW_SHUT_DOWN_REG 0X0A

#define AUDIO_GAIN_CTL_REG 0X0B
#define STAGGERED_DELAY_REG 0X0D
#define SLEW_RATE_CTL_REG 0X0E
#define CURRENT_CTL_REG 0X0F
#define VAF_CTL_REG 0X14
#define VAF_CTL_REG2 0X15

#define MSKSTD1 (0x3 << 0)
#define MSKSTD2 (0x3 << 2)
#define MSKSTD3 (0x3 << 4)
#define MSKSTD4 (0x3 << 6)
#define CONST_STD_GROUP1 0x00
#define CONST_STD_GROUP2 0x55
#define CONST_STD_GROUP3 0xAA
#define CONST_STD_GROUP4 0xFF

#define MSKVAF1 (0x4 << 0)
#define MSKVAF2 (0x4 << 4)
#define MSKVAF3 (0x4 << 0)

#define MSKFORCEVAFTIME_CONST (0x0 << 3)
#define MSKFORCEVAFCTL_ALWAYSON (0x0 << 6)
#define MSKFORCEVAFCTL_DISABLE (0x2 << 6)
#define MSKCURRENT_CTL_EN (0x1 << 7)
#define CONST_CURRENT_STEP_20mA (0x19 << 0)
#define MSKBLINK_FRAME_300 (0x0 << 6)
#define MSKBLINK_EN (0x1 << 3)
#define MSKBLINK_DIS (0x0 << 3)
#define MSKBLINK_PERIOD_TIME_CONST (0x7 << 0)

#define RGB_MATRIX_ADDRESS 0x74

extern uint8_t icons[20][64];
extern uint8_t font8_8[92][8];

uint8_t bg_R = 0, bg_G = 0, bg_B = 0;

uint32_t color_hsv(uint16_t hue, uint8_t sat, uint8_t val);

uint8_t gamma8(uint8_t x)
{
  return _NeoPixelGammaTable[x];
}

uint32_t gamma32(uint32_t x);

const uint8_t tab_led_type3Vaf[64] = {
    // Reference SLED1735 Datasheet Type3 Circuit Map
    // Frame 1
    0x50, 0x55, 0x55, 0x55, // C1-A ~ C1-P
    0x00, 0x00, 0x00, 0x00, // C2-A ~ C2-P ,
    0x00, 0x00, 0x00, 0x00, // C3-A ~ C3-P
    0x15, 0x54, 0x55, 0x55, // C4-A ~ C4-P
    0x00, 0x00, 0x00, 0x00, // C5-A ~ C5-P
    0x00, 0x00, 0x00, 0x00, // C6-A ~ C6-P
    0x55, 0x05, 0x55, 0x55, // C7-A ~ C7-P
    0x00, 0x00, 0x00, 0x00, // C8-A ~ C8-P
    // Frame 2
    0x00, 0x00, 0x00, 0x00, // C9-A ~ C9-P
    0x55, 0x55, 0x41, 0x55, // C10-A ~ C10-P
    0x00, 0x00, 0x00, 0x00, // C11-A ~ C11-P
    0x00, 0x00, 0x00, 0x00, // C12-A ~ C12-P
    0x55, 0x55, 0x55, 0x50, // C13-A ~ C13-P
    0x00, 0x00, 0x00, 0x00, // C14-A ~ C14-P
    0x00, 0x00, 0x00, 0x00, // C15-A ~ C15-P
    0x00, 0x00, 0x00, 0x00, // C16-A ~ C16-P
};
// Table Index means the number of LED, and the data of table means PWM control ram address
const uint8_t tab_led_type_three_pwm_ctl_anode_red[70] = {
    // Reference SLED1735 Datasheet Type3 Map
    0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, // Red D1~D14 PWM CTL Mapping address
    0x50, 0x51, 0x52, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, // Red D15~D28 PWM CTL Mapping address
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, // Red D29~D42 PWM CTL Mapping address
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // Red D43~D56 PWM CTL Mapping address
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6E, 0x6F, // Red D57~D70 PWM CTL Mapping address
};

// Table Index means the number of LED, and the data of table means PWM control ram address
const uint8_t tab_led_type_three_pwm_ctl_anode_green[70] = {
    // Reference SLED1735 Datasheet Type3 Map
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // Green D1~D14 PWM CTL Mapping address
    0x60, 0x61, 0x62, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, // Green D15~D28 PWM CTL Mapping address
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, // Green D29~D42 PWM CTL Mapping address
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // Green D43~D56 PWM CTL Mapping address
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7E, 0x7F, // Green D57~D70 PWM CTL Mapping address
};

// Table Index means the number of LED, and the data of table means PWM control ram address
const uint8_t tab_led_type_three_pwm_ctl_anode_blue[70] = {
    // Reference SLED1735 Datasheet Type3 Map
    0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // Blue D1~D14 PWM CTL Mapping address
    0x70, 0x71, 0x72, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, // Blue D15~D28 PWM CTL Mapping address
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, // Blue D29~D42 PWM CTL Mapping address
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, // Blue D43~D56 PWM CTL Mapping address
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8E, 0x8F, // Blue D57~D70 PWM CTL Mapping address
};

uint8_t matrix_pow(uint8_t x, uint8_t n)
{
  if (0 == n)
  {
    return 1;
  }
  if (1 == n)
  {
    return x;
  }
  return x * matrix_pow(x, n - 1);
}

void disp_show_color(struct I2C1735 *handle, uint8_t R, uint8_t G, uint8_t B)
{
  for (int i = 0; i < 64; i++)
  {
    handle->rgb_buffer[i][0] = B;
    handle->rgb_buffer[i][1] = G;
    handle->rgb_buffer[i][2] = R;
  }
  image(handle);
}

void disp_show_color_uint32(struct I2C1735 *handle, uint32_t c)
{
  for (int i = 0; i < 64; i++)
  {
    handle->rgb_buffer[i][0] = (uint8_t)(c >> 16);
    handle->rgb_buffer[i][1] = (uint8_t)(c >> 8);
    handle->rgb_buffer[i][2] = (uint8_t)c;
  }
  image(handle);
}

void draw_line_color(struct I2C1735 *handle, uint8_t coor[4], uint8_t R, uint8_t G, uint8_t B)
{
  if (coor[0] == coor[2])
  {
    for (int i = coor[1] * 8 + coor[0]; i < (coor[3] + 1) * 8; i += 8)
    {
      handle->rgb_buffer[i][0] = B;
      handle->rgb_buffer[i][1] = G;
      handle->rgb_buffer[i][2] = R;
    }
  }
  else if (coor[1] == coor[3])
  {
    for (int i = coor[1] * 8 + coor[0]; i < coor[1] * 8 + coor[2] + 1; i++)
    {
      handle->rgb_buffer[i][0] = B;
      handle->rgb_buffer[i][1] = G;
      handle->rgb_buffer[i][2] = R;
    }
  }
}
void draw_line(struct I2C1735 *handle, uint8_t coor[4], uint32_t c)
{
  if (coor[0] == coor[2])
  {
    for (int i = coor[1] * 8 + coor[0]; i < (coor[3] + 1) * 8; i += 8)
    {
      handle->rgb_buffer[i][0] = (uint8_t)(c >> 16);
      handle->rgb_buffer[i][1] = (uint8_t)(c >> 8);
      handle->rgb_buffer[i][2] = (uint8_t)c;
    }
  }
  else if (coor[1] == coor[3])
  {
    for (int i = coor[1] * 8 + coor[0]; i < coor[1] * 8 + coor[2] + 1; i++)
    {
      handle->rgb_buffer[i][0] = (uint8_t)(c >> 16);
      handle->rgb_buffer[i][1] = (uint8_t)(c >> 8);
      handle->rgb_buffer[i][2] = (uint8_t)c;
    }
  }
}

void draw_point_color(struct I2C1735 *handle, uint8_t coor[2], uint8_t R, uint8_t G, uint8_t B)
{
  handle->rgb_buffer[8 * coor[1] + coor[0]][0] = B;
  handle->rgb_buffer[8 * coor[1] + coor[0]][1] = G;
  handle->rgb_buffer[8 * coor[1] + coor[0]][2] = R;
}

void draw_point(struct I2C1735 *handle, uint8_t coor[2], uint32_t c)
{

  handle->rgb_buffer[8 * coor[1] + coor[0]][0] = (uint8_t)(c >> 16);
  handle->rgb_buffer[8 * coor[1] + coor[0]][1] = (uint8_t)(c >> 8);
  handle->rgb_buffer[8 * coor[1] + coor[0]][2] = (uint8_t)c;
}

void draw_rectangle_color(struct I2C1735 *handle, uint8_t coor[4], uint8_t R, uint8_t G, uint8_t B)
{
  uint8_t line[8][4] = {{}};
  for (int i = 0; i < coor[3] - coor[1] + 1; i++)
  {
    line[i][0] = coor[0];
    line[i][1] = coor[1] + i;
    line[i][2] = coor[2];
    line[i][3] = coor[1] + i; // e.g:coor[4]={3,3,5,5}, line[0]={3,3,5,3},line[1]={3,4,5,4}

    draw_line_color(handle, line[i], R, G, B);
  }
}
void draw_rectangle(struct I2C1735 *handle, uint8_t coor[4], uint32_t c)
{
  uint8_t line[8][4] = {{}};
  for (int i = 0; i < coor[3] - coor[1] + 1; i++)
  {
    line[i][0] = coor[0];
    line[i][1] = coor[1] + i;
    line[i][2] = coor[2];
    line[i][3] = coor[1] + i; // e.g:coor[4]={3,3,5,5}, line[0]={3,3,5,3},line[1]={3,4,5,4}

    draw_line(handle, line[i], c);
  }
}

void set_backcolor(struct I2C1735 *handle, uint8_t R, uint8_t G, uint8_t B)
{
  bg_R = R;
  bg_G = G;
  bg_B = B;
}

void char2rgb_line(struct I2C1735 *handle, char charline, char row, char column, uint8_t R, uint8_t G, uint8_t B)
{
  if (charline & 0x80)
  {
    handle->rgb_buffer[8 * row + column][0] = B;
    handle->rgb_buffer[8 * row + column][1] = G;
    handle->rgb_buffer[8 * row + column][2] = R;
  }
  else
  {
    handle->rgb_buffer[8 * row + column][0] = bg_B;
    handle->rgb_buffer[8 * row + column][1] = bg_G;
    handle->rgb_buffer[8 * row + column][2] = bg_R;
  }
}

void disp_show_char(struct I2C1735 *handle, char chr, uint8_t R, uint8_t G, uint8_t B, char bias, char direction)
{
  char i, j;
  unsigned char Char, temp;
  unsigned char chrtemp[24] = {0};
  if ((bias > 8) || (bias < -8))
    return;

  Char = chr - 32;
  j = bias;
  // Reading character data
  for (i = 0; i < 8; i++)
  {
    chrtemp[j] = (font8_8[Char][i]); // moving the array is equivalent to moving characters
    j++;
  }
  // Convert to RGB array
  switch (direction)
  {
  // display direction 0
  case 0:
    for (i = 0; i < 8; i++)
    {
      temp = chrtemp[i];
      for (j = 0; j < 8; j++)
      {
        char2rgb_line(handle, temp, i, j, R, G, B);
        temp = temp << 1;
      }
    }
    break;
  // display direction 1
  case 1:
    for (j = 7; j > -1; j--)
    {
      temp = chrtemp[j % 7];
      for (i = 0; i < 8; i++)
      {
        char2rgb_line(handle, temp, i, j, R, G, B);
        temp = temp << 1;
      }
    }
    break;
  // display direction 2
  case 2:
    for (i = 7; i > -1; i--)
    {
      temp = chrtemp[i % 7];
      for (j = 7; j > -1; j--)
      {

        char2rgb_line(handle, temp, i, j, R, G, B);
        temp = temp << 1;
      }
    }
    break;
  // display direction 3
  case 3:
    for (j = 0; j < 8; j++)
    {
      temp = chrtemp[j];
      for (i = 7; i > -1; i--)
      {
        char2rgb_line(handle, temp, i, j, R, G, B);
        temp = temp << 1;
      }
    }
    break;
  }
  // display rgbmatrix list
  image(handle);
}
void flowchar(struct I2C1735 *handle, char chr, uint8_t R, uint8_t G, uint8_t B)
{
  for (int i = 5; i > -8; i--)
  {
    disp_show_char(handle, chr, R, G, B, i, 0);
    delay(50);
  }
}

void show_text(struct I2C1735 *handle, char str[], uint8_t R, uint8_t G, uint8_t B)
{
  int len = strlen(str);
  for (int i = 0; i < len; i++)
  {
    flowchar(handle, str[i], R, G, B);
  }
}

void flow_text(struct I2C1735 *handle, char str[], uint8_t R, uint8_t G, uint8_t B, char direction, int delayms)
{
  int len = strlen(str);
  int i = 0, j = 0, k = 0, x = 0, y = 0;
  unsigned char chrtemp[24] = {0};
  char letter_spacing = 2;
  unsigned char temp;
  str[len] = ' ';
  for (i = 0; i < len; i++)
  {
    for (j = 0; j < 8; j++)
    {
      for (k = 0; k < 8 - j; k++)
      {
        chrtemp[k] = pgm_read_uint8_t(&(font8_8[str[i] - 32][k + j]));
      }
      for (k = 8 - j; k < 8; k++)
      {
        chrtemp[k] = pgm_read_uint8_t(&(font8_8[str[i + 1] - 32][k - (8 - j)]));
      }
      switch (direction)
      {
      // display direction 0
      case 0:
        for (x = 0; x < 8; x++)
        {
          temp = chrtemp[x];
          for (y = 0; y < 8; y++)
          {
            char2rgb_line(handle, temp, x, y, R, G, B);
            temp = temp << 1;
          }
        }
        break;
      // display direction 1
      case 1:
        for (y = 7; y > -1; y--)
        {
          temp = chrtemp[y % 7];
          for (x = 0; x < 8; x++)
          {
            char2rgb_line(handle, temp, x, y, R, G, B);
            temp = temp << 1;
          }
        }
        break;
      // display direction 2
      case 2:
        for (x = 7; x > -1; x--)
        {
          temp = chrtemp[x % 7];
          for (y = 7; y > -1; y--)
          {

            char2rgb_line(handle, temp, x, y, R, G, B);
            temp = temp << 1;
          }
        }
        break;
      // display direction 3
      case 3:
        for (y = 0; y < 8; y++)
        {
          temp = chrtemp[y];
          for (x = 7; x > -1; x--)
          {
            char2rgb_line(handle, temp, x, y, R, G, B);
            temp = temp << 1;
          }
        }
        break;
      }
      image(handle);
      delay(delayms);
    }
  }
}

void show_hex(struct I2C1735 *handle, uint8_t hex[], uint8_t R, uint8_t G, uint8_t B, char bias)
{
  char i, j, temp;
  unsigned char chrtemp[24] = {0};
  unsigned char x, y, temp2;
  unsigned char chrtemp2[24] = {0};
  y = bias;
  temp2 = 0;
  for (int dex = 0; dex < 8; dex++)
  {
    for (x = 0; x < 8; x++)
    {
      if ((hex[x] << dex) & 0x80)
      {
        temp2 = 1 * matrix_pow(2, x) + temp2;
        chrtemp2[y] = temp2;
      }
      else
      {
        temp2 = 0 * matrix_pow(2, x) + temp2;
        chrtemp2[y] = temp2;
      }
    }
    temp2 = 0;
    y++;
  }
  for (i = 0; i < 8; i++)
  {
    temp = chrtemp2[i];
    for (j = 7; j > -1; j--)
    {
      if (temp & 0x80)
      {
        handle->rgb_buffer[8 * j + i][0] = B;
        handle->rgb_buffer[8 * j + i][1] = G;
        handle->rgb_buffer[8 * j + i][2] = R;
      }
      else
      {
        handle->rgb_buffer[8 * j + i][0] = 0;
        handle->rgb_buffer[8 * j + i][1] = 0;
        handle->rgb_buffer[8 * j + i][2] = 0;
      }
      temp = temp << 1;
    }
  }
  image(handle);
}

void disp_show_pic(struct I2C1735 *handle, uint8_t Index, uint8_t R, uint8_t G, uint8_t B)
{
  int data_val = 0;
  for (int i = 0; i < 64; i++)
  {
    data_val = pgm_read_uint8_t(&(icons[Index][i]));
    if (data_val == 1)
    {
      handle->rgb_buffer[i][0] = B;
      handle->rgb_buffer[i][1] = G;
      handle->rgb_buffer[i][2] = R;
    }
    else if (data_val == 0)
    {
      handle->rgb_buffer[i][0] = 0;
      handle->rgb_buffer[i][1] = 0;
      handle->rgb_buffer[i][2] = 0;
    }
  }
  image(handle);
}

void check_err_i2c(int result)
{
  switch (result)
  {
  case EBADF:
    printf("I2C FD is not a valid descriptor");
    break;
  case EFAULT:
    printf("Arg references an inaccessamble memory area I2C");
    break;
  case EINVAL:
    printf("CMD or ARG is not valid I2C");
    break;
  case ENOTTY:
    printf("I2C: fd is not associated with a character special device");
    break;
  default:
    printf("Unkown I2C error has been found: %d\n", result);
    break;
  }

  ASSERT((result < 0));
}

void write_cmd(struct I2C1735 *handle, uint8_t reg, uint8_t cmd)
{
  int16_t ex;
  struct i2c_msg_s i2c_msg;
  struct i2c_transfer_s i2c_transfer;
  uint8_t tx_buff[2];
  tx_buff[0] = reg;
  tx_buff[1] = cmd;

  i2c_msg.addr = RGB_MATRIX_ADDRESS;
  i2c_msg.buffer = tx_buff;
  i2c_msg.length = 2;
  i2c_msg.flags = I2C_M_NOSTART;
  i2c_msg.frequency = I2C_SPEED_FAST_PLUS;

  i2c_transfer.msgv = &i2c_msg;
  i2c_transfer.msgc = 1;
  int transfer_result = ioctl(handle->i2c_handle, I2CIOC_TRANSFER, (unsigned long)(uintptr_t)&i2c_transfer);

  if (transfer_result < 0)
  {
    printf("i2c Write Error: Couldn't write to LED matrix correctly...\n");
    check_err_i2c(transfer_result);

  }
}

void write_array_data(struct I2C1735 *handle, uint8_t startaddr, uint8_t data[64], uint8_t data_len)
{
  for (int i = 0; i < data_len; i++)
  {
    write_cmd(handle, startaddr, data[i]);
    startaddr++;
  }
}

void write_n_data(struct I2C1735 *handle, uint8_t startaddr, uint8_t data, uint8_t data_len)
{
  for (int i = 0; i < data_len; i++)
  {
    write_cmd(handle, startaddr, data);
    startaddr++;
  }
}

void image_test(struct I2C1735 *handle)
{
  image(handle);
}

void image(struct I2C1735 *handle)
{
  uint8_t revert_image[3][64] = {};
  for (int i = 0; i < 64; i++)
  {
    revert_image[0][i] = handle->rgb_buffer[i][1];
    revert_image[1][i] = handle->rgb_buffer[i][0];
    revert_image[2][i] = handle->rgb_buffer[i][2];
  }

  uint8_t reg = 0x20; //一页的寄存器起始地址
  uint8_t empty = 0;  //寄存器地址空缺的位置（写入的数据需要补0）
  uint8_t pos = 0;    //数据列表需要写入的数据所对应的索引
  for (int i = 0; i < 15; i++)
  {
    if (i == 0)
    {
      // print("Write Page 1")
      write_cmd(handle, CONFIGURE_CMD_PAGE, FRAME1_PAGE); //设置写入的页
    }
    else if (reg == 0x20)
    {
      // print("Write Page 2")
      write_cmd(handle, CONFIGURE_CMD_PAGE, FRAME2_PAGE);
    }

    uint8_t color = i % 3;
    uint8_t pos_data = pos * 14;
    int data_left = 64 - (pos + 1) * 14;

    uint8_t data_lenth = 0;
    if (data_left > 0)
    {
      data_lenth = 14;
    }
    else
    {
      data_lenth = 8;
      pos_data = 56;
    }
    uint8_t data[data_lenth];
    memset(data, 0, data_lenth);
    for (int i = 0; i < data_lenth; i++)
    {
      // data[i] = revert_image_data[pos_data+i];
      data[i] = revert_image[color][pos_data + i];
    }
    uint8_t insert_data[data_lenth + 2];
    memset(insert_data, 0, data_lenth + 2);
    for (int i = 0; i < (data_lenth + 2); i++)
    {
      if (i < empty)
      {
        insert_data[i] = data[i];
      }
      else if (i == empty || i == (empty + 1))
      {
        insert_data[i] = 0;
      }

      else if (i > (empty + 1))
      {
        insert_data[i] = data[i - 2];
      }
    }
    if (data_lenth == 8)
    {
      insert_data[8] = 0;
      insert_data[9] = 0;
    }
    int16_t ex;
    struct i2c_msg_s i2c_msg;
    struct i2c_transfer_s i2c_transfer;
    uint8_t tx_buff;

    uint8_t out_buff[data_lenth + 3];
    out_buff[0] = reg;
    for (int n = 0; n < data_lenth + 2; n++)
      out_buff[n + 1] = insert_data[n];

    i2c_msg.addr = RGB_MATRIX_ADDRESS;
    i2c_msg.buffer = out_buff;
    i2c_msg.length = data_lenth + 3;
    i2c_msg.flags = I2C_M_NOSTART;
    i2c_msg.frequency = I2C_SPEED_FAST_PLUS;

    i2c_transfer.msgv = &i2c_msg;
    i2c_transfer.msgc = 1;

    int result = ioctl(handle->i2c_handle, I2CIOC_TRANSFER, (unsigned long)(uintptr_t)&i2c_transfer);

    if (result < 0)
    {
      printf("i2c Write Error: Couldn't write to LED matrix correctly...\n");

    }
    if (color == 2)
    {
      empty += 3;
      pos += 1;
    }
    reg += 0x10;
    if (reg == 0xA0)
    {
      reg = 0x20;
    }
  }
}

void i2c_init(struct I2C1735 *handle)
{
  int fd;

  /* Open the i2c driver */
  printf("Initializing i2c device...\n");
  fd = open("/dev/i2c0", O_WRONLY);
  if (fd < 0)
  {
    printf("ERROR: failed to open /dev/i2c0: %d\n", fd);
    return ERROR;
  }
  else
  {
    printf("i2c init successful!\n");
  }
  handle->i2c_handle = fd;
}

void RGBMatrixInit(struct I2C1735 *handle)
{
  // put your setup code here, to run once:
  i2c_init(handle);
  write_cmd(handle, CONFIGURE_CMD_PAGE, FUNCTION_PAGE);
  write_cmd(handle, SW_SHUT_DOWN_REG, 0x0);                                                                                                                            // System must go to SW shutdowm mode when initialization
  write_cmd(handle, PICTURE_DISPLAY_REG, 0x10);                                                                                                                        // Setting Matrix Type = Type3
  write_cmd(handle, STAGGERED_DELAY_REG, ((MSKSTD4 & CONST_STD_GROUP4) | (MSKSTD3 & CONST_STD_GROUP3) | (MSKSTD2 & CONST_STD_GROUP2) | (MSKSTD1 & CONST_STD_GROUP1))); // Setting Staggered Delay
  write_cmd(handle, SLEW_RATE_CTL_REG, 0x1);                                                                                                                           // Enable Slew Rate control
  write_cmd(handle, VAF_CTL_REG, (MSKVAF2 | MSKVAF1));                                                                                                                 // VAF Control settings base on the LED type.
  write_cmd(handle, VAF_CTL_REG2, (MSKFORCEVAFCTL_DISABLE | MSKFORCEVAFTIME_CONST | MSKVAF3));
  write_cmd(handle, CURRENT_CTL_REG, (MSKCURRENT_CTL_EN | CONST_CURRENT_STEP_20mA)); // Setting LED driving current = 20mA and Enable current control
  write_cmd(handle, CONFIGURE_CMD_PAGE, FRAME1_PAGE);                                // Init Frame1Page(Clear all Ram) Setting SLED1735 Ram Page to Frame 1 Page
  write_n_data(handle, 0x00, 0X00, 0XB3);
  write_cmd(handle, CONFIGURE_CMD_PAGE, FRAME2_PAGE);
  write_n_data(handle, 0x00, 0X00, 0XB3);
  write_cmd(handle, CONFIGURE_CMD_PAGE, LED_VAF_PAGE);
  write_array_data(handle, 0X00, tab_led_type3Vaf, 0X40);

  write_cmd(handle, CONFIGURE_CMD_PAGE, FUNCTION_PAGE);
  write_cmd(handle, SW_SHUT_DOWN_REG, 0x1); // After initialization , system back to SW Normal mode.
  write_cmd(handle, CONFIGURE_CMD_PAGE, FRAME1_PAGE);
  write_n_data(handle, 0X00, 0XFF, 0X10); // Clear LED CTL Registers (Frame1Page)
  write_n_data(handle, 0x20, 0x00, 0X80);
  write_cmd(handle, CONFIGURE_CMD_PAGE, FRAME2_PAGE);
  write_n_data(handle, 0X00, 0XFF, 0X10); // Clear LED CTL Registers (Frame1Page)
  write_n_data(handle, 0x20, 0x00, 0X80);
  // set_matrix_full(255);
  // image(rgb_buffer);
}

uint32_t color_hsv(uint16_t hue, uint8_t sat, uint8_t val)
{

  uint8_t r, g, b;

  // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
  // 0 is not the start of pure red, but the midpoint...a few values above
  // zero and a few below 65536 all yield pure red (similarly, 32768 is the
  // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
  // each for red, green, blue) really only allows for 1530 distinct hues
  // (not 1536, more on that below), but the full unsigned 16-bit type was
  // chosen for hue so that one's code can easily handle a contiguous color
  // wheel by allowing hue to roll over in either direction.
  hue = (hue * 1530L + 32768) / 65536;
  // Because red is centered on the rollover point (the +32768 above,
  // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
  // where 0 and 1530 would yield the same thing. Rather than apply a
  // costly modulo operator, 1530 is handled as a special case below.

  // So you'd think that the color "hexcone" (the thing that ramps from
  // pure red, to pure yellow, to pure green and so forth back to red,
  // yielding six slices), and with each color component having 256
  // possible values (0-255), might have 1536 possible items (6*256),
  // but in reality there's 1530. This is because the last element in
  // each 256-element slice is equal to the first element of the next
  // slice, and keeping those in there this would create small
  // discontinuities in the color wheel. So the last element of each
  // slice is dropped...we regard only elements 0-254, with item 255
  // being picked up as element 0 of the next slice. Like this:
  // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
  // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
  // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
  // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
  // the constants below are not the multiples of 256 you might expect.

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if (hue < 510)
  { // Red to Green-1
    b = 0;
    if (hue < 255)
    { //   Red to Yellow-1
      r = 255;
      g = hue; //     g = 0 to 254
    }
    else
    {                //   Yellow to Green-1
      r = 510 - hue; //     r = 255 to 1
      g = 255;
    }
  }
  else if (hue < 1020)
  { // Green to Blue-1
    r = 0;
    if (hue < 765)
    { //   Green to Cyan-1
      g = 255;
      b = hue - 510; //     b = 0 to 254
    }
    else
    {                 //   Cyan to Blue-1
      g = 1020 - hue; //     g = 255 to 1
      b = 255;
    }
  }
  else if (hue < 1530)
  { // Blue to Red-1
    g = 0;
    if (hue < 1275)
    {                 //   Blue to Magenta-1
      r = hue - 1020; //     r = 0 to 254
      b = 255;
    }
    else
    { //   Magenta to Red-1
      r = 255;
      b = 1530 - hue; //     b = 255 to 1
    }
  }
  else
  { // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  uint16_t s1 = 1 + sat;  // 1 to 256; same reason
  uint8_t s2 = 255 - sat; // 255 to 0
  return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
         (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
         (((((b * s1) >> 8) + s2) * v1) >> 8);
}

uint32_t gamma32(uint32_t x)
{
  uint8_t *y = (uint8_t *)&x;
  // All four uint8_ts of a 32-bit value are filtered even if RGB (not WRGB),
  // to avoid a bunch of shifting and masking that would be necessary for
  // properly handling different endianisms (and each uint8_t is a fairly
  // trivial operation, so it might not even be wasting cycles vs a check
  // and branch for the RGB case). In theory this might cause trouble *if*
  // someone's storing information in the unused most significant uint8_t
  // of an RGB value, but this seems exceedingly rare and if it's
  // encountered in reality they can mask values going in or coming out.
  for (uint8_t i = 0; i < 4; i++)
    y[i] = gamma8(y[i]);
  return x; // Packed 32-bit return
}
