/*
Arduino-MAX30100 oximetry / heart rate integrated sensor library
Copyright (C) 2016  OXullo Intersecans <x@brainrapers.org>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     1000

// Set these to run example.
#define FIREBASE_HOST "breatheeasy-7c7af.firebaseio.com"
#define FIREBASE_AUTH "SnEqneD01m16LemV1eHdetgGVCQX8lCGqrutPaHs"
#define WIFI_SSID "MSV"
#define WIFI_PASSWORD "darkhorses"


// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;

uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}
String t;
int c;
void setup()
{
  c=0;
    t=getTime();
    Serial.begin(115200);
    //SCL=D1,SDA=D2
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.setInt("ard01/"+t+"/spo2/spo2total",0);
    Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
    
}

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        float pulse=pox.getHeartRate(),spo2=pox.getSpO2();
        Serial.print("Heart rate:");
        Serial.print(pulse);
        Serial.print("bpm / SpO2:");
        Serial.print(spo2);
        Serial.println("%");
        Firebase.pushFloat("ard01/"+t+"/pulse",pulse);
        if(Firebase.failed())
        {
          Serial.println("Error");
          Serial.println(Firebase.error());
        }

        Firebase.setFloat("ard01/"+t+"/spo2/"+c,spo2);
        c++;
        if(Firebase.failed())
        {
          Serial.println("Error");
          Serial.println(Firebase.error());
        }

        tsLastReport = millis();
    }
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


