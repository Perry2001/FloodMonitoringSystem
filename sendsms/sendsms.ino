#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <vector>
#include "connection.h"

#define GSMRX D1
#define GSMTX D2

FirebaseData firebaseData;
SoftwareSerial mySerial(GSMTX, GSMRX);
String message;
String names;                      // Store names
String numbers;                    // Store numbers
std::vector<String> numbersArray;  // Array to store numbers

int floodHeight = 0;
int height = 0;
int previousFloodHeight = -200;  // Initialize to an invalid value

std::vector<String> split(const String &s, char delimiter) {
  std::vector<String> tokens;
  int start = 0, end = s.indexOf(delimiter);
  while (end != -1) {
    String token = s.substring(start, end);
    token.trim();             // Remove leading and trailing whitespaces
    token.replace("\"", "");  // Remove double quotes
    tokens.push_back(token);
    start = end + 1;
    end = s.indexOf(delimiter, start);
  }
  String lastToken = s.substring(start);
  lastToken.trim();
  lastToken.replace("\"", "");
  tokens.push_back(lastToken);
  return tokens;
}


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.println("Initializing...");
  connectToWiFi();
  Serial.println("Connected to WiFi");
  connectToFirebase();
  Serial.println("Connected to Firebase");

  // Retrieve the data from /contacts/number
  if (Firebase.getString(firebaseData, "/contacts/number")) {
    String dataValue = firebaseData.stringData();
    Serial.print("Data received from /contacts/number: ");
    Serial.println(dataValue);

    // Split the string into pairs
    std::vector<String> pairs = split(dataValue, ',');

    for (const auto &pair : pairs) {
      // Split each pair into name and number
      std::vector<String> parts = split(pair, ':');
      if (parts.size() == 2) {
        String name = parts[0];
        String number = parts[1];
        names += name + "\n";
        numbers += number + "\n";
        number.trim();                   // Remove leading and trailing whitespaces
        numbersArray.push_back(number);  // Save the number in the array
      } else {
        Serial.println("Invalid data format.");
        break;
      }
    }
  } else {
    Serial.println("Failed to retrieve data from /contacts/number.");
    Serial.println("Reason: " + firebaseData.errorReason());
  }

  // Print the numbers array
  Serial.println("Numbers Array:");
  for (const auto &num : numbersArray) {
    Serial.println(num);
  }


  mySerial.println("AT");
  updateSerial();
  mySerial.println("AT+CSQ");
  updateSerial();
  mySerial.println("AT+CCID");
  updateSerial();
  mySerial.println("AT+CREG?");
  updateSerial();


  delay(3000);
}

void loop() {

  if (Firebase.getInt(firebaseData, "/FloodUpdate/height")) {
    height = firebaseData.intData();



    if (height == 3) {
      floodHeight = 3;
      message = "3";
    } else if (height == 2) {
      floodHeight = 2;
      message = "2";
    } else if (height == 1) {
      floodHeight = 1;
      message = "1";
    } else if (height == 0) {
      floodHeight = 0;
      message = "0";
    } else {
      message = "didn't get height value";
    }

    if (floodHeight != previousFloodHeight) {
      if (floodHeight >= 0) {
        // Send SMS to all numbers in the array
        for (const auto &phoneNumber : numbersArray) {
          sendSMS(phoneNumber, message);
          delay(5000);
        }
      }

      previousFloodHeight = floodHeight;  // Update the previous flood height
    }

    Serial.println(height);
  } else {
    Serial.println("error");
  }

  delay(1000);
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read());  // Forward what Serial received to Software Serial Port
  }
  while (mySerial.available()) {
    Serial.write(mySerial.read());  // Forward what Software Serial received to Serial Port
  }
}

void sendSMS(const String &phoneNumber, const String &message) {
  mySerial.println("AT+CMGF=1");  // Set SMS mode to text (1)
  updateSerial();
  mySerial.print("AT+CMGS=\"");  // Set the phone number you want to send an SMS to
  mySerial.print(phoneNumber);
  mySerial.println("\"");
  delay(500);
  mySerial.print(message);
  mySerial.write(26);  // Ctrl+Z to send the message
  delay(500);
  updateSerial();

  Serial.println("SMS Sent.");  // Print a message indicating that the SMS has been sent
}