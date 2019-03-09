#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include "spi.h"
#include "st7735.h"

void Delay(uint32_t nTime);
uint16_t mystrlen(const char *str);
char *myitoa(int i, char b[]);

int main(void)
{
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		while (1)
			;
	}
	spiInit(SPI2);
	ST7735_init();
	ST7735_backLight(0x00);
	fillScreen(0x001F);
	Delay(1000);
	while (1)
	{
		//char data[] = "ABCD efgh";
		int number = 1234567890;
		char data[20] = { 0 };
		myitoa(number, data);
		uint16_t len = mystrlen(data);
		printChar(data, len, 0x0000, 0x001F);
		Delay(2000);
	}
}

char *myitoa(int i, char b[])
{
	char const digit[] = "0123456789";
	char *p = b;
	if (i < 0)
	{
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do
	{ //Move to where representation ends
		++p;
		shifter = shifter / 10;
	} while (shifter);
	*p = '\0';
	do
	{ //Move back, inserting digits as u go
		*--p = digit[i % 10];
		i = i / 10;
	} while (i);
	return b;
}

uint16_t mystrlen(const char *str)
{
	const char *s;
	for (s = str; *s; ++s)
		;
	return (s - str);
}

static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime)
{
	TimingDelay = nTime;
	while (TimingDelay != 0)
		;
}

void SysTick_Handler(void)
{
	if (TimingDelay != 0x00)
		TimingDelay--;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	while(1);
}
#endif
