markdown
# ✈️ Aviation Weather Station Dashboard

A real-time, touch-enabled weather dashboard for pilots and aviation enthusiasts, built on the Waveshare ESP32-S3-Touch-LCD-7 using LVGL and the Arduino IDE. It visualizes METAR data, wind direction, and runway alignment with custom overlays and modular components.

📁 **Main folder**: [`lvgl_working_test_3`](https://github.com/astlgit/Aviation_Weather_Station/tree/main/Working_Demo/lvgl_working_test_3)

---

## 🚀 Features

- 🌐 Live METAR parsing via AVWX API
- 🧭 Compass-style wind meter with arrowhead needle
- 🛬 Runway overlay visualization with paired identifiers (e.g. 05L/23R)
- 📦 SD card PNG asset loading for runways and weather icons
- 🧠 Modular architecture with reusable components in `/src`
- 🖱️ Touch-enabled airport selection popup
- 🧪 Diagnostic overlays and LVGL file system driver listing

---

## 🧰 Requirements

- **Board**: Waveshare ESP32-S3-Touch-LCD-7
- **IDE**: Arduino IDE 2.x
- **Libraries**:
  - LVGL v8.x
  - ArduinoJson
  - WiFi (ESP32)
  - SD_MMC or SPI SD
  - PNG decoder (e.g. `lvgl/lv_lib_png`)
  - Any required display/touch drivers for your panel

---

## 📁 Folder Overview

| File/Folder                     | Purpose |
|--------------------------------|---------|
| `lvgl_working_test_3.ino`      | Main entry point, Wi-Fi setup, AVWX polling |
| `src/airport_popup.*`          | Touch-enabled ICAO input popup |
| `src/lvgl_sd_driver.*`         | LVGL-compatible SD card file system setup |
| `src/runway_data.*`            | Runway overlay rendering and bearing alignment |
| `src/wind_meter.*`             | Wind meter creation and needle logic |
| `src/wifi_setup.h`             | Wi-Fi credentials and connection logic |
| `metar_parser.*`               | METAR JSON parsing and wind/runway extraction |
| `metar_data.*`                 | AVWX API call and METAR storage |
| `ui_layout.*`                  | LVGL screen layout and button callbacks |
| `esp_panel_*.h`                | Display/touch configuration headers |
| `lvgl_v8_port.*`               | LVGL initialization and porting logic |
| `lv_conf.h`                    | LVGL configuration file |

---

## 🔧 Setup Instructions

1. **Clone the repo**:
   ```bash
   git clone https://github.com/astlgit/Aviation_Weather_Station.git
Open Arduino IDE, navigate to Working_Demo/lvgl_working_test_3.

Install required libraries via Library Manager or GitHub.

Add your AVWX API key in lvgl_working_test_3.ino:

cpp
const char* AVWX_API_KEY = "your_api_key_here";
Prepare your SD card:

Format as FAT32

Create an /assets folder with:

Code
/assets/runways/05L_23R.png
/assets/icons/cloudy.png
Select the correct board:

Tools → Board → ESP32S3 Dev Module (or Waveshare variant)

Enable PSRAM, set appropriate Flash size and upload speed

Compile and upload via USB

🧪 Debugging Tips
Use Serial Monitor to confirm wind direction and asset loading

Enable diagnostic overlays to verify LVGL file system registration

Check fallback logic for missing PNGs or SD card errors

📌 To Do
[ ] Add ICAO airport search with autocomplete

[ ] Cache recent METARs for offline fallback

[ ] Add wind component calculator (head/crosswind)

[ ] Add visual indicators for missing assets or SD errors

📄 License
MIT License. See LICENSE for details.
