enum spiSpeed
{
	SPI_SLOW, SPI_MEDIUM, SPI_FAST
};

void spiInit(SPI_TypeDef *SPIx);
int spiReadWrite(SPI_TypeDef *SPIx, uint8_t *rbuf, const uint8_t *tbuf, int cnt,
		enum spiSpeed speed);
int spiReadWrite16(SPI_TypeDef *SPIx, uint16_t *rbuf, const uint16_t *tbuf,
		int cnt, enum spiSpeed speed);

enum eepromCMD
{
	cmdREAD = 0x03,
	cmdWRITE = 0x02,
	cmdWREN = 0x06,
	cmdWRDI = 0x04,
	cmdRDSR = 0x05,
	cmdWRSR = 0x01
};
