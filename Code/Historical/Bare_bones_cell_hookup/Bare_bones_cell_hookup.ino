#include "Adafruit_FONA.h" 


/************************* PIN DEFINITIONS *********************************/
#define FONA_PWRKEY 6
#define FONA_RST 7
#define FONA_TX 10 // Microcontroller RX
#define FONA_RX 11 // Microcontroller TX
#define LED 13 // Just for testing if needed!

#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

SoftwareSerial *fonaSerial = &fonaSS;
  
Adafruit_FONA_LTE fona = Adafruit_FONA_LTE();


/****************************** OTHER STUFF ***************************************/
char imei[16] = {0}; // Use this for device ID
uint8_t type;

void setup() {
  Serial.begin(9600);
  Serial.println(F("*** SIMCom Module IoT Example ***"));
  
  pinMode(FONA_RST, OUTPUT);
  digitalWrite(FONA_RST, HIGH); // Default state

  pinMode(FONA_PWRKEY, OUTPUT);
  digitalWrite(FONA_PWRKEY, LOW);
  delay(100); // For SIM7000
  digitalWrite(FONA_PWRKEY, HIGH);
  
  moduleSetup(); // Establishes first-time serial comm and prints IMEI

  // Set modem to full functionality
  fona.setFunctionality(1); // AT+CFUN=1
  fona.setNetworkSettings(F("hologram")); // For Hologram SIM card
  Serial.println(F("Hologram set up"));
}

void loop() {
  while (!netStatus()) {
    Serial.println(F("Failed to connect to cell network, retrying..."));
    delay(2000); // Retry every 2s
  }
  Serial.println(F("Connected to cell network!"));
  while(1);
}

// Power on the module
void powerOn() {
  digitalWrite(FONA_PWRKEY, LOW);
  delay(100); // For SIM7000
  digitalWrite(FONA_PWRKEY, HIGH);
}

void moduleSetup() {
  // SIM7000 takes about 3s to turn on but SIM7500 takes about 15s
  fonaSS.begin(115200); // Default SIM7000 shield baud rate
  
  Serial.println(F("Configuring to 9600 baud"));
  while(1){
    fonaSS.println("AT+IPR=9600"); // Set baud rate
    delay(100); // Short pause to let the command run
    fonaSS.begin(9600);
    if (! fona.begin(fonaSS)) {
      Serial.println(F("Couldn't find FONA"));// Don't proceed if it couldn't find the device
    }
    else{
      break;
    }
  }

  type = fona.type();
  Serial.println(F("FONA is OK"));
  Serial.print(F("Found "));
  // Print module IMEI number.
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
}

bool netStatus() {
  int n = fona.getNetworkStatus();
  
  Serial.print(F("Network status ")); Serial.print(n); Serial.print(F(": "));
  if (n == 0) Serial.println(F("Not registered"));
  if (n == 1) Serial.println(F("Registered (home)"));
  if (n == 2) Serial.println(F("Not registered (searching)"));
  if (n == 3) Serial.println(F("Denied"));
  if (n == 4) Serial.println(F("Unknown"));
  if (n == 5) Serial.println(F("Registered roaming"));

  if (!(n == 1 || n == 5)) return false;
  else return true;
}
