#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "HX711.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Base64.h"

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "greg"
#define WIFI_PASSWORD "xxxxxxxx"
#define API_KEY "AIzaSyBP8FDAg7z7kjNS44jWiln7U18UOHE49C8"
#define USER_EMAIL "asdf@gmail.com"
#define USER_PASSWORD "asdfasdf"
#define DATABASE_URL "https://projeksemester3-7951b-default-rtdb.firebaseio.com/"

int LED_BUILTIN;
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 5;
const int LED_PIN = 19;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
HX711 scale;

String uid;
String databasePath;
String weightPath = "/weight";

int timestamp;
FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 1000;

void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

void setup(){
  Serial.begin(115200);

  initWiFi();
  configTime(0, 0, ntpServer);

  pinMode(LED_PIN, OUTPUT);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);

//  Serial.println("Getting User UID");
//  while ((auth.token.uid) == "") {
//    Serial.print('.');
//    delay(1000);
//  }
//  uid = auth.token.uid.c_str();
//  Serial.print("User UID: ");
//  Serial.println(uid);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(400.7180327868852);
  scale.tare();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
  Serial.println("muter");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    long reading = scale.get_units(20);
    Serial.print("HX711 reading: ");
    Serial.println(reading);

    if (reading > 10){
      digitalWrite(LED_PIN, HIGH);
    }
    else {
      digitalWrite(LED_PIN, LOW);
    }

    json.set("/" + weightPath, String(reading));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, weightPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
