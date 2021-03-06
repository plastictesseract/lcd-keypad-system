/**
 * Author: Michael Rosa
 * Date: June 2016
 */

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

#define  LCD_BUFFER_SIZE    LCD_LINES * LCD_DISP_LENGTH
#define  KEY_PORT           PORTB
#define  KEY_PIN            PINB
#define  KEY_DDR            DDRB

#define  COLS               0x70
#define  ROWS               0x0F


static char lcd_buffer[LCD_BUFFER_SIZE];
static uint8_t cursor;


static void lcd_update(void)
{
  char line[LCD_DISP_LENGTH + 1];
  
  lcd_clrscr();
  for(uint8_t i = 0; i < LCD_LINES; i++)
  {
    // accounting for how LCD memory is set up
    if(i == 1)
      memcpy(line, &lcd_buffer[(i + 1) * LCD_DISP_LENGTH], LCD_DISP_LENGTH);
    else if(i == 2)
      memcpy(line, &lcd_buffer[(i - 1) * LCD_DISP_LENGTH], LCD_DISP_LENGTH);
    else
      memcpy(line, &lcd_buffer[i * LCD_DISP_LENGTH], LCD_DISP_LENGTH);
    
    line[LCD_DISP_LENGTH] = '\0';
    
    lcd_puts(line);
  }
}

void lcd_buffer_clrscr(void)
{
  memset(lcd_buffer, 254, LCD_BUFFER_SIZE);   // extended ASCII blank 
  cursor = 0;
  //lcd_update();
}

void lcd_buffer_init(void)
{
  lcd_buffer_clrscr();
}

void lcd_buffer_gotoxy(const uint8_t x, const uint8_t y)
{
  if(x < LCD_DISP_LENGTH && y < LCD_LINES)
    cursor = y * LCD_DISP_LENGTH + x;
}

void lcd_buffer_putc(const char c)
{
  lcd_buffer[cursor] = c;
  if(++cursor == LCD_BUFFER_SIZE) cursor = 0;
  
  //lcd_update();
}

void lcd_buffer_puts(const char *s)
{
  if(cursor + strlen(s) < LCD_BUFFER_SIZE)        // if not on the last line
    strncpy(lcd_buffer + cursor, s, strlen(s));   // write as normal w/o null byte
  else
  {                                               // overflow from last line to first
    strncpy(lcd_buffer + cursor, s, LCD_BUFFER_SIZE - cursor);   
    strncpy(lcd_buffer, &s[LCD_BUFFER_SIZE - cursor], strlen(s) - LCD_BUFFER_SIZE + cursor);
  } 
  
  cursor += strlen(s);
  
  if(cursor >= LCD_BUFFER_SIZE)
    cursor %= LCD_BUFFER_SIZE;
  
  //lcd_update();
}

char key_from_rc(uint8_t r, uint8_t c)
{
  switch(r)
  {
    case 0:
      switch(c)
      {
        case 0:
          return '1';
          
        case 1:
          return '2';
          
        case 2:
          return '3';
      }
    
    case 1:
      switch(c)
      {
        case 0:
          return '4';
          
        case 1:
          return '5';
          
        case 2:
          return '6';
      }
    
    case 2:
      switch(c)
      {
        case 0:
          return '7';
          
        case 1:
          return '8';
          
        case 2:
          return '0';
      }
      
    case 3:
      switch(c)
      {
        case 0:
          return '*';
          
        case 1:
          return '#';
          
        case 2:
          return '9';
      }
  }
}

uint8_t get_key_pressed(void)
{
  uint8_t r, c;
  
  for(c = 0; c < 3; c++)
  {
    KEY_DDR &= ~(COLS);         // set cols as inputs for z-state
    KEY_DDR |=  0x10 << c;    // bit shift to set one pin as output (low)
    
    for(r = 0; r < 4; r++)
    {
      // if the pin is being pulled low
      if(!(KEY_PIN & (0x01 << r)))
      { 
        _delay_ms(20);    // delay for debounce
        if(!(KEY_PIN & (0x01 << r)))  // still pressed after debounce
        {
          return key_from_rc(r, c);
        }
      }
    }
  }
  
  return 0xFF;  // no button pressed
}


int main(void)
{
  lcd_init(LCD_DISP_ON);
  lcd_buffer_init();
  
  KEY_DDR  &= ~(ROWS);
  KEY_PORT |=   ROWS;
  
  char key;
  while(1)
  {
    key = get_key_pressed();
    if(key != 0xFF)
    {
      lcd_buffer_putc(key);
      lcd_update();
      while(get_key_pressed() == key){}
    }
  }
  
  return 0;
}