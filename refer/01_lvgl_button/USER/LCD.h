#ifndef __LCD_H__
#define __LCD_H__


#include "main.h"


#define XDP 126
#define YDP 294



#define	LCDPin_RS	   34   ///  DC  , P1-A2  , 1*32+2
#define	LCDPin_RST	   35   /// RES  , P1-A3 , 0*32+3
#define	LCDPin_CS	   19   ///  CS  , P0-C3 .0*32+8+8+3

extern int spi_fd;


extern uint16_t u16TxBuffer[1024*20];

int SPI_init();
void LCD_init();
void LCD_Simple_inition();
void LCD_WR_REG(uint8_t data);
void LCD_WR_DATA8(uint8_t data);
void LCD_WR_DATA(uint16_t data);


int spi_w(uint8_t*d,int l);
int spi_w_16(uint16_t* tx,int len);



void LCD_SPI_SetDisplayWindow(u16 add_sx, u16 add_ex, u16 add_sy, u16 add_ey);
void LCD_SPI_WriteRAM_Prepare();

void  showpic_a();
void tran(uint16_t num);
void tran_img();

void LCD_All_Color_16();
#endif

