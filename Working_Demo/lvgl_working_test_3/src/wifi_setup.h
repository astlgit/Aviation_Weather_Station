#include <WiFi.h>

// Wi-Fi profiles
const char* ssid_1 = "Saintsville_WIFI";
const char* password_1 = "YouCanDoIt1355";

const char* ssid_2 = "Verizon-MW513U-7B87";
const char* password_2 = "297fc898";

// Select profile: change this to 1 or 2
int wifi_profile = 1;

// Active credentials
const char* active_ssid = nullptr;
const char* active_password = nullptr;

void setupWiFi() {
  // Choose credentials based on profile
  switch (wifi_profile) {
    case 1:
      active_ssid = ssid_1;
      active_password = password_1;
      break;
    case 2:
      active_ssid = ssid_2;
      active_password = password_2;
      break;
    default:
      Serial.println("Invalid Wi-Fi profile selected.");
      return;
  }

  // Connect
  Serial.printf("Connecting to %s...\n", active_ssid);
  WiFi.begin(active_ssid, active_password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nConnected! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nFailed to connect.");
  }
}
