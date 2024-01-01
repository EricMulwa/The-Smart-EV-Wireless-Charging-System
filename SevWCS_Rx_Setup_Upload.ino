/*
THE FINAL SevWCS RECEIVER (Rx) PROGRAM
This program controls two IR Sensor transmitters depending on RFID Tags scanned. 
The Power ON Tag controls the IR1 Transmitter which switches ON the Tx Section of the SevWCS 
The Power OFF Tag controls the IR2 Transmitter which swithes OFF the Tx section of the SevWCS
The 1602 LCD display shows when system is switching on, switched on, switching off, switched off and when charging
The system can also switch Off itself when a full-charge signal is received from the BMS of the EV
To be added: Indicator LEDs (Optional)

  Last modified 23 OCT 2023
  by Eric Mulwa Eng BSc
*/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

const int rs = 14, en = 15, d4 = 16, d5 = 17, d6 = 18, d7 = 19;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define RST_PIN 9 // Configurable, see typical pin layout above
#define SS_PIN 10 // Configurable, see typical pin layout above
#define InductionPin 3 // Define the Induction Pin
#define BmsPin 2 // Define the BMS pin

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
byte accessUID[4] = { 0x26, 0xF2, 0x21, 0x7E }; //Power on access UID
byte offccessUID[4] = { 0xA2, 0x14, 0x66, 0xD9 }; //Power off access UID

int IR1transmitterPin = 4; // Power on the system
int IR2transmitterPin = 5; // Power off the system

void setup() {
  pinMode(IR1transmitterPin, OUTPUT);  // Set the IR1 Transmitter pin as an output
  pinMode(IR2transmitterPin, OUTPUT);  // Set the IR2 Transmitter pin as an output
  pinMode(InductionPin, INPUT); // Set the Induction Pin as Input
  pinMode(BmsPin, INPUT); // Set the BMS Pin as Input

  Serial.begin(9600);  // Initialize serial communications with the PC
  while (!Serial);      // Do nothing if no serial port is opened
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);             // Optional delay. Some board do need more time after init to be ready
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  //Welcome Greeting
  lcd.clear();
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("The SevWC System");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("Welcome to Tech!");
}

void loop() {
  if (digitalRead(InductionPin) == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Power On");
    lcd.setCursor(0, 1);
    lcd.print("EV Charging");
  }

  // Check if BMS is detected
  if (digitalRead(BmsPin) == HIGH) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Switching Off...");
        digitalWrite(IR2transmitterPin, HIGH); // Turn on the IR2 Transmitter (Power off the system)
        delay(2000); // Wait for 3s
        digitalWrite(IR2transmitterPin, LOW); // Turn Off the IR2 Transmitter
        delay(4000); // Delay to allow the Tx to cut off power supply before
        //Only display "System Power Off" if and only if the system is actually off. That is induction signal is LOW
        lcd.clear();
        delay(1000);
        lcd.setCursor(0, 0);
        lcd.print("System Power Off");
        delay(2000);
        lcd.setCursor(0, 1);
        lcd.print("EV Disconnected");
        delay(2000);
    
     //23s delay in total for automatic power off. 
     //The BMS Signal will only last for 10s to make sure the If Statement is not executed recursively 
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  if (mfrc522.uid.uidByte[0] == accessUID[0] && mfrc522.uid.uidByte[1] == accessUID[1] && mfrc522.uid.uidByte[2] == accessUID[2] && mfrc522.uid.uidByte[3] == accessUID[3]) {
    // Switch on the system only when the system is off. That is the Induction signal is LOW
     lcd.clear();
     delay(1000);
     lcd.setCursor(0, 0);
     lcd.print("Switching On...");
     digitalWrite(IR1transmitterPin, HIGH); // Turn on the IR1 Transmitter (Power on the System)
     delay(2000); // Wait for 4s
     digitalWrite(IR1transmitterPin, LOW); // Turn off the IR1 Transmitter
     delay(5000); // Delay to allow the Tx to connect power supply
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("System Power On");
            lcd.setCursor(0, 1);
            lcd.print("EV Charging");
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  if (mfrc522.uid.uidByte[0] == offccessUID[0] && mfrc522.uid.uidByte[1] == offccessUID[1] && mfrc522.uid.uidByte[2] == offccessUID[2] && mfrc522.uid.uidByte[3] == offccessUID[3]) {
  // Switch Off the system only when the system is On. That is the Induction signal is HIGH
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Switching Off...");
       digitalWrite(IR2transmitterPin, HIGH); // Turn on the IR2 Transmitter (Power off the system)
       delay(2000); // Wait for 4s
       digitalWrite(IR2transmitterPin, LOW); // Turn Off the IR2 Transmitter
       delay(5000); // Delay to allow the Tx to cut off power supply before displaying "System power off"
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("System Power Off");
            delay(1000);
            lcd.setCursor(0, 1);
            lcd.print("EV Disconnected");
  }
  mfrc522.PICC_HaltA();
}

//----------------------------------Eric Mulwa BSc Eng----------------------------------//