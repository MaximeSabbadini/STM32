#include <libopencm3/cm3/common.h> // BEGIN DECL
#include <libopencm3/stm32/f4/memorymap.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/stm32/f4/timer.h>
#include "input_capture_TIM4.h"
  
extern volatile uint16_t period;       //We recover the global variables, declared in inputcapture.h
extern volatile uint8_t wait_for;	   //This variable is a 'flag' that tells us that the STM32 had an interrupt.

static void gpio_setup(void)
{
  rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);  	//Setup of the GPIO

  rcc_periph_clock_enable(RCC_GPIOD);


  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO11|GPIO12|GPIO13);   
}

int main( void ) {
  gpio_setup () ;
  gpio_set (GPIOD,GPIO12);			//Setup of the LED
  gpio_clear (GPIOD,GPIO12);
  
  setupInputCapture(0);  			
  enableInputCapture();				//Setup of the IC
  selectInputTrigger(TIM_SMCR_TS_TI2FP2);
  
  wait_for=0;
  
  while (1) {
	  
	  if (wait_for==1)
	    {wait_for=0;					//When we have a rising edge, the LED toggles.
		 gpio_toggle (GPIOD, GPIO12);
    }
    return 0; 
}
}
