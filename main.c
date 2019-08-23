#include <libopencm3/cm3/common.h> // BEGIN DECL
#include <libopencm3/stm32/f4/memorymap.h>
#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/stm32/f4/timer.h>
#include <libopencm3/stm32/f4/usart.h>
#include "input_capture.h"
  
extern volatile uint16_t period;
extern volatile uint8_t wait_for;


static void clock_setup(void)
{
	/* Enable GPIOD clock for LED & USARTs. */
	rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART2);
}


static void gpio_setup(void)
{  
    /* Setup GPIO pin GPIO12 on GPIO port D for LED. */
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

	/* Setup GPIO pins for USART2 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);

	/* Setup USART2 TX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
}


static void usart_setup(void)
{
	/* Setup USART2 parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART2);
}


int main( void ) {
	
  uint16_t counter=0;
  uint16_t old_timestamp = 0 , new_timestamp = 0 ;
  uint16_t  time_period = 0;
  char c = 0;
  volatile int i ;
  
  
  clock_setup ();		//Initialisation de l'horloge
  gpio_setup ();		//Initialisation du GPIO
  usart_setup();		//Initialisation de l'USART
  
  //usart_send_blocking(USART2, 'a');
  
  gpio_set (GPIOD,GPIO12) ;		//On déclare comme sortie la broche 12
  gpio_clear (GPIOD,GPIO12);    //du registre D et on met son état à 0
  
  //usart_send_blocking(USART2, 'b');
  
  setupInputCapture(0);			//Initialisation de l'input capture
  //usart_send_blocking(USART2, 'c');
  
  enableInputCapture();			//L'input capture est maintenant dispo
  //usart_send_blocking(USART2, 'd');
  
  selectInputTrigger(TIM_SMCR_TS_TI2FP2);		//
  //usart_send_blocking(USART2, 'e');
  
  
  
  wait_for = 0;
  
  while (1) {

/* La variable wait_for est un "flag", c'est-à-dire que lorsque le STM32
va détecter un front montant, il va déclencher ce que l'on appelle une 
interruption qui va lui faire executer une fonction définie auparavant 
et qui va donner la valeur 1 à wait_for. C'est donc de cette manière
que l'on repère un front montant dans notre main.  */
	  
	  
	  if (wait_for==1){ 		//Si wait_for=1 --> Front montant
	     wait_for = 0;			//On réinitialise sa valeur
		 gpio_toggle (GPIOD, GPIO12); //La LED s'allume/s'éteint
		 
		 if (counter == 0){
			 old_timestamp=period;
			 }
			 
		 else if (counter != 0){
			 new_timestamp=period;					 //Ici on calcule la
			 time_period=new_timestamp-old_timestamp;//période du signal 
			 
			 
			 for (i=10000;i>=1;i/=10){       //Toute cette boucle sert à 
			 c = (time_period)/i;		      //envoyer la valeur de la 
			 usart_send_blocking(USART2,'0'+c);//période au port série
			 time_period-=c*i;
				}
			 usart_send_blocking(USART2,'\r');
			 usart_send_blocking(USART2,'\n');
			 old_timestamp=new_timestamp;
			 }
		counter=1;
		}
    }
    
    return 0; 
}
