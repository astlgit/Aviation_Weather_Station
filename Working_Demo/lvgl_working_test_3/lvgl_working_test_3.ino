#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"
#include "src/wifi_setup.h"
#include "ui_layout.h"
#include "metar_data.h"
#include "src/lvgl_sd_driver.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <esp_io_expander.hpp>

// SD and I2C pin definitions
#define SD_MOSI 11
#define SD_CLK  12
#define SD_MISO 13
#define SD_SS   -1
#define SD_CS    4

#define EXAMPLE_I2C_ADDR    (ESP_IO_EXPANDER_I2C_CH422G_ADDRESS)
#define EXAMPLE_I2C_SDA_PIN 8
#define EXAMPLE_I2C_SCL_PIN 9

using namespace esp_panel::drivers;
using namespace esp_panel::board;

Board* board = nullptr;  // Global board instance

bool init_sd_card() {
  auto expander = static_cast<esp_expander::CH422G*>(board->getExpander());
  expander->enableAllIO_Output();
  expander->digitalWrite(SD_CS, LOW);  // Enable SD card via expander

  SPI.setHwCs(false);
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_SS);

  if (!SD.begin(SD_SS)) {
    Serial.println("SD Card Mount Failed");
    return false;
  }

  Serial.println("SD Card Initialized");
  return true;
}

void setup() {
  Serial.begin(115200);
  setupWiFi();

  Serial.println("Initializing board");
  board = new Board();
  board->init();

#if LVGL_PORT_AVOID_TEARING_MODE
  auto lcd = board->getLCD();
  lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);

#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
  auto lcd_bus = lcd->getBus();
  if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
    static_cast<BusRGB*>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
  }
#endif
#endif

  assert(board->begin());

  Serial.println("Initializing LVGL");
  lvgl_port_init(board->getLCD(), board->getTouch());

  if (!init_sd_card()) {
    Serial.println("SD card init failed!");
    return;
  }

 lv_png_init();              // Enables PNG decoding
register_lvgl_sd_driver();  // Registers "S:" file system


  Serial.println("Creating UI");
  lvgl_port_lock(-1);
  setupUI();

  const char* defaultStation = "KRDU";
  updateDisplay(defaultStation);
  lvgl_port_unlock();

}

void loop() {
  lv_timer_handler();
  delay(5);
}
