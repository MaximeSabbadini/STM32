#include <libopencm3/cm3/common.h> // BEGIN DECL
#include <libopencm3/stm32/f4/memorymap.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/stm32/f4/flash.h>
#include <libopencm3/stm32/f4/timer.h>
#include <libopencm3/stm32/f4/nvic.h>
#include "input_capture.h"

volatile uint8_t wait_for;
void timer2_enable_irq(void);
void timeBaseInit(void) 
{
	uint32_t tmpcr1 = TIM_CR1(TIM2);

	tmpcr1 &= ~(1 << 4 | TIM_CR1_CMS_MASK);
	tmpcr1 |= TIM_CR1_DIR_UP;
	tmpcr1 &= ~TIM_CR1_CKD_CK_INT_MASK;
	tmpcr1 |= TIM_CR1_CKD_CK_INT;
	TIM_CR1(TIM2) = tmpcr1;

	/* Set the Autoreload value */
	TIM_ARR(TIM2) = 0xFFFF;
  	/* Set the Prescaler value */
	TIM_PSC(TIM2) = 1; //((rcc_apb1_frequency * 2) / 10000);

	/* Generate an update event to reload the Prescaler value immediatly */
	TIM_EGR(TIM2) = TIM_EGR_UG;

	/* select clock source */
	uint32_t tmpsmcr = TIM_SMCR(TIM2);
	tmpsmcr &= ~(TIM_SMCR_SMS_MASK | TIM_SMCR_TS_MASK);
	tmpsmcr &= ~(TIM_SMCR_ETF_MASK | TIM_SMCR_ETPS_MASK | TIM_SMCR_ECE | TIM_SMCR_ETP);
	TIM_SMCR(TIM2) = tmpsmcr;
	TIM_SMCR(TIM2) |= TIM_SMCR_SMS_OFF;

	/* synchro */
	/* Reset the MMS Bits */
	TIM_CR2(TIM2) &= ~TIM_CR2_MMS_MASK;
	/* Select the TRGO source */
	TIM_CR2(TIM2) |= TIM_CR2_MMS_RESET;
	/* Reset the MSM Bit */
	TIM_SMCR(TIM2) &= ~TIM_SMCR_MSM;
	/* Set or Reset the MSM Bit */
	TIM_SMCR(TIM2) &= ~TIM_SMCR_MSM;
}

void rcc_init(void)
{
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_GPIOA);
}
void gpio_init(void)
{
	gpio_mode_setup(GPIOA, GPIO_MODE_AF,
			GPIO_PUPD_NONE, GPIO1);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO1);
}

void T2Config(void)
{
	uint32_t tmpccmr1 = 0;
	uint32_t tmpccer = 0;

	/* Disable the Channel 2: Reset the CC2E Bit */
	TIM_CCER(TIM2) &= ~TIM_CCER_CC2E;
	tmpccmr1 = TIM_CCMR1(TIM2);
	tmpccer = TIM_CCER(TIM2);

	/* Select the Input */
	tmpccmr1 &= ~TIM_CCMR1_CC2S_MASK;
	tmpccmr1 |= TIM_CCMR1_CC2S_IN_TI2;

	/* Set the filter */
	tmpccmr1 &= ~TIM_CCMR1_IC2F_MASK;
	tmpccmr1 |= (TIM_CCMR1_IC2F_OFF);

	/* Select the Polarity and set the CC2E Bit */
	tmpccer &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
	//tmpccer |= ((TIM_ICPolarity << 4U) & (TIM_CCER_CC2P | TIM_CCER_CC2NP));

	/* Write to TIMx CCMR1 and CCER registers */
	TIM_CCMR1(TIM2) = tmpccmr1 ;
	TIM_CCER(TIM2) = tmpccer;

	/* Reset the IC1PSC Bits */
	TIM_CCMR1(TIM2) &= ~TIM_CCMR1_IC1PSC_MASK;

	/* Set the IC1PSC value */
	TIM_CCMR1(TIM2) |= TIM_CCMR1_IC1PSC_OFF;

}

#define TIM_TS_TI2FP2	((uint16_t)0x0060)
#define SMCR_TS_Mask    ((uint16_t)0xFF8F)
void selectInputTrigger(uint16_t trigger_source)
{
	//selectInputTrigger(TIM_TS_TI2FP2);

	uint16_t tmpsmcr = 0;
    /* Get the TIMx SMCR register value */
    tmpsmcr = TIM2_SMCR;
    /* Reset the TS Bits */
    tmpsmcr &= SMCR_TS_Mask;
    /* Set the Input Trigger source */
    tmpsmcr |= trigger_source;
    /* Write to TIMx SMCR */
    TIM2_SMCR = tmpsmcr;    
    
  /*  
TIM2_SMCR.SMS2 =1; // Reset Mode 100
TIM2_SMCR.SMS1 =0;
TIM2_SMCR.SMS0 =0;

TIM2_SMCR.TS2 =1; //Trigger Reset on Filtered Timer Input 2
TIM2_SMCR.TS1 =1; //100 = TI1 Edge Detect, 101 Filted TI1, 110 Filtered TI2
TIM2_SMCR.TS0 =0;

TIM2_SMCR.MSM =1; //Master slave mode
*/
}
#define TIM_SlaveMode_Reset         ((uint16_t)0x0004)
#define TIM_MasterSlaveMode_Enable	((uint16_t)0x0080)
#define SMCR_SMS_Mask               ((uint16_t)0xFFF8)
#define SMCR_MSM_Reset              ((uint16_t)0xFF7F)

#define CCMR_IC13PSC_Mask           ((uint16_t)0xFFF3)
void setupInputCapture(uint16_t usTimeBase)
{
	(void)usTimeBase;
	// trop specifique faut deplacer
	rcc_init();
	gpio_init();
	//rcc_periph_reset_pulse(RST_TIM2);
	//timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
	//        TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	 //TIM_ICSelection_DirectTI, TIM_ICPolaryty_Rising, 1*/

	//timer_set_prescaler(TIM2, ((rcc_apb1_frequency* 2)/ 50));
	
	// ok mais les params a passer a l'appel
	timeBaseInit();
    /* TI2 Configuration */
	T2Config();
	//TIM_SMCR(TIM2) = (0x6 << 4);

	timer2_enable_irq();
}

void timer2_enable_irq(void)
{
    nvic_enable_irq(NVIC_TIM2_IRQ);
    nvic_set_priority(NVIC_TIM2_IRQ, 0);

	/* TIM_ClearFlag */
	//TIM2_SR = (uint16_t)~TIM_FLAG_CC2;
	/* ARRPreloadConfig*/
	//TIM2_CR1 |=CR1_ARPE_Set;
	/* IT_Config */	
	//TIM2_DIER |= TIM_DIER_CC2IE;
	//TIM2_DIER |= 1<<10;
	//TIM2_DIER |= TIM_DIER_UIE | /*TIM_DIER_CC1IE|*/TIM_DIER_CC2IE;
	TIM2_DIER = 0;
	TIM2_DIER |= TIM_DIER_CC2IE;
	TIM2_DIER &= ~1;
	 /* Counter enable. */
    //timer_enable_counter(TIM2);

		     /* Enable Channel 1 compare interrupt to recalculate compare values
			  * */
    //timer_enable_irq(TIM2, TIM_DIER_CC1IE|TIM_DIER_CC2IE);
}

void enableInputCapture(void)
{
	//return;
	//timer_ic_enable(TIM2, TIM_IC1);
	//timer_ic_enable(TIM2, TIM_IC2);
	//TIM_SMCR(TIM2) |= TIM_SMCR_SMS_OFF;
    TIM_CR1(TIM2) |= 0x0001;
	TIM_EGR(TIM2) |= 1 << 2;
	TIM_CCER(TIM2) |= TIM_CCER_CC2E;
}
void disableInputCapture(void)
{
	timer_ic_disable(TIM2, TIM_IC1);
    //TIM_CR1(TIM2) &= ~0x0001;
}

#define TIM_IT_CC2                  ((uint16_t)0x0004)

volatile uint16_t IC2Value;
volatile uint16_t DutyCycle;
volatile uint16_t period;
volatile uint16_t height;
volatile uint16_t IC2Nb;
volatile uint16_t counter;
volatile uint8_t CaptureNumber;

void tim2_isr(void)
{
	uint16_t tim_sr = TIM2_SR;
	//TIM2_SR &= ~0xffff;
		IC2Value = TIM2_CCR2;
	if (tim_sr & (0x01<<10)){
		//TIM2_SR |= 1<<10; 
		//put_string(CONSOLE, "overcapture\r\n");
		//put_unsigned_int(CONSOLE, TIM2_CNT,4);
	}
	if ((tim_sr & (TIM_IT_CC2)) != 0x00) {
//	put_string(CONSOLE, "machin ");
//	writeHEXc(CONSOLE, tim_sr);
//	put_string(CONSOLE, "\r\n");
	//gpio_toggle(GPIOD, GPIO13);
		/* Clear TIM2 Capture compare interrupt pending bit */
		TIM2_SR = (uint16_t)~TIM_IT_CC2;

		/* Get the Input Capture value */
		//IC2Value = TIM2_CCR2;

		//if (IC2Value != 0) {
    		/* Duty cycle computation */
    	//	DutyCycle = TIM2_CCR1;
		//	if (wait_for >= 10) {
		//		counter = TIM2_CNT;
				period = IC2Value;
		//		height = DutyCycle;
				wait_for = 1;
				CaptureNumber = 1;
		//	} else 
		//		wait_for ++;
  		//} else {
    	//	DutyCycle = 0;
		//}
	}
}
