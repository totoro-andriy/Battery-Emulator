#ifndef __HW_DEVKIT_CUSTOM_H__
#define __HW_DEVKIT_CUSTOM_H__

/*
ESP32 DevKit V1 development board with 30 pins.
For more information, see: https://lastminuteengineers.com/esp32-pinout-reference/.

The pin layout below supports the following:
- 1x CAN (via MCP2515 (SPI))
- 1x I2C OLED 
*/

class DevKitCustomHal : public Esp32Hal {
 public:
  const char* name() { return "ESP32 DevKit Custom"; }

  //CAN_NATIVE
  virtual gpio_num_t CAN_TX_PIN() { return GPIO_NUM_27; }
  virtual gpio_num_t CAN_RX_PIN() { return GPIO_NUM_26; }

  // CAN_ADDON (MCP2515)
  virtual gpio_num_t MCP2515_SCK()  { return GPIO_NUM_18; } // D18
  virtual gpio_num_t MCP2515_MOSI() { return GPIO_NUM_23; } // D23 (SI)
  virtual gpio_num_t MCP2515_MISO() { return GPIO_NUM_19; } // D19 (SO)
  virtual gpio_num_t MCP2515_CS()   { return GPIO_NUM_5;  } // D5
  virtual gpio_num_t MCP2515_INT()  { return GPIO_NUM_15; } // D15

  // i2c display (SSD1306)
  virtual gpio_num_t DISPLAY_SDA_PIN() override { return GPIO_NUM_21; } // D21
  virtual gpio_num_t DISPLAY_SCL_PIN() override { return GPIO_NUM_22; } // D22
  
  // Coolant Pump Pins
  virtual gpio_num_t COOLANT_PUMP_PWM_PIN() override { return GPIO_NUM_25; }


  // LED
  virtual gpio_num_t LED_PIN() { return GPIO_NUM_4; }
  virtual uint8_t LED_MAX_BRIGHTNESS() { return 40; }

  // Equipment stop pin
  virtual gpio_num_t EQUIPMENT_STOP_PIN() { return GPIO_NUM_12; }

  std::vector<comm_interface> available_interfaces() {
    return {
        comm_interface::CanAddonMcp2515,
        comm_interface::CanNative
    };
  }
};

#endif  // __HW_DEVKIT_CUSTOM_H__

