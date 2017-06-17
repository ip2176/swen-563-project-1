#include "stm32l476xx.h"

void timer_init(void);
void start_capture(void);
void stop_capture(void);
uint32_t get_current_measurement(void);
uint32_t get_current_time(void);
int measurement_detected(void);
