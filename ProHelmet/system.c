/*
 * system.c
 *
 *  Created on: Sep 9, 2020
 *      Author: luca
 */

#include "system.h"

/*
 * 2^32 / (24 * 60 * 60 * 1000) ~ overflow every 50 days
 */
volatile time_ms_t SYS_time_millis = 0;
void (*SYS_SysTimer_callback)() = 0;

/*Side buttons*/
static volatile int32_t but2 = 0, but3 = 0, but4 = 0;
static volatile uint8_t but2_stat = 0, but3_stat = 0, but4_stat = 0;

/*
 * Setup system: I and D chaches,
 * SysClock to 180 Mhz, AHB = 180 Mhz, APB2 = 90 Mhz, APB1 = 45 Mhz.
 * */
void SYS_InitSys(){
	//Disable interrupts
	__set_PRIMASK(1);

	//Setup FLASH controller: enable caches, prefetch, and insert 0 wait states in the prefetch buffer (= 1 cpu cycles)
	// FLASH memory is clocked by HCLK, used by AHB bus.
	FLASH->ACR = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | (0 << FLASH_ACR_LATENCY_Pos);

	/* HSE Crystal is 8Mhz */

	//Turn on HSE oscillator and wait for it to stabilize (6 clk)
	RCC->CR |= RCC_CR_HSEON;
	while((RCC->CR & RCC_CR_HSERDY) == 0);

	//Now setup the main PLL
	// - PLLM division factor set to 4 -> HSE/PLLM = 8 Mhz / 4 = 2 Mhz at VCO input
	// - PLLN multiplication factor for VCO set to 180 -> 2 Mhz * 180 = 360 Mhz at VCO output
	// - PLLP division factor for Sys Clock set to 2 -> 360 Mhz / 2 = 180 Mhz system clock
	// - PLLSRC set to HSE
	// - PLLQ division factor for USB/SDIO set to 8 -> 360 Mhz / 8 = 45 Mhz (USB requires 48 Mhz!)
	RCC->PLLCFGR = (4 << RCC_PLLCFGR_PLLM_Pos) | (180 << RCC_PLLCFGR_PLLN_Pos)
			| (0b00 << RCC_PLLCFGR_PLLP_Pos) | RCC_PLLCFGR_PLLSRC_HSE | (8 << RCC_PLLCFGR_PLLQ_Pos);

	//Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	//Enable clock to PWR module on low speed APB (APB1)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	//In the mean time setup the Over Drive mode of the power unit!
	PWR->CR = PWR_CR_ODEN | (0b10 << PWR_CR_VOS_Pos); /* Set voltage regulator to mode 1 */
	while((PWR->CSR & PWR_CSR_VOSRDY) == 0);
	while((PWR->CSR & PWR_CSR_ODRDY) == 0);

	PWR->CR |= PWR_CR_ODSWEN;
	while((PWR->CSR & PWR_CSR_ODSWRDY) == 0);

	//wait for PLL
	while((RCC->CR & RCC_CR_PLLRDY) == 0);

	//Before changing main Sys clock, insert 5 wait states in the prefetch buffer (= 6 cpu cycles)
	FLASH->ACR |= (5 << FLASH_ACR_LATENCY_Pos);

	//Setting up clocks configurations:
	// - SW: sys sel clock switch to PLL
	// - HPRE: AHB prescaler to 1 -> 180 Mhz
	// - PPRE1: APB1 prescaler (max 45Mhz) to 4 -> 45 Mhz
	// - PPRE2: APB2 prescaler (max 95Mhz) to 2 -> 90 Mhz
	// - RTCPRES: RTC Clock = HSE / 8 -> 1 Mhz
	RCC->CFGR = RCC_CFGR_SW_PLL | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2
			| (8 << RCC_CFGR_RTCPRE_Pos);

	//Wait for the clock switch-
	while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

	//Set flash VTOR??? no... è gia messa perchè chiamata prima di jumpare al main + controllare linker script!!!!!

	//Enable interrupts again
	__set_PRIMASK(0);
}

void SYS_InitGreenLed(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;

	//led on PG13
	REG_SETV(GPIOG->MODER, GPIO_MODER_MODER13, GPIO_MODER_OUTPUT);
	REG_SETV(GPIOG->OTYPER, GPIO_OTYPER_OT13, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOG->OSPEEDR, GPIO_OSPEEDR_OSPEED13, GPIO_OSPEEDR_25Mhz);
	REG_SETV(GPIOG->PUPDR, GPIO_PUPDR_PUPD13, GPIO_PUPDR_NOPULL);
	SYS_GreenLedOn();
}

void SYS_InitUserButton(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//PA0
	REG_SETV(GPIOA->MODER, GPIO_MODER_MODE0, GPIO_MODER_INPUT);
	REG_SETV(GPIOA->PUPDR, GPIO_PUPDR_PUPD0, GPIO_PUPDR_NOPULL);

	//select port
	REG_CLEAR(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0);
	REG_SET(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0_PA);

	EXTI->RTSR |= EXTI_RTSR_TR0_Msk;  //rising trigger
	EXTI->IMR |= EXTI_IMR_IM0; //do not mask
}

void SYS_InitSysTimer(){
	/*
	 * AHB frequency is 180Mhz,
	 * */
	SysTick->CTRL = 0; /* Stop the timer */
	SysTick->LOAD = 180000;
	SysTick->VAL = 0;
	SysTick->CTRL = 1 << SysTick_CTRL_CLKSOURCE_Pos |SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	SYS_time_millis = 0;
	SYS_SysTimerEnableInt();
}

/*
 * Helmet side buttons
 * PF2 -> Button 0
 * PF3 -> Button 1
 * PF4 -> Button 2
 * PF5 -> Common GND strong pull down
 * Since buttons are pulled low on click, we need a falling edge trigger
 */
void SYS_InitSideButtons(){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER2, GPIO_MODER_INPUT);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD2, GPIO_PUPDR_PULLUP);

	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER3, GPIO_MODER_INPUT);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD3, GPIO_PUPDR_PULLUP);

	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER4, GPIO_MODER_INPUT);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD4, GPIO_PUPDR_PULLUP);

	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER5, GPIO_MODER_OUTPUT);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD5, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOF->OTYPER, GPIO_OTYPER_OT5, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOF->OSPEEDR, GPIO_OSPEEDR_OSPEED5, 0b00);
	GPIOF->BSRR = GPIO_BSRR_BR5; // Output low

	//Configure EXTI ports
	REG_CLEAR(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI2 | SYSCFG_EXTICR1_EXTI3);
	REG_SET(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI2_PF | SYSCFG_EXTICR1_EXTI3_PF);
	REG_CLEAR(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI4);
	REG_SET(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI4_PF);

	// Falling trigger
	EXTI->FTSR |= EXTI_FTSR_TR2 | EXTI_FTSR_TR3 | EXTI_FTSR_TR4;

	// Do not mask
	EXTI->IMR |= EXTI_IMR_IM2 | EXTI_IMR_IM3 | EXTI_IMR_IM4;

	SYS_SideButtonsEnableInt();
}

uint8_t SYS_getButton2Status(){
	return but2_stat;
}

uint8_t SYS_getButton3Status(){
	return but3_stat;
}

uint8_t SYS_getButton4Status(){
	return but4_stat;
}

// Called every 1ms
static void SYS_SideButtonsDebounce(){
	if(but2 > 0){
		but2--;
		if(but2 == 0){
			SYS_SideButton2ClearInt();
			SYS_SideButton2EnableInt();
		}
	}
	if(but3 > 0){
		but3--;
		if(but3 == 0){
			SYS_SideButton3ClearInt();
			SYS_SideButton3EnableInt();
		}
	}
	if(but4 > 0){
		but4--;
		if(but4 == 0){
			SYS_SideButton4ClearInt();
			SYS_SideButton4EnableInt();
		}
	}
}

/*
 * Photoresistor is connected with a 3kOhm resistor
 * PF6 -> ADC3_IN4
 * PF8 -> V+
 * PF10 -> V-
 */
void SYS_InitPhotores(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;

	// PF8
	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER8, GPIO_MODER_OUTPUT);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD8, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOF->OTYPER, GPIO_OTYPER_OT8, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOF->OSPEEDR, GPIO_OSPEEDR_OSPEED8, 0b00);
	GPIOF->BSRR = GPIO_BSRR_BS8; // Output high

	// PF10
	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER10, GPIO_MODER_OUTPUT);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD10, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOF->OTYPER, GPIO_OTYPER_OT10, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOF->OSPEEDR, GPIO_OSPEEDR_OSPEED10, 0b00);
	GPIOF->BSRR = GPIO_BSRR_BR10; // Output low

	// PF6 (ADC3_IN4)
	REG_SETV(GPIOF->MODER, GPIO_MODER_MODER6, GPIO_MODER_ANALOG);
	REG_SETV(GPIOF->PUPDR, GPIO_PUPDR_PUPD6, GPIO_PUPDR_NOPULL);

	// Regular Conversion sequence with only IN4
	REG_SETV(ADC3->SQR1, ADC_SQR1_L, 0);  //Length = 1 conversion
	REG_SETV(ADC3->SQR3, ADC_SQR3_SQ1, 4); // 1st sequence is channel 4

	// ADC clock = APB2 / PRESCALER = 90Mhz / 4 = 20.3 Mhz
	ADC->CCR = ADC_CLOCKPRESCALER_PCLK_DIV4;
	ADC3->CR1 = 0b11 << ADC_CR1_RES_Pos;
	ADC3->CR2 = ADC_CR2_ADON;
}

uint16_t SYS_PhotoresGetVal(){
	ADC3->CR2 |= ADC_CR2_SWSTART;
	while((ADC3->SR & ADC_SR_EOC) == 0);
	return (uint16_t) ADC3->DR;
}


/*
 * Interrupt service routines provided by Erika3
 */

// Every 1 ms
ISR2(SysTimerISR){
	/* Manage time counter */
	SYS_SysTimerClearInt();
	SYS_time_millis++;
	/* Check debounce logic */
	SYS_SideButtonsDebounce();
	/* Execute the hook if needed */
	if(SYS_SysTimer_callback)
		(*SYS_SysTimer_callback)();
}

ISR1(Button2ISR) {
	SYS_SideButton2DisableInt();
	SYS_SideButton2ClearInt();
	if(but2_stat == 0){
		REG_CLEAR(EXTI->FTSR, EXTI_FTSR_TR2);
		REG_SET(EXTI->RTSR, EXTI_RTSR_TR2);
		but2_stat = 1;
	} else {
		REG_CLEAR(EXTI->RTSR, EXTI_RTSR_TR2);
		REG_SET(EXTI->FTSR, EXTI_FTSR_TR2);
		but2_stat = 0;
	}
	but2 = SYS_BUTTON_DEBOUNCE_ms;
}

ISR1(Button3ISR) {
	SYS_SideButton3DisableInt();
	SYS_SideButton3ClearInt();
	if(but3_stat == 0){
		REG_CLEAR(EXTI->FTSR, EXTI_FTSR_TR3);
		REG_SET(EXTI->RTSR, EXTI_RTSR_TR3);
		but3_stat = 1;
	} else {
		REG_CLEAR(EXTI->RTSR, EXTI_RTSR_TR3);
		REG_SET(EXTI->FTSR, EXTI_FTSR_TR3);
		but3_stat = 0;
	}
	but3 = SYS_BUTTON_DEBOUNCE_ms;
}

ISR1(Button4ISR) {
	SYS_SideButton4DisableInt();
	SYS_SideButton4ClearInt();
	if(but4_stat == 0){
		REG_CLEAR(EXTI->FTSR, EXTI_FTSR_TR4);
		REG_SET(EXTI->RTSR, EXTI_RTSR_TR4);
		but4_stat = 1;
	} else {
		REG_CLEAR(EXTI->RTSR, EXTI_RTSR_TR4);
		REG_SET(EXTI->FTSR, EXTI_FTSR_TR4);
		but4_stat = 0;
	}
	but4 = SYS_BUTTON_DEBOUNCE_ms;
}
