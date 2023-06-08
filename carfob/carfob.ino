#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEScan* pBLEScan;
bool isLocked = true; // Initial state is locked
bool isDeviceInRange = false; // Flag to track device presence
bool isTrunkOpen = false; // Flag to track trunk opening

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // Empty implementation, we are only interested in scanning results
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(14, OUTPUT);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Create a new BLEScan object
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Active scan uses more power, but gets results faster

  //Initialize Fob
  Serial.println("Initializing Fob");
  digitalWrite(14, LOW);
  delay(500);
  digitalWrite(14, HIGH);
  delay(500);
  digitalWrite(14, LOW);
  delay(500);
  digitalWrite(14, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(12, LOW);
  delay(100);
  digitalWrite(12, HIGH);
  isLocked = true;
  Serial.println("Doors locked");
  Serial.println("Ready...");
}

void unlockTrunk() {
  if (!isLocked) {
    digitalWrite(14, LOW); // Unlock the trunk
    delay(100);
    digitalWrite(14, HIGH);
    delay(500);
    digitalWrite(14, LOW);
    delay(100);
    digitalWrite(14, HIGH);
    Serial.println("Trunk unlocked");
  }
}

void lockDoors() {
  if (!isLocked) {
    digitalWrite(12, LOW); // Lock the doors
    delay(100);
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW); // Lock the doors
    delay(100);
    digitalWrite(12, HIGH);
    Serial.println("Doors locked");
    isLocked = true;
  }
}

void unlockDoors() {
  if (isLocked) {
    digitalWrite(13, LOW); // Unlock the doors
    delay(100);
    digitalWrite(13, HIGH);
    delay(750);
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    isLocked = false;
    Serial.println("Doors unlocked");
  }
}

void loop() {
  if (Serial.available()) { // if there is data comming
    String command = Serial.readStringUntil('\n'); // read string until newline character

    if (command == "lock") {
      digitalWrite(12, LOW);
      Serial.println("lock");
      delay(100);
      digitalWrite(12, HIGH);
    }
    if (command == "unlock") {
      digitalWrite(13, LOW);
      Serial.println("unlock");
      delay(100);
      digitalWrite(13, HIGH);
    }
    if (command == "trunk") {
      digitalWrite(14, LOW);
      Serial.println("trunk");
      delay(100);
      digitalWrite(14, HIGH);
    }
  }
  BLEScanResults foundDevices = pBLEScan->start(1, false); // Scan for 1 second

  bool foundTargetDevice = false;

  if (foundDevices.getCount() > 0) {
    for (int i = 0; i < foundDevices.getCount(); i++) {
      BLEAdvertisedDevice device = foundDevices.getDevice(i);

      if (device.getAddress().equals(BLEAddress("34:85:18:8e:b9:b1"))) {
        foundTargetDevice = true;
        int rssi = device.getRSSI();
        Serial.print("RSSI: ");
        Serial.println(rssi); // Print the RSSI value
        /*if (rssi >= -47) { // Strong signal condition
          if (isDeviceInRange && !isTrunkOpen) {
            unlockTrunk();
            isTrunkOpen = true;
            isDeviceInRange = true;
          }
        }*/
        if (rssi >= -78) { // Strong signal condition
          if (!isDeviceInRange) {
            unlockDoors();
            isDeviceInRange = true;
          }
        } else {
          if (isDeviceInRange && rssi <= -91) {
            lockDoors();
            isTrunkOpen = false;
            isDeviceInRange = false;
          }
        }
      }
    }
  }


  pBLEScan->clearResults(); // Clear the scan results for the next scan
  delay(200);
}
