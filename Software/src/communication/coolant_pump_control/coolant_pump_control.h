#ifndef _COOLANT_PUMP_CONTROL_H_
#define _COOLANT_PUMP_CONTROL_H_

#include "../../devboard/utils/events.h"

// Coolant pump controller.
//
// Behavior summary:
// - Initializes PWM output for coolant pump.
// - In runtime, maps battery max temperature to PWM duty with hysteresis.
// - Forces at least minimal flow when battery current exceeds threshold.

// Runtime settings
extern bool coolant_pump_control_enabled;
extern uint16_t coolant_pump_pwm_frequency;
extern uint8_t coolant_pump_pwm_stop_percent;  // Valid stop zone: 8..12
extern uint8_t coolant_pump_pwm_min_percent;   // Valid run zone starts at 21
extern uint8_t coolant_pump_pwm_max_percent;   // Valid run zone ends at 79
extern int16_t coolant_pump_temp_on_dC;        // deci-C, e.g. 300 = 30.0C
extern int16_t coolant_pump_temp_off_dC;       // deci-C, e.g. 270 = 27.0C
extern int16_t coolant_pump_temp_full_dC;      // deci-C, e.g. 430 = 43.0C

extern int16_t coolant_pump_current_on_dA;  // deci-A, absolute current threshold for min flow

// Configure GPIO/PWM and set initial stop duty.
bool init_coolant_pump_control();

// Update pump PWM according to temperature, hysteresis and current threshold.
void handle_coolant_pump_control(unsigned long currentMillis);

#endif
