#include "st7735.h"
#include "font.h"

static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

static void LcdWrite(char dc, const char *data, int nbytes);
static void LcdWrite16(char dc, const uint16_t *data, int cnt);
static void ST7735_writeCmd(uint8_t c);

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
		uint8_t madctl)
{
	madctl = MADVAL(madctl);
	if (madctl != madctlcurrent)
	{
		ST7735_writeCmd(ST7735_MADCTL);
		LcdWrite(LCD_D, &madctl, 1);
		madctlcurrent = madctl;
	}
	ST7735_writeCmd(ST7735_CASET);
	LcdWrite16(LCD_D, &x0, 1);
	LcdWrite16(LCD_D, &x1, 1);

	ST7735_writeCmd(ST7735_RASET);
	LcdWrite16(LCD_D, &y0, 1);
	LcdWrite16(LCD_D, &y1, 1);

	ST7735_writeCmd(ST7735_RAMWR);
}

void ST7735_pushColor(uint16_t *color, int cnt)
{
	LcdWrite16(LCD_D, color, cnt);
}

void ST7735_backLight(uint8_t on)
{
	if (on)
	{
		GPIO_WriteBit(LCD_PORT_BLK, GPIO_PIN_BLK, LOW);
	}
	else
	{
		GPIO_WriteBit(LCD_PORT_BLK, GPIO_PIN_BLK, HIGH);
	}
}

void fillScreen(uint16_t color)
{
	uint8_t x, y;
	ST7735_setAddrWindow(0, 0, ST7735_width - 1, ST7735_height - 1,
	MADCTLGRAPHICS);
	for (x = 0; x < ST7735_width; x++)
	{
		for (y = 0; y < ST7735_height; y++)
		{
			ST7735_pushColor(&color, 1);
		}
	}
}

static void LcdWrite(char dc, const char *data, int nbytes)
{
	GPIO_WriteBit(LCD_PORT, GPIO_PIN_DC, dc);
	GPIO_ResetBits(LCD_PORT, GPIO_PIN_SCE);
	spiReadWrite(SPILCD, 0, data, nbytes, LCDSPEED);
	GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
}

static void LcdWrite16(char dc, const uint16_t *data, int cnt)
{
	GPIO_WriteBit(LCD_PORT, GPIO_PIN_DC, dc);
	GPIO_ResetBits(LCD_PORT, GPIO_PIN_SCE);
	spiReadWrite16(SPILCD, 0, data, cnt, LCDSPEED);
	GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
}

static void ST7735_writeCmd(uint8_t c)
{
	LcdWrite(LCD_C, &c, 1);
}

struct ST7735_cmdBuf
{
	uint8_t command;
	uint8_t delay;
	uint8_t len;
	uint8_t data[16];
};

static const struct ST7735_cmdBuf initializers[] =
{
//SWRESET Software Reset
		{ ST77XX_SWRESET, 150, 0, 0 },
		//SPLOUT Leave sleep mode
		{ ST77XX_SLPOUT, 150, 0, 0 },
		//FRMCTR1, FRMCTR2 Frame Rate configuration -- Normal mode, idle
		// frame rate = focs / (1 x 2 + 40) * (LINE + 2C + 2D)
		{ ST7735_FRMCTR1, 0, 3,
		{ 0x01, 0x2C, 0x2D } },
		{ ST7735_FRMCTR2, 0, 3,
		{ 0x01, 0x2C, 0x2D } },
		//FRCTR3 Frame Rate configuraion -- partial mode
		{ ST7735_FRMCTR3, 0, 6,
		{ 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D } },
		//INVCTR Display inversion (no inversion)
		{ ST7735_INVCTR, 0, 1,
		{ 0x07 } },
		// PWCTR1 Power control -4.6V, Auto mode
		{ ST7735_PWCTR1, 0, 3,
		{ 0xA2, 0x02, 0x84 } },
		// PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
		{ ST7735_PWCTR2, 0, 1,
		{ 0xC5 } },
		// PWCTR3 Power control, opamp current smal, boost frequency
		{ ST7735_PWCTR3, 0, 2,
		{ 0x0A, 0x00 } },
		// PWCTR4 Power control, BLK/2, opamp current small and medium low
		{ ST7735_PWCTR4, 0, 2,
		{ 0x8A, 0x2A } },
		// PWRCTR5, VMCTR1 Power control
		{ ST7735_PWCTR5, 0, 2,
		{ 0x8A, 0xEE } },
		{ ST7735_VMCTR1, 0, 1,
		{ 0x0E } },
		// INVOFF Don't invert display
		{ ST77XX_INVOFF, 0, 0, 0 },
		// Memory access directions. row address/col address, bottom to top refesh (10.1.27)
		{ ST77XX_MADCTL, 0, 1,
		{ 0xC8 } /*{MADVAL(MADCTLGRAPHICS)}*/},
		// Color mode 16 bit (10.1.30
		{ ST77XX_COLMOD, 0, 1,
		{ 0x05 } },
		// Column address set 0..127
		{ ST77XX_CASET, 0, 4,
		{ 0x00, 0x02 /*0x00*/, 0x00, 0x7F } },
		// Row address set 0..159
		{ ST77XX_RASET, 0, 4,
		{ 0x00, 0x00, 0x00, 0x9F } },
		// GMCTRP1 Gamma correction
		{ ST7735_GMCTRP1, 0, 16,
		{ 0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B,
				0x39, 0x00, 0x01, 0x03, 0x10 } },
		// GMCTRP2 Gamma Polarity corrction
		{ ST7735_GMCTRN1, 0, 16,
		{ 0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37,
				0x3F, 0x00, 0x00, 0x02, 0x10 } },
		{ ST77XX_NORON, 10, 0, 0 },
		// NORON Normal on
		{ ST77XX_DISPON, 100, 0, 0 },
		// DISPON Display on
		/*
		 { 0x13, 10, 0, 0},
		 */
		// End
		{ 0, 0, 0, 0 } };

void ST7735_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	const struct ST7735_cmdBuf *cmd;
	GPIO_StructInit(&GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,
			ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    //BLK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;    //RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;    //DC
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;    //LCD Select
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;    //SD Select
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOC, GPIO_Pin_6, HIGH);

	//set cs, reset low

	GPIO_WriteBit(LCD_PORT, GPIO_PIN_SCE, HIGH);
	GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, HIGH);
	Delay(10);
	GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, LOW);
	Delay(10);
	GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, HIGH);
	Delay(10);

	//Send initialization commands to ST7735

	for (cmd = initializers; cmd->command; cmd++)
	{
		LcdWrite(LCD_C, &(cmd->command), 1);
		if (cmd->len)
		{
			LcdWrite(LCD_D, cmd->data, cmd->len);
		}
		if (cmd->delay)
		{
			Delay(cmd->delay);
		}
	}
}

void printChar(char data[], uint16_t cnt, uint16_t color, uint16_t bgcolor)
{
	const char *ptr;
	uint8_t x, y;
	char mask = 0x00;
	uint16_t x0 = 0x0001;
	uint16_t y0 = 0x0001;
	uint16_t x1 = 0x0005;
	uint16_t y1 = 0x0008;
	uint16_t i = 0x0000;
	for (i = 0x0000; i < cnt; i++)
	{
		ST7735_setAddrWindow(x0, y0, x1, y1, 0x6);
		mask = 0x01;
		for (x = 0; x < 0x08; x++)
		{
			ptr = &font[(data[i] - 32) * 5];
			for (y = 0; y < 0x05; y++)
			{
				if (mask & (*ptr))
				{
					ST7735_pushColor(&color, 1);
				}
				else
				{
					ST7735_pushColor(&bgcolor, 1);
				}
				ptr++;
			}
			mask = mask << 1;
		}
		x0 += 0x0006;
		x1 += 0x0006;
		if (x1 >= ST7735_width)
		{
			x0 = 0x0001;
			x1 = 0x0005;
			y0 += 0x0008;
			y1 += 0x0008;
		}
		if (y1 >= ST7735_height)
		{
			y0 = 0x0001;
			y1 = 0x0008;
		}
	}
}
