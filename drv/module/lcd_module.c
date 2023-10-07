#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include "lcd_module.h"


#define SET_SCLK()      PIN_SET(GPIOA7)
#define CLK_SCLK()      PIN_CLEAR(GPIOA7)
#define SET_DATA()      PIN_SET(GPIOA9)
#define CLK_DATA()      PIN_CLEAR(GPIOA9)

#define INC_ADDR        0x40
#define NOT_INC_ADDR    0x44

union eightsegment eightsegment_code[EIGHT_SEGMENT_LENGTH] = {0};
uint8_t table[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

uint16_t l_data = 0;
uint16_t r_data = 999;

void gpio_aip16400_init(void);

void aip1640_start(void)
{
    SET_SCLK();
    SET_DATA();
    SysTick_DelayUS(2);
    CLK_DATA();
    SysTick_DelayUS(2);
    CLK_SCLK();
}

void aip1640_stop(void)
{
    CLK_SCLK();
    CLK_DATA();
    SysTick_DelayUS(2);
    SET_SCLK();
    SysTick_DelayUS(2);
    SET_DATA();
}

void aip1640_send_byte(uint8_t data)
{
    int i;
    for(i=0; i<8; i++)
    {
        CLK_SCLK();
        if(data & 0x01)
        {
            SET_DATA();
        }
        else
        {
            CLK_DATA();
        }
        SET_SCLK();
        SysTick_DelayUS(2);
        data = data >> 1;
    }
}

void aip1640_write(uint8_t data)
{
    aip1640_start();
    aip1640_send_byte(data);
    aip1640_stop();
}

void aip1640_init(void)
{
    gpio_aip16400_init();
    aip1640_write(INC_ADDR);
    aip1640_write(LIGHT_4_16);
}

void aip1640_write_array(uint8_t *inputdate)
{
    int i;
    aip1640_start();
    aip1640_send_byte(DATA_ADDR);
    for(i=0; i<8;  i++)
    {
       aip1640_send_byte(*inputdate++);
    }
    aip1640_stop();
}

void show_data(uint16_t l_data, uint16_t r_data)
{
    eightsegment_code[SEG1].uint8_t_led = table[l_data];
    // eightsegment_code[SEG2].uint8_t_led = table[l_data/100%10];
    // eightsegment_code[SEG3].uint8_t_led = table[l_data%100/10];
    // eightsegment_code[SEG4].uint8_t_led = table[l_data%100%10];

    // eightsegment_code[SEG5].uint8_t_led = table[r_data/100/10];
    // eightsegment_code[SEG6].uint8_t_led = table[r_data/100%10];
    // eightsegment_code[SEG7].uint8_t_led = table[r_data%100/10];
    // eightsegment_code[SEG8].uint8_t_led = table[r_data%100%10];
}

void aip1640_test(void)
{
    int i=0;
		aip1640_init();
    while (1)
    {
       show_data(i++,0);
       //aip1640_write_array((uint8_t *)eightsegment_code);
			aip1640_write_array((uint8_t *)0x3f);
			aip1640_write_array((uint8_t *)0x06);
			aip1640_write_array((uint8_t *)0x5b);
			aip1640_write_array((uint8_t *)0x4f);
			aip1640_write_array((uint8_t *)0x66);
			aip1640_write_array((uint8_t *)0x6d);
			aip1640_write_array((uint8_t *)0x7d);
      SysTick_DelayMS(1000);
    }
}
