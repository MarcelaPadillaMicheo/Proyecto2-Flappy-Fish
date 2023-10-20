/*********************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *********************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL2Mvx7wwCr"
#define BLYNK_TEMPLATE_NAME "Flappy Bird"
#define BLYNK_AUTH_TOKEN "2ToI4_xbkZCg8FLwdGHyTzTRscwmTAoT"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth [] = "2ToI4_xbkZCg8FLwdGHyTzTRscwmTAoT";

// Your WiFi credentials.
// Set password to "" for open networks.

char ssid[] = "marcelapad";
char pass[] = "Makima0611";

void setup (){
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}
void loop () {
  Blynk.run();
}