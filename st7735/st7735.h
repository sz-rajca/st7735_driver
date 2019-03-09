#include <stm32f10x_spi.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#include "adafriut_st7735.h"
#include "adafriut_st77xx.h"

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP 0x2

#define ST7735_width 128
#define ST7735_height 160

#define LCDSPEED SPI_BaudRatePrescaler_2

#define LOW 0
#define HIGH 1

#define LCD_C LOW
#define LCD_D HIGH

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_MADCTL 0x36
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_COLMOD 0x3A

#define MADVAL(x) (((x) << 5) | 8)
#define LCD_PORT GPIOC
#define GPIO_PIN_DC GPIO_Pin_2 //PC2
#define GPIO_PIN_SCE GPIO_Pin_0 //PC0
#define GPIO_PIN_RST GPIO_Pin_1 //PC1
#define GPIO_PIN_BLK GPIO_Pin_1 //PA1
#define LCD_PORT_BLK GPIOA
#define SPILCD SPI2

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint8_t madctl);
void ST7735_pushColor(uint16_t *color, int cnt);
void ST7735_init();
void ST7735_backLight(uint8_t on);
void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint8_t madctl);
void printChar(char data[], uint16_t cnt, uint16_t color, uint16_t bgcolor);
void fillScreen(uint16_t color);

