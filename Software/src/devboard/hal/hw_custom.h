#ifndef __HW_CUSTOM_H__
#define __HW_CUSTOM_H__

class DevKitHal : public Esp32Hal {
 public:
  const char* name() { return "ESP32 DevKit by TOTORO"; }
  // CAN_NATIVE
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

  std::vector<comm_interface> available_interfaces() {
    return {
        comm_interface::CanNative,
        comm_interface::CanAddonMcp2515,
    };
  }
};

#endif  // __HW_CUSTOM_H__