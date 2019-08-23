#ifndef __INPUT_CAPTURE
#define __INPUT_CAPTURE
void setupInputCapture(uint16_t usTimeBase);
void setupPwmInputCapture(uint16_t usTimeBase);
void enableInputCapture(void);
void selectInputTrigger(uint16_t trigger_source);
uint32_t waitForInterrupt(uint16_t *_period, uint16_t *_duty);
#endif /*__INPUT_CAPTURE*/

