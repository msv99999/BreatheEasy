#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "breatheeasy-7c7af.firebaseio.com"
#define FIREBASE_AUTH "SnEqneD01m16LemV1eHdetgGVCQX8lCGqrutPaHs"
#define WIFI_SSID "MSV"
#define WIFI_PASSWORD "darkhorses"
String t;
void setup() {
  Serial.begin(115200);
  initWifi();
  t=getTime();
  pinMode(A0,INPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
}

void loop() {
  int flexadc=analogRead(A0);
  String adc=String(flexadc);
  Serial.println(adc);
  
  Firebase.pushInt("ard01/"+t+"/flex",flexadc);
  
  if(Firebase.failed())
  {
    Serial.println("Error");
    Serial.println(Firebase.error());
  }
  delay(500);
  

}

void initWifi()
{
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}


String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");
 
  while(!!!client.available()) {
     yield();
  }

  while(client.available()){
    if (client.read() == '\n') {    
      if (client.read() == 'D') {    
        if (client.read() == 'a') {    
          if (client.read() == 't') {    
            if (client.read() == 'e') {    
              if (client.read() == ':') {    
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                return theDate;
              }
            }
          }
        }
      }
    }
  }
}


