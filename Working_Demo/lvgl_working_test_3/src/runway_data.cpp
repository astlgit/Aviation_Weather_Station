#include "src/runway_data.h"
#include "src/wind_meter.h"

std::vector<RunwayPair> runwayPairs;

void preloadDefaultRunways() {
  runwayPairs.clear();
  runwayPairs.push_back({ "02", 20, "20", 200 });
  runwayPairs.push_back({ "05L", 50, "23R", 230 });
  runwayPairs.push_back({ "14", 135, "32", 315 });
}

void preloadRunwaysForAirport(const std::vector<RunwayPair>& runways) {
  runwayPairs = runways;
  Serial.printf("✅ Loaded %d runway pairs\n", (int)runways.size());
}

void showRunwayNeedle(const String& ident1, const String& ident2, int bearing) {
  setRunwayNeedle(bearing);
  Serial.printf("✅ Showing runway needle: %s/%s → Heading: %d\n",
                ident1.c_str(), ident2.c_str(), bearing);
}
