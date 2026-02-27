#include "coolant_pump_control.h"
#include <Arduino.h>
#include "../../datalayer/datalayer.h"
#include "../../devboard/hal/hal.h"
#include "../../devboard/utils/value_mapping.h"

bool coolant_pump_control_enabled = true;
uint16_t coolant_pump_pwm_frequency = 100;  // Tesla pumps are typically driven at 100 Hz
uint8_t coolant_pump_pwm_stop_percent = 10;
uint8_t coolant_pump_pwm_min_percent = 21;
uint8_t coolant_pump_pwm_max_percent = 79;
int16_t coolant_pump_temp_on_dC = 280;
int16_t coolant_pump_temp_off_dC = 250;
int16_t coolant_pump_temp_full_dC = 380;

int16_t coolant_pump_current_on_dA = 30;  // 10.0A absolute current triggers minimum flow
#define COOLANT_PUMP_PWM_CHANNEL 2
#define COOLANT_PUMP_PWM_RESOLUTION 10
#define COOLANT_PUMP_PWM_MAX_DUTY ((1U << COOLANT_PUMP_PWM_RESOLUTION) - 1)

static bool pump_enabled_state = false;
static int32_t last_duty = -1;

static uint16_t percent_to_duty(uint8_t percent) {
  uint32_t p = CONSTRAIN((uint32_t)percent, 0U, 100U);
  return (uint16_t)((p * COOLANT_PUMP_PWM_MAX_DUTY) / 100U);
}

bool init_coolant_pump_control() {
  if (!coolant_pump_control_enabled) {
    return true;
  }

  auto pump_pin = esp32hal->COOLANT_PUMP_PWM_PIN();
  if (pump_pin == GPIO_NUM_NC) {
    DEBUG_PRINTF("Coolant pump pin not defined\n");
    return false;
  }

  if (!esp32hal->alloc_pins("Coolant pump control", pump_pin)) {
    DEBUG_PRINTF("Coolant pump pin allocation failed\n");
    return false;
  }

  ledcAttachChannel(pump_pin, coolant_pump_pwm_frequency, COOLANT_PUMP_PWM_RESOLUTION, COOLANT_PUMP_PWM_CHANNEL);
  ledcWrite(pump_pin, percent_to_duty(coolant_pump_pwm_stop_percent));
  return true;
}

void handle_coolant_pump_control(unsigned long currentMillis) {
  (void)currentMillis;

  if (!coolant_pump_control_enabled) {
    return;
  }

  auto pump_pin = esp32hal->COOLANT_PUMP_PWM_PIN();
  if (pump_pin == GPIO_NUM_NC) {
    return;
  }

  // Safety default: keep valid stop PWM if battery is not active.
  if (datalayer.battery.status.bms_status != ACTIVE) {
    pump_enabled_state = false;
  }

  int16_t t_dC = datalayer.battery.status.temperature_max_dC;

  // Hysteresis
  if (!pump_enabled_state && t_dC >= coolant_pump_temp_on_dC) {
    pump_enabled_state = true;
  } else if (pump_enabled_state && t_dC <= coolant_pump_temp_off_dC) {
    pump_enabled_state = false;
  }

  uint8_t pmin = CONSTRAIN(coolant_pump_pwm_min_percent, 21, 79);
  uint8_t pmax = CONSTRAIN(coolant_pump_pwm_max_percent, pmin, 79);

  uint8_t pwm_percent = coolant_pump_pwm_stop_percent;
  if (pump_enabled_state) {
    if (coolant_pump_temp_full_dC <= coolant_pump_temp_on_dC) {
      pwm_percent = pmin;
    } else {
      int16_t t = CONSTRAIN(t_dC, coolant_pump_temp_on_dC, coolant_pump_temp_full_dC);
      pwm_percent =
          pmin + (uint32_t)(t - coolant_pump_temp_on_dC) * (pmax - pmin) / (coolant_pump_temp_full_dC - coolant_pump_temp_on_dC);
    }
  }

  // If battery current is flowing, force at least minimum pump speed for heat equalization.
  int16_t abs_current_dA = abs((int)datalayer.battery.status.current_dA);
  int16_t current_threshold_dA = MAX((int16_t)0, coolant_pump_current_on_dA);
  if (abs_current_dA >= current_threshold_dA && abs_current_dA > 0) {
    if (pwm_percent < pmin) {
      pwm_percent = pmin;
    }
  }

  uint16_t duty = percent_to_duty(pwm_percent);
  if ((int32_t)duty != last_duty) {
    ledcWrite(pump_pin, duty);
    last_duty = duty;
  }
}



