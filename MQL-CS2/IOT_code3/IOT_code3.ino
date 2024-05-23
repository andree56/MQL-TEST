#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         3 // Configurable, see typical pin layout above
#define SS_PIN          4 // Configurable, see typical pin layout above

const char* ssid = "Jinnah_Home";
const char* password = "BojoGwapa";
const char* serverUrl = "http://192.168.1.38:3001"; // Replace with your backend server's IP address

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
WiFiClient wifiClient; // Create a WiFi client

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  delay(4); // Optional delay. Some boards need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Convert UID to hexadecimal strings and send it to the server
  String uidStr[4];
  for (byte i = 0; i < 4; i++) {
    uidStr[i] = "0x";
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidStr[i] += "0";
    }
    uidStr[i] += String(mfrc522.uid.uidByte[i], HEX);
    uidStr[i].toLowerCase(); // Convert to lowercase
  }

  Serial.print("Scanned UID: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(uidStr[i]);
    if (i < 3) {
      Serial.print(", ");
    }
  }
  Serial.println();

  String userName, userDepartment;
  if (checkUID(uidStr, userName, userDepartment)) {
    Serial.println("Access Granted");
    // Print the UID before logging attendance
    Serial.print("Logging UID: ");
    for (byte i = 0; i < 4; i++) {
      Serial.print(uidStr[i]);
      if (i < 3) {
        Serial.print(", ");
      }
    }
    Serial.println();

    // Add actions for granted access
    logAttendance(uidStr, userName, userDepartment);
  } else {
    Serial.println("Access Denied");
    // Add actions for denied access
  }

  mfrc522.PICC_HaltA();
  delay(3000);
}

// Function to check UID and return user name if access is granted
bool checkUID(String *uid, String &name, String &department) {
  HTTPClient http;
  String url = String(serverUrl) + "/checkUID";
  Serial.println("Attempting to connect to URL: " + url); // Debug statement
  http.begin(wifiClient, url);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"uid\":[\"" + uid[0] + "\",\"" + uid[1] + "\",\"" + uid[2] + "\",\"" + uid[3] + "\"]}";
  Serial.println("Payload: " + payload); // Debug statement
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    if (response.indexOf("\"success\":true") > 0) {
      int nameStartIndex = response.indexOf("\"name\":\"") + 8;
      int nameEndIndex = response.indexOf("\"", nameStartIndex);
      name = response.substring(nameStartIndex, nameEndIndex);

      int deptStartIndex = response.indexOf("\"department\":\"") + 14;
      int deptEndIndex = response.indexOf("\"", deptStartIndex);
      department = response.substring(deptStartIndex, deptEndIndex);

      return true;
    } else {
      return false;
    }
  } else {
    Serial.println("Error on sending POST: yawa ka vannnnnnnnnnnnnnnnnnnnnnnnnn " + String(httpResponseCode));
    return false;
  }

  http.end();
}

// Function to log attendance
void logAttendance(String *uid, String name, String department) {
  HTTPClient http;
  String url = String(serverUrl) + "/logAttendance";
  http.begin(wifiClient, url);
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"uid\":[\"" + uid[0] + "\",\"" + uid[1] + "\",\"" + uid[2] + "\",\"" + uid[3] + "\"], \"name\":\"" + name + "\", \"department\":\"" + department + "\"}";
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
}
