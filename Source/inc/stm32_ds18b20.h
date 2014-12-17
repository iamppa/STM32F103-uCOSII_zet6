#ifndef _DS18B20_H
#define _DS18B20_H
extern u8 flag;
void us_delay(u16 us);
void ms_delay(u16 count);
//延时子函数
void ds18b20_reset(void);
u8 ds18b20_read_byte(void);
void ds18b20_write_byte(u8 dat);
void temperature_convert(void);
u16 get_temperature(void);
void initialize_ds18b20(void);
void GPIO_Configuration1(void);
void GPIO_Configuration2(void);
#endif
