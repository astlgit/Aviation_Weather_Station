#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <esp_io_expander.hpp>
#include <esp_display_panel.hpp>

#define SD_MOSI 11
#define SD_CLK  12
#define SD_MISO 13
#define SD_SS   -1
#define SD_CS   4

extern esp_panel::board::Board* board;  // Access the initialized board

bool init_sd_card() {
  esp_expander::CH422G* expander = board->getExpander();  // Use board's expander
  expander->enableAllIO_Output();
  expander->digitalWrite(SD_CS, LOW);

  SPI.setHwCs(false);
  SPI.begin(SD_CLK, SD_MISO, SD_MOSI, SD_SS);

  if (!SD.begin(SD_SS)) {
    Serial.println("SD Card Mount Failed");
    return false;
  }

  Serial.println("SD Card Initialized");
  return true;
}
