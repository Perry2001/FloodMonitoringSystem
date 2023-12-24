#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <vector>
#include "connection.h"

FirebaseData firebaseData;

int floodHeight = 0;
int previousFloodHeight = -1; 




void setup() {
  Serial.begin(115200);
  
  Serial.println("Initializing...");
  connectToWiFi();
  Serial.println("Connected to WiFi");
  connectToFirebase();
  Serial.println("Connected to Firebase");

  delay(2000);
}

void loop() {


  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    

    // Process the received data
    if (data.startsWith("SwitchesOn:")) {
      int tot = data.substring(11).toInt(); // Extract the value after the prefix

      Serial.print(tot);

      // Update water level based on the received value
      if (tot == 3) {
        floodHeight = 0;
      } else if (tot == 2) {
        floodHeight = 1;
      } else if (tot == 1) {
        floodHeight = 2;
      } else {
        floodHeight = 3;
      }

      if (floodHeight != previousFloodHeight) {
        // The flood height has changed, update and send SMS
        if (Firebase.setInt(firebaseData, "/FloodUpdate/height", floodHeight)) {
          Serial.println("Firebase data updated successfully.");
          Serial.println(floodHeight);
        } else {
          Serial.println("Failed to update Firebase data.");
          Serial.println("Reason: " + firebaseData.errorReason());
        }
        previousFloodHeight = floodHeight;  // Update the previous flood height
      }
    }
  }

  delay(1000);
}

