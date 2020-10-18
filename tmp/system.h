/*
 * system.h
 *
 *  Created on: Sep 9, 2020
 *      Author: luca
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "utils.h"
#include "ee.h"

/*
 * Clocks
 */
#define SYS_CLK_CORE_Mhz	180
#define SYS_CLK_AHB_Mhz		180
#define SYS_CLK_APB1_Mhz	 45
#define SYS_CLK_APB2_Mhz	 90

/*
 * Time of debouncing in milliseconds
 */
#define SYS_BUTTON_DEBOUNCE_ms 50

/*
 * Data type to hold how many milliseconds have been elapsed since boot happened
 */
typedef uint32_t time_ms_t;

extern volatile time_ms_t SYS_time_millis;
extern void (*SYS_SysTimer_callback)();

/*
 * Return current time in milliseconds
 */
static inline time_ms_t SYS_getTimeMillis(){
	return SYS_time_millis;
}

/*
 * Setup system: I and D chaches,
 * SysClock to 180 Mhz, AHB = 180 Mhz, APB2 = 90 Mhz, APB1 = 45 Mhz.
 * */
void SYS_InitSys();

/*
 * Init the DISCO green led on PA0
 */
void SYS_InitGreenLed();

/*
 * Green led turned on
 */
inline void SYS_GreenLedOn() {
	GPIOG->BSRR = GPIO_BSRR_BS13_Msk;
}

/*
 * Green led turned off
 */
inline void SYS_GreenLedOff() {
	GPIOG->BSRR = GPIO_BSRR_BR13_Msk;
}

/*
 * Init the DISCO user button and installs interrupts EXTI0
 */
void SYS_InitUserButton();

/*
 * Enables interrupt from User button on EXTI0
 */
inline static void SYS_UserButtonEnableInt() {
	__NVIC_EnableIRQ(EXTI0_IRQn);
}

/*
 * Disables interrupt from User button on EXTI0
 */
inline static void SYS_UserButtonDisableInt() {
	__NVIC_DisableIRQ(EXTI0_IRQn);
}

/*
 * Acknowledges the User button interrupt and prevent it to be fired again
 */
inline static void SYS_UserButtonClearInt() {
	EXTI->PR = EXTI_PR_PR0_Msk;
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

/*
 * SysTick every 1 ms
 */
void SYS_InitSysTimer();

inline static void SYS_SysTimerEnableInt(){
	__NVIC_EnableIRQ(SysTick_IRQn);
}

inline static void SYS_SysTimerDisableInt(){
	__NVIC_DisableIRQ(SysTick_IRQn);
}

inline static void SYS_SysTimerClearInt(){
	NVIC_ClearPendingIRQ(SysTick_IRQn);
}

/*
 * Helmet side buttons:
 * PF2 -> Button 2, PF3 -> Button 3, PF4 -> Button 4, PF5 -> Common GND strong pull down.
 */
void SYS_InitSideButtons();

/*
 * Enable all side buttons interrupt
 */
inline static void SYS_SideButtonsEnableInt(){
	__NVIC_EnableIRQ(EXTI2_IRQn);
	__NVIC_EnableIRQ(EXTI3_IRQn);
	__NVIC_EnableIRQ(EXTI4_IRQn);
}

/*
 * Disable all side buttons interrupt
 */
inline static void SYS_SideButtonsDisableInt(){
	__NVIC_DisableIRQ(EXTI2_IRQn);
	__NVIC_DisableIRQ(EXTI3_IRQn);
	__NVIC_DisableIRQ(EXTI4_IRQn);
}

/*
 * Clear button 2 interrupt request
 */
inline static void SYS_SideButton2ClearInt(){
	EXTI->PR = EXTI_PR_PR2_Msk;
	NVIC_ClearPendingIRQ(EXTI2_IRQn);
}

/*
 * Clear button 3 interrupt request
 */
inline static void SYS_SideButton3ClearInt(){
	EXTI->PR = EXTI_PR_PR3_Msk;
	NVIC_ClearPendingIRQ(EXTI3_IRQn);
}

/*
 * Clear button 4 interrupt request
 */
inline static void SYS_SideButton4ClearInt(){
	EXTI->PR = EXTI_PR_PR4_Msk;
	NVIC_ClearPendingIRQ(EXTI4_IRQn);
}

inline static void SYS_SideButton2EnableInt(){
	__NVIC_EnableIRQ(EXTI2_IRQn);
}

inline static void SYS_SideButton2DisableInt(){
	__NVIC_DisableIRQ(EXTI2_IRQn);
}

inline static void SYS_SideButton3EnableInt(){
	__NVIC_EnableIRQ(EXTI3_IRQn);
}

inline static void SYS_SideButton3DisableInt(){
	__NVIC_DisableIRQ(EXTI3_IRQn);
}

inline static void SYS_SideButton4EnableInt(){
	__NVIC_EnableIRQ(EXTI4_IRQn);
}

inline static void SYS_SideButton4DisableInt(){
	__NVIC_DisableIRQ(EXTI4_IRQn);
}

/*
 * Returns 1 if buttons is pressed
 */
uint8_t SYS_getButton2Status();

/*
 * Returns 1 if buttons is pressed
 */
uint8_t SYS_getButton3Status();

/*
 * Returns 1 if buttons is pressed
 */
uint8_t SYS_getButton4Status();


/*
 * Initialize photoresistor
 */
void SYS_InitPhotores();

/*
 * Reads value from ADC3
 */
uint16_t SYS_PhotoresGetVal();


#endif /* SYSTEM_H_ */
