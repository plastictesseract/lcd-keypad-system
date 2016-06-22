/**
 * Author: Michael Rosa
 * Date: June 2016
 */

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"


uint8_t lcd_buffer[LCD_LINES][LCD_DISP_LENGTH];


void lcd_buffer_init(void)
{
  memset(lcd_buffer, 254, sizeof(lcd_buffer));
}

void lcd_buffer_clr(void)
{
  lcd_buffer_init();
}

void lcd_buffer_writec(const uint8_t x, uint8_t y, const uint8_t c)
{
  if(x < LCD_DISP_LENGTH && y < LCD_LINES)
  {
    if(y == 1) y = 2;        // to account for memory of LCD
    else if (y == 2) y = 1;
    
    lcd_buffer[y][x] = c;
  }
}

void lcd_buffer_writes(const uint8_t x, const uint8_t y, const char *s)
{
  uint8_t length = strlen(s);
  
  if(x < LCD_DISP_LENGTH && y < LCD_LINES)
  {
    if(x + length < LCD_DISP_LENGTH)
    {
      for(uint8_t i = 0; i < length; i++)
        lcd_buffer_writec(x + i, y, *(s + i));
      
      //strcpy(*(lcd_buffer[y] + x), s);
    }
    else
    {   
      const uint8_t pos = x + y * LCD_DISP_LENGTH;
      for(uint8_t i = 0; i < length && i + pos < LCD_LINES * LCD_DISP_LENGTH; i++)
      {
        uint8_t y_from_pos = (i + pos) / 10 * 10 / LCD_DISP_LENGTH;
        uint8_t x_from_pos = (i + pos) - y_from_pos * LCD_DISP_LENGTH;
        
        lcd_buffer_writec(x_from_pos, y_from_pos, *(s + i));
      }
    }
  }
}

void lcd_update(void)
{
  uint8_t x, y;
  
  lcd_clrscr();
  for(y = 0; y < LCD_LINES; y++)
  {
    for(x = 0; x < LCD_DISP_LENGTH; x++)
    {
      lcd_putc(lcd_buffer[y][x]);
    }
  }
}


int main(void)
{
  lcd_init(LCD_DISP_ON);
  lcd_buffer_init();
  
  lcd_buffer_writes(0, 0, "Hello world");
  lcd_update();
  
  while(1)
  {
  }
  
  return 0;
}