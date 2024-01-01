#include <Arduino.h>
#include <string.h>
#include <SoftwareSerial.h>
SoftwareSerial myserial(7, 8); // Define virtual serial port name as myserial, Rx is port 7, Tx is port 8

// Defining Pins for the Arduino Nano
#define RelayPin 2      // Define the Relay Module pin as D2
#define DrivercPin 3    // Define the L298N Clockwise rotation enable pin as D3 (Switching On)
#define DriveraPin 4    // Define the L298N Anticlockwise rotation enable pin as D4 (Switching Off)
#define IrPin1 5         // Define the first IR sensor pin (Power Off IR Receiver)
#define IrPin2 6         // Define the second IR sensor pin (Power On IR Receiver)

char incomingMessage[100]; // Array to store incoming SMS

void setup()
{
  pinMode(RelayPin, OUTPUT); // Set the LED pin as an output
  pinMode(DrivercPin, OUTPUT); // Set the Driverc pin as an output
  pinMode(DriveraPin, OUTPUT); // Set the Drivera pin as an output
  
  myserial.begin(115200); // Initialize virtual serial port
  Serial.begin(115200);    // Initialize Arduino default serial port
  delay(1000);             // Wait for the GSM module to start

  // Initialize the GSM module
  myserial.println("AT");
  delay(1000);
  myserial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  myserial.println("AT+CNMI=2,2,0,0,0"); // Enable SMS notifications
  delay(1000);

}

void loop()
{
  // Check the state of the Relay Pin
  int relayState = digitalRead(RelayPin);

  // SevWC System Tx control using GSM SIM7600CE Protocol.
  // Check for incoming SMS
  if (myserial.available()) {
    char c = myserial.read();
    if (c == '\n') {
      // Check the received message
      if (strstr(incomingMessage, "syson") != NULL) {
        // Only switch ON the system when it is actually off
        if (relayState == LOW)
        {
           //Switching ON the system...
           digitalWrite(DrivercPin, HIGH); // Rotate the motor clockwise for 2s
           delay(4000); // Wait for 2s
           digitalWrite(DrivercPin, LOW); // Turn off the Motor
           delay(4000); // Wait for 2s
           digitalWrite(RelayPin, HIGH); // Turn on the Relay Module to connect power
           delay(4000); // 1s delay to allow the GSM to Reset
           // Sending message to notify when the system is switched On
           myserial.begin(115200); // Initialize virtual serial port
           Serial.begin(115200);    // Initialize Arduino default serial port
           delay(1000);             // Wait for the GSM module to start
           // Initialize the GSM module
           myserial.println("AT");
           delay(1000);
           myserial.println("AT+CMGF=1"); // Set SMS mode to text
           delay(1000);
           // Send an SMS when the Arduino starts
           myserial.println("AT+CMGS=\"+254796456877\""); // Replace with the desired phone number
           delay(1000);
           myserial.print("The SevWC System is Switched ON successfully!"); // Replace with your message
           delay(100);
           myserial.write(26); // Ctrl-Z to end the SMS
           delay(1000);
        }
      } else if (strstr(incomingMessage, "sysoff") != NULL) {
        // Only switch OFF the system when it is actually On
         if (relayState == HIGH)
          {
           //Switching OFF the system...
            digitalWrite(RelayPin, LOW); // Turn off the Relay module
            delay(4000); // Wait for 2s
            digitalWrite(DriveraPin, HIGH); // Rotate the motor anticlockwise for 2s
            delay(4000); // Wait for 2s
            digitalWrite(DriveraPin, LOW); // Turn off the Motor
            delay(4000); // 10s delay to allow the GSM to Reset
           
           // Sending message to notify when the system is switched Off
           myserial.begin(115200); // Initialize virtual serial port
           Serial.begin(115200);    // Initialize Arduino default serial port
           delay(1000);             // Wait for the GSM module to start
           // Initialize the GSM module
           myserial.println("AT");
           delay(1000);
           myserial.println("AT+CMGF=1"); // Set SMS mode to text
           delay(1000);
           // Send an SMS when the Arduino starts
           myserial.println("AT+CMGS=\"+254796456877\""); // Replace with the desired phone number
           delay(1000);
           myserial.print("The Electric Vehicle has been Disconnected from Power Successfully!"); // Replace with your message
           delay(100);
           myserial.write(26); // Ctrl-Z to end the SMS
           delay(1000);
          }
      }
          // Clear the message buffer
      memset(incomingMessage, 0, sizeof(incomingMessage));
     } else {
      // Append the received character to the message buffer
      strncat(incomingMessage, &c, 1);
    }
  }


  // SevWC System Tx control using the Rx. (RFID Access Control)

  // First IR Receiver (Switch Off IR Receiver)
  int detect1 = digitalRead(IrPin1); // Read first IR sensor status and store
  if (detect1 == LOW)  // Signal is received from the first transmitter (Switch Off the System)
  {
    digitalWrite(RelayPin, LOW); // Turn off the Relay module
    delay(4000); // Wait for 2s
    digitalWrite(DriveraPin, HIGH); // Rotate the motor anticlockwise for 2s
    delay(4000); // Wait for 2s
    digitalWrite(DriveraPin, LOW); // Turn off the Motor
    delay(4000); // Wait for 2s
     // Sending message to notify when the system is switched Off
    myserial.begin(115200); // Initialize virtual serial port
    Serial.begin(115200);    // Initialize Arduino default serial port
    delay(1000);             // Wait for the GSM module to start
    // Initialize the GSM module
    myserial.println("AT");
    delay(1000);
    myserial.println("AT+CMGF=1"); // Set SMS mode to text
    delay(1000);
    // Send an SMS when the Arduino starts
    myserial.println("AT+CMGS=\"+254796456877\""); // Replace with the desired phone number
    delay(1000);
    myserial.print("The Electric Vehicle has been Disconnected from Power Successfully!"); // Replace with your message
    delay(100);
    myserial.write(26); // Ctrl-Z to end the SMS
    delay(1000);

  }

  // Second IR Receiver (Switch On IR Receiver)
  int detect2 = digitalRead(IrPin2); // Read second IR sensor status and store
  if (detect2 == LOW)  // Signal is received from the second transmitter (Switch On the System)
  {
    digitalWrite(DrivercPin, HIGH); // Rotate the motor clockwise for 2s
    delay(4000); // Wait for 2s
    digitalWrite(DrivercPin, LOW); // Turn off the Motor
    delay(4000); // Wait for 2s
    digitalWrite(RelayPin, HIGH); // Turn on the Relay Module to connect power
    delay(4000); // Wait for 2s

    // Sending message to notify when the system is switched On
    myserial.begin(115200); // Initialize virtual serial port
    Serial.begin(115200);    // Initialize Arduino default serial port
    delay(1000);             // Wait for the GSM module to start
    // Initialize the GSM module
    myserial.println("AT");
    delay(1000);
    myserial.println("AT+CMGF=1"); // Set SMS mode to text
    delay(1000);
    // Send an SMS when the Arduino starts
    myserial.println("AT+CMGS=\"+254796456877\""); // Replace with the desired phone number
    delay(1000);
    myserial.print("The SevWC System is Switched ON successfully!"); // Replace with your message
    delay(100);
    myserial.write(26); // Ctrl-Z to end the SMS
    delay(1000);

  }
}
