/*
 * oled.c
 *
 *  Created on: Sep 4, 2020
 *      Author: luca
 */

#include "oled.h"

#include "../utils.h"

#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

#define OLED_PIN_DC_HIGH()			GPIOA->BSRR = GPIO_BSRR_BS3
#define OLED_PIN_DC_LOW()			GPIOA->BSRR = GPIO_BSRR_BR3
#define OLED_PIN_RESET_HIGH()		GPIOB->BSRR = GPIO_BSRR_BS1
#define OLED_PIN_RESET_LOW()		GPIOB->BSRR = GPIO_BSRR_BR1

#define OLED_SEND_DATA()			OLED_PIN_DC_HIGH()
#define OLED_SEND_COMMAND()			OLED_PIN_DC_LOW()

/* Private in-line functions */
void OLED_InitHardware(OLED_data_t * data);
void OLED_initSPI1();
void OLED_SPI1Send(uint8_t data);
void OLED_WR_REG(uint8_t data);
void OLED_WR_DATA8(uint8_t data);
void OLED_WR_DATA(uint16_t data);
void OLED_WR_DATA_f(uint16_t data);
void OLED_initDMA2();

void OLED_InitScreen(OLED_data_t * data, OLED_orientation_t orientation){
	data->rendering_framebuffer = 0;

	OLED_InitHardware(data);

	/* Init sequence */
	OLED_PIN_RESET_LOW();
	__delay_ms(200);
	OLED_PIN_RESET_HIGH();
	__delay_ms(50);

	//************* Start Initial Sequence **********//
	OLED_WR_REG(0x11); //sleep out, boost on
	__delay_ms(50);

	OLED_WR_REG(0x21); //inversion on

	// Frame rate=fosc/((RTNX x 2 + 40) x (LINE + FPX + BPX +2))
	// fosc = 850kHz
	// FPX > 0, BPX > 0
	// where X = A(normal mode), B(idle mode), C(partial mode + dot inversion), D(partial mode + column inversion)

	OLED_WR_REG(0xB1); //Frame Rate Control 1: set frame rate in Normal Mode (full colors)
	OLED_WR_DATA8(0x01);  // 4 bits - RTNA: set one line period
	OLED_WR_DATA8(0x2C);  // 6 bits - Front Porch A
	OLED_WR_DATA8(0x2D);  // 6 bits - Back Porch A
	OLED_WR_REG(0xB3); //Frame Rate Control 3: set frame rate in Partial Mode (full colors)
	// firt 3 param: Dot Inversion Mode
	OLED_WR_DATA8(0x05);  // 4 bits - RTNC
	OLED_WR_DATA8(0x3A);  // 6 bits - Front Porch C
	OLED_WR_DATA8(0x3A);  // 6 bits - Front Porch C
	// last 3 param: Column Inversion Mode
	OLED_WR_DATA8(0x05);  // 4 bits - RTND
	OLED_WR_DATA8(0x3A);  // 6 bits - Front Porch D
	OLED_WR_DATA8(0x3A);  // 6 bits - Front Porch D

	OLED_WR_REG(0xB4); // Display Inversion mode control
	OLED_WR_DATA8(0x03); // NLA : 1 bit, NLB : 1 bit, NLC : 1 bit (A=Normal, B=Idel, C=Partial)
	// NLX: 0 -> Dot inversion, 1 -> Column inversion

	OLED_WR_REG(0xC0); // Power Control 1
	//AVDD: Analog Power Supply
	//GVDD: Gamma reference voltage
	//GVCL: ???
	OLED_WR_DATA8(0x62); // 7:6:5 -> AVDD sel (6.8v); 4:3:2:1:0 -> VRHP (GVDD) sel
	OLED_WR_DATA8(0x02); //                           4:3:2:1:0 -> VRLP (GVCL) sel
	OLED_WR_DATA8(0x04); // 7:6 -> MODE sel (x2); 2-> always one, 1-> VRHP 5; 0-> VRLP 5

	OLED_WR_REG(0xC1); // Power Control 2
	OLED_WR_DATA8(0xC0); // VGH25=2.4v, VGH=2*AVDD+VGH25-0.5, VGL=-7.5v

	OLED_WR_REG(0xC2); // Power Control 3
	//Set the amount of current in Operational amplifier in normal mode/full color
	OLED_WR_DATA8(0x0D); // AP: Large; APA: Small
	OLED_WR_DATA8(0x00); // Booster step-up Cycle = 1, 1/3, 1, 1, 1.

	OLED_WR_REG(0xC3); // Power Control 4
	//Same as before for idle mode
	OLED_WR_DATA8(0x8D); // AP: Large; APA: Small
	OLED_WR_DATA8(0x6A); // Booster step-up Cycle = 1/2, 1, 1/2, 1/2, 1/2

	OLED_WR_REG(0xC4); // Power Control 5
	//Same as before for partial mode
	OLED_WR_DATA8(0x8D); // AP: Large; APA: Small
	OLED_WR_DATA8(0xEE); // Booster step-up Cycle = 1/2, 1/2, 1/2, 1/4, 1/2

	OLED_WR_REG(0xC5);  // VCOM Control 1
	OLED_WR_DATA8(0x0E);  // VCOMS = -0.775v

	OLED_WR_REG(0xE0); // Gamma '+' Polarity Correction Characteristics Setting
	OLED_WR_DATA8(0x10);  // high level adj resistor
	OLED_WR_DATA8(0x0E);  // low level adj resistor
	OLED_WR_DATA8(0x02);
	OLED_WR_DATA8(0x03);
	OLED_WR_DATA8(0x0E);
	OLED_WR_DATA8(0x07);
	OLED_WR_DATA8(0x02);
	OLED_WR_DATA8(0x07);
	OLED_WR_DATA8(0x0A);
	OLED_WR_DATA8(0x12);
	OLED_WR_DATA8(0x27);
	OLED_WR_DATA8(0x37);
	OLED_WR_DATA8(0x00);
	OLED_WR_DATA8(0x0D);
	OLED_WR_DATA8(0x0E);
	OLED_WR_DATA8(0x10);

	OLED_WR_REG(0xE1); // Gamma '-' polarity Correction Characteristics Setting
	OLED_WR_DATA8(0x10);
	OLED_WR_DATA8(0x0E);
	OLED_WR_DATA8(0x03);
	OLED_WR_DATA8(0x03);
	OLED_WR_DATA8(0x0F);
	OLED_WR_DATA8(0x06);
	OLED_WR_DATA8(0x02);
	OLED_WR_DATA8(0x08);
	OLED_WR_DATA8(0x0A);
	OLED_WR_DATA8(0x13);
	OLED_WR_DATA8(0x26);
	OLED_WR_DATA8(0x36);
	OLED_WR_DATA8(0x00);
	OLED_WR_DATA8(0x0D);
	OLED_WR_DATA8(0x0E);


	OLED_WR_REG(0x3A); // Interface Pixel Format
	OLED_WR_DATA8(0x05); // 0b011 (3) 12-bit/pixel - 0b101 (5) 16-bit/pixel - 0b110 (6) 18-bit/pixel

	OLED_WR_REG(0x29); // Display On: This command is used to recover from DISPLAY OFF mode.

	OLED_RotateScreen(data, orientation);

	OLED_initDMA2(data);
}


void OLED_RotateScreen(OLED_data_t * data, OLED_orientation_t orientation){
	if(SPI1->CR1 & SPI_CR1_DFF){
		// Use 8-bit data format in the SPI
		while(SPI1->SR & SPI_SR_BSY);
		REG_CLEAR(SPI1->CR1, SPI_CR1_SPE);
		REG_CLEAR(SPI1->CR1, SPI_CR1_DFF);
		REG_SET(SPI1->CR1, SPI_CR1_SPE);
	}

	switch(orientation){
	case OLED_NORD:
	case OLED_SOUTH:
		data->width = OLED_NORD_WIDTH;
		data->height = OLED_NORD_HEIGHT;
		OLED_WR_REG(0x2a);// Column addres set (ADDR start) ... (ADDR end not given...?)
		OLED_WR_DATA(0+1);
		OLED_WR_DATA(OLED_NORD_WIDTH-1+1);
		OLED_WR_REG(0x2b);// Row address set (ADDR start) ... (ADDR end not given...?)
		OLED_WR_DATA(0+26);
		OLED_WR_DATA(OLED_NORD_HEIGHT-1+26);
		break;
	case OLED_WEST:
	case OLED_EAST:
		OLED_WR_REG(0x2a);// Column addres set (ADDR start) ... (ADDR end not given...?)
		OLED_WR_DATA(0+26);
		OLED_WR_DATA(OLED_NORD_HEIGHT-1+26);
		OLED_WR_REG(0x2b);// Row address set (ADDR start) ... (ADDR end not given...?)
		OLED_WR_DATA(0+1);
		OLED_WR_DATA(OLED_NORD_WIDTH-1+1);
		data->height = OLED_NORD_WIDTH;
		data->width = OLED_NORD_HEIGHT;
		break;
	}

	OLED_WR_REG(0x36); // Memory Data Access Control: This command defines read/ write scanning direction of frame memory
		//Content: 7:MX, 6:MY, 5:MV, 4:ML, 3:RGB, 2:MH.
		//         \________________/ -> microcontroller memory write direction
		//                           \___/        \___/ -> screen refresh direction (line and column)
		//                                  \__/ -> 0 = RBG, 1 = BGR
		// MX = 1 -> invert LINEs addresses in R/W commands
		// MY = 1 -> inver COLUMNs addresses in R/W commands
		// MV = 1 -> swap LINEs and COLUMNs in R/W commands

	switch(orientation){
	case OLED_NORD:
		OLED_WR_DATA8(0b01101000);
		break;
	case OLED_SOUTH:
		OLED_WR_DATA8(0b10101000);
		break;
	case OLED_WEST:
		OLED_WR_DATA8(0b00001000);
		break;
	case OLED_EAST:
		OLED_WR_DATA8(0b11001000);
		break;
	}

	data->orientation = orientation;

	OLED_WR_REG(0x2c);// Write memory

	//Wait for CMD to set Data mode
	while((SPI1->SR & SPI_SR_BSY_Msk) != 0);
	OLED_SEND_DATA();
}

inline void OLED_InitHardware(OLED_data_t * data){
	/* GPIOA
	 *  * SDA, SCL, CS, DC, RES, BLK.
	 *     |    |   |   |    |    |
	 *    PA7  PA5 PA4 PA3  PB1  PA6
	 * */

	/* Configure DC and RES output pins */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

	// General output (0b01)
	REG_SETV(GPIOA->MODER, GPIO_MODER_MODE3, GPIO_MODER_OUTPUT);
	REG_SETV(GPIOB->MODER, GPIO_MODER_MODE1, GPIO_MODER_OUTPUT);

	// All push-pull
	REG_SETV(GPIOA->OTYPER, GPIO_OTYPER_OT3, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOB->OTYPER, GPIO_OTYPER_OT1, GPIO_OTYPER_PUSHPULL);

	// No pull up or pull down
	REG_SETV(GPIOA->PUPDR, GPIO_PUPDR_PUPD3, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOB->PUPDR, GPIO_PUPDR_PUPD1, GPIO_PUPDR_NOPULL);

	// Max speed
	REG_SETV(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED3, 0b11);
	REG_SETV(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED1, 0b11);

	// Reset and D/C HIGH
	GPIOA->BSRR = GPIO_BSRR_BS3;
	GPIOB->BSRR = GPIO_BSRR_BS1;

	/*
	 * Configure PWM backlight control
	 * PA6 - TIM3_CH1
	 * APB1 -> 45Mhz
	 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 = 0;
	TIM3->CR2 = 0;
	TIM3->SMCR = 0;
	TIM3->PSC = 45; // timer freq is 2Mhz
	TIM3->ARR = OLED_BACKLIGHT_MAX - 1; // count from zero to 255
	TIM3->CCER = 0;

	OLED_SetBacklight(data, OLED_BACKLIGHT_MAX);

	/* Output mode, using PWM 1 mode */
	TIM3->CCMR1 = 0b00 << TIM_CCMR1_CC1S_Pos | 0b110 << TIM_CCMR1_OC1M_Pos;
	TIM3->CCER |= TIM_CCER_CC1E;

	//PA6, alternate function 2: TIM3_CH1
	REG_SETV(GPIOA->MODER, GPIO_MODER_MODE6, GPIO_MODER_ALTERNATE);
	REG_SETV(GPIOA->AFR[0], GPIO_AFRL_AFSEL6, 2);
	REG_SETV(GPIOA->OTYPER, GPIO_OTYPER_OT6, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOA->PUPDR, GPIO_PUPDR_PUPD6, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED6, 0b11);

	// Start timer
	TIM3->CR1 |= TIM_CR1_CEN;

	/*
	 * Configure SPI1
	 */
	OLED_initSPI1();
}

inline void OLED_initSPI1(){
	/*
	 *    MOSI (SDA) -> PA7
	 *    CLK  (SCL) -> PA5
	 *    NSS  (CS)  -> PA4
	 * */

	// All push-pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4_Msk | GPIO_OTYPER_OT5_Msk | GPIO_OTYPER_OT7_Msk);
//	GPIOA->OTYPER |= (GPIO_OTYPER_OT4_Msk | GPIO_OTYPER_OT5_Msk | GPIO_OTYPER_OT7_Msk);

	// No pull up or pull down
	REG_SETV(GPIOA->PUPDR, GPIO_PUPDR_PUPD4, 0b00);
	REG_SETV(GPIOA->PUPDR, GPIO_PUPDR_PUPD5, 0b00);
	REG_SETV(GPIOA->PUPDR, GPIO_PUPDR_PUPD7, 0b00);

	// Max speed
	REG_SETV(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED4, 0b11);
	REG_SETV(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5, 0b11);
	REG_SETV(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED7, 0b11);

	// Enable SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN_Msk;

	// SPI: Clock 1 when idle, sampling on rising edge, using MISO as INPUT and OUTPUT, BAUD = 90Mhz / 2^(BR+1)
	SPI1->CR1 = 1 << SPI_CR1_CPHA_Pos | 1 << SPI_CR1_CPOL_Pos | 1 << SPI_CR1_MSTR_Pos |
			OLED_SPI_BR << SPI_CR1_BR_Pos | 0 << SPI_CR1_LSBFIRST_Pos |
			0 << SPI_CR1_DFF_Pos | 0 <<  SPI_CR1_CRCEN_Pos | 1 << SPI_CR1_BIDIOE_Pos |
			1 << SPI_CR1_BIDIMODE_Pos;

	// Enable CS pin and use Motorola frame format
	SPI1->CR2 = 1 << SPI_CR2_SSOE_Pos | 0 << SPI_CR2_FRF_Pos;

	// Alternate function 5 on PIN 4, 5, 7
	REG_SETV(GPIOA->AFR[0], GPIO_AFRL_AFSEL4, 5);
	REG_SETV(GPIOA->AFR[0], GPIO_AFRL_AFSEL5, 5);
	REG_SETV(GPIOA->AFR[0], GPIO_AFRL_AFSEL7, 5);

	// Alternate function (0b10)
	REG_SETV(GPIOA->MODER, GPIO_MODER_MODE4, 0b10);
	REG_SETV(GPIOA->MODER, GPIO_MODER_MODE5, 0b10);
	REG_SETV(GPIOA->MODER, GPIO_MODER_MODE7, 0b10);

	//Finally, enable SPI1
	SPI1->CR1 |= SPI_CR1_SPE;
}

inline void OLED_SPI1Send(uint8_t data){
	// Wait for the TX buffer to be empty
	while((SPI1->SR & SPI_SR_TXE_Msk) == 0);
	while((SPI1->SR & SPI_SR_BSY_Msk) != 0);
	SPI1->DR = data;
}

inline void OLED_WR_REG(uint8_t data){
	OLED_SPI1Send(data);
	OLED_SEND_COMMAND();
}

inline void OLED_WR_DATA8(uint8_t data){
	OLED_SPI1Send(data);
	OLED_SEND_DATA();
}

inline void OLED_WR_DATA(uint16_t data){
	OLED_SPI1Send(data >> 8);
	OLED_SEND_DATA();
	OLED_SPI1Send(data & 0xFF);
}

inline void OLED_WR_DATA_f(uint16_t data){
	while((SPI1->SR & SPI_SR_TXE_Msk) == 0);
	SPI1->DR =(data >> 8);
	while((SPI1->SR & SPI_SR_TXE_Msk) == 0);
	SPI1->DR =(data & 0xFF);
}

inline void OLED_initDMA2(OLED_data_t * data){
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	// Disable the Stream
	DMA2_Stream5->CR = 0;
	while((DMA2_Stream5->CR & DMA_SxCR_EN) != 0);

	/**
	 * Direction: PtoM, Increment memory.
	 * MEMORY BURST = 8 (FIFO FULL THRES)
	 */
	DMA2_Stream5->CR = 0b01 << DMA_SxCR_DIR_Pos | 0 << DMA_SxCR_PINC_Pos | 1 << DMA_SxCR_MINC_Pos |
			0b01 << DMA_SxCR_PSIZE_Pos | 0b01 << DMA_SxCR_MSIZE_Pos | 0b10 << DMA_SxCR_PL_Pos |
			0b01 << DMA_SxCR_MBURST_Pos | 3 << DMA_SxCR_CHSEL_Pos;

	// Using the FIFO with the FULL threshold
	DMA2_Stream5->FCR = DMA_SxFCR_DMDIS | 0b11 << DMA_SxFCR_FTH_Pos;

	DMA2_Stream5->PAR = (uint32_t) &(SPI1->DR);

	SPI1->CR2 |= SPI_CR2_TXDMAEN;
}

void OLED_SwapBuffers(OLED_data_t * data){

	/* Wait for DMA */
	while((DMA2_Stream5->CR & DMA_SxCR_EN) != 0);

	// Clear all the flags of the previous transfer
	DMA2->HIFCR = DMA_HIFCR_CFEIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTCIF5;

	/*
	 * Draw rendering buffer
	 */
	DMA2_Stream5->M0AR = (uint32_t)  &(data->framebuffer[data->rendering_framebuffer]);
	DMA2_Stream5->NDTR = OLED_PIXELS;

	// Swap
	data->rendering_framebuffer ^= 1;

	// Wait for SPI1 to have finished current transaction
	while(SPI1->SR & SPI_SR_BSY);

	if((SPI1->CR1 & SPI_CR1_DFF) == 0){
		// Use 16-bit data format in the SPI
		REG_CLEAR(SPI1->CR1, SPI_CR1_SPE);
		REG_SET(SPI1->CR1, SPI_CR1_DFF);
		REG_SET(SPI1->CR1, SPI_CR1_SPE);
	}

	// DMA kicks in
	DMA2_Stream5->CR |= DMA_SxCR_EN;
}

void OLED_Clear(OLED_data_t * data, uint16_t color){
	for(uint_fast16_t i = 0; i < OLED_PIXELS; i++)
		data->framebuffer[data->rendering_framebuffer][i] = color;
}

void OLED_SetBacklight(OLED_data_t * screen, int16_t backlight){
	if(backlight < OLED_BACKLIGHT_MIN)
		backlight = OLED_BACKLIGHT_MIN;
	else if(backlight > OLED_BACKLIGHT_MAX)
		backlight = OLED_BACKLIGHT_MAX;

	screen->backlight = backlight;
	TIM3->CCR1 = screen->backlight;
}

