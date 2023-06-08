#include <BLEDevice.h>
#include <esp_sleep.h>
void setupBLEBeacon() {
  BLEDevice::init("CarFobTransmitter");
  BLEServer* pServer = BLEDevice::createServer();
  BLEAdvertising* pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}
void setup() {
  setupBLEBeacon();
  esp_sleep_enable_timer_wakeup(10 * 1000000);
  delay(3000);
  esp_deep_sleep_start();
}
void loop() {
}
