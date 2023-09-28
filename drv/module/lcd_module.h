#ifndef __LCD_MOUDLE_H
#define __LCD_MOUDLE_H

#include <common.h>
//#include <stdint.h>

#define LIGHT_1_16      0X88+0
#define LIGHT_2_16      0X88+1
#define LIGHT_4_16      0X88+2
#define LIGHT_10_16     0X88+3
#define LIGHT_11_16     0X88+4
#define LIGHT_12_16     0X88+5
#define LIGHT_13_16     0X88+6
#define LIGHT_14_16     0X88+7
#define LIGHT_ON        0X88
#define LIGHT_OFF       0X80

#define SEG0            0
#define SEG1            1
#define SEG2            2
#define SEG3            3
#define SEG4            4
#define SEG5            5
#define SEG6            6
#define SEG7            7
#define SEG8            8
#define SEG9            9
#define SEG10           10
#define SEG11           11
#define SEG12           12
#define SEG13           13
#define SEG14           14
#define SEG15           15
#define SEG16           16

#define DATA_ADDR       0x01
#define EIGHT_SEGMENT_LENGTH    8



union eightsegment
{
    uint8_t uint8_t_led;

    struct
    {
        uint8_t A:1;
        uint8_t B:1;
        uint8_t C:1;
        uint8_t D:1;
        uint8_t E:1;
        uint8_t F:1;
        uint8_t G:1;
        uint8_t H:1;
    }bit;

    struct
    {
        uint8_t ABCDEFG:7;
        uint8_t DP:1;
    }bits;
};



#endif
