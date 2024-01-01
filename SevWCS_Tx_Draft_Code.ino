/*
THE COMPLETE SevWCS TRANSMITTER (Tx STM32) PROGRAM
This Program controls a DC Motor and a Relay Module Using GSM SIM7600CE Module, Two IR Receivers and STM32F103C8T6 Microconroller.
The program is the Transmitter section of the Smart EV Wireless Charging System.
In the program, The SIM7600CE GSM Module receives the SMS "syson" from the user and executes it by turning on the DC Motor and Rotating it in the 
clockwise direction for two seconds before turning on the relay module and connecting power to charge the Electric Vehicle on station.
When the SIM7600CE GSM Module receives the SMS "sysoff", it executes it by turning OFF the relay module 
which disconnects power from the Electric Vehicle on station before rotating the DC Motor in the antclockwise direction for two seconds 
The movement of the DC Motor in the Clockwise direction and Anticlockwise direction implies movement of the system Up and Down respectively.
When IR1 Receiver gets a signal, it Executes the same process as the "sysoff" command and sends an sms to the parking lot attendant.
The SMS send is "The Ev No.X has been Disconnected from Power!"
When IR2 Receiver gets a signal, it Executes the same process as the "syson" command.
The IR Receivers are protected from recursive execution by reading the RelayPin status. This way, you cannot turn on or off the system twice.

  Last modified 23 OCT 2023
  by Eric Mulwa Eng BSc
*/

// User Includes
#include <Arduino.h>
#include <string.h>

// Defining Pins for the STM32F103C8T6
#define RelayPin PB12    // Define the Relay Module pin as PB12
#define DrivercPin PB13  // Define the L298N Clockwise rotation enable pin as PB13 (Switching On)
#define DriveraPin PB14  // Define the L298N Anticlockwise rotation enable pin as PB14 (Switching Off)
#define IrPin1 8         // Define the first IR sensor pin (Power Off IR Receiver)
#define IrPin2 1         // Define the second IR sensor pin (Power On IR Receiver)

char incomingMessage[100]; // Array to store incoming SMS

void setup()
{
  pinMode(RelayPin, OUTPUT); // Set the LED pin as an output
  pinMode(DrivercPin, OUTPUT); // Set the Driverc pin as an output
  pinMode(DriveraPin, OUTPUT); // Set the Drivera pin as an output

  Serial1.begin(115200);  // Use Serial1 for STM32, adjust the baud rate as needed
  delay(1000);            // Wait for the GSM module to start

  // Initialize the GSM module
  Serial1.println("AT");
  delay(1000);
  Serial1.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  Serial1.println("AT+CNMI=2,2,0,0,0"); // Enable SMS notifications
  delay(1000);

  Serial.begin(9600);
  pinMode(IrPin1, INPUT); // Set the first IR sensor pin as input
  pinMode(IrPin2, INPUT); // Set the second IR sensor pin as input
}

void loop()
{
  // Check the state of the Relay Pin
  int relayState = digitalRead(RelayPin);

  // SevWC System Tx control using GSM SIM7600CE Protocol.
  // Check for incoming SMS
  if (Serial1.available())
  {
    char c = Serial1.read();
    if (c == '\n')
    {
      // Check the received message
      if (strstr(incomingMessage, "syson") != NULL)
      {
        // Only switch ON the system when it is actually off
        if (relayState == LOW)
          {
           //Switching ON the system...
           digitalWrite(DrivercPin, HIGH); // Rotate the motor clockwise for 2s
           delay(2000); // Wait for 2s
           digitalWrite(DrivercPin, LOW); // Turn off the Motor
           delay(2000); // Wait for 2s
           digitalWrite(RelayPin, HIGH); // Turn on the Relay Module to connect power

           delay(10000); // 10s delay to allow the GSM to Reset
            // Sending message to notify when the system is switched On
            char mobileNumber[] = "+254796456877";  // Mobile Number to send SMS
            char ATcommand[80];
            uint8_t buffer[30] = {0};
            uint8_t ATisOK = 0;

            while (!ATisOK) {
            sprintf(ATcommand, "AT\r\n");
            Serial1.print(ATcommand);
            delay(1000);
            if (Serial1.find("OK")) {
            ATisOK = 1;
            }
           delay(1000);
            }

           sprintf(ATcommand, "AT+CMGF=1\r\n");
           Serial1.print(ATcommand);
           delay(100);
           Serial1.readBytes(buffer, sizeof(buffer));
           delay(1000);
           memset(buffer, 0, sizeof(buffer));

           sprintf(ATcommand, "AT+CMGS=\"%s\"\r\n", mobileNumber);
           Serial1.print(ATcommand);
           delay(100);
           sprintf(ATcommand, "The SevWC System is Switched ON successfully !%c", 0x1a);
           Serial1.print(ATcommand);
           delay(4000);
          }
      }
      else if (strstr(incomingMessage, "sysoff") != NULL)
      {
        // Only switch OFF the system when it is actually On
        if (relayState == HIGH)
          {
           //Switching OFF the system...
            digitalWrite(RelayPin, LOW); // Turn off the Relay module
            delay(2000); // Wait for 2s
            digitalWrite(DriveraPin, HIGH); // Rotate the motor anticlockwise for 2s
            delay(2000); // Wait for 2s
            digitalWrite(DriveraPin, LOW); // Turn off the Motor

            delay(10000); // 10s delay to allow the GSM to Reset
            // Sending message to notify when the system is switched Off
            char mobileNumber[] = "+254796456877";  // Mobile Number to send SMS
            char ATcommand[80];
            uint8_t buffer[30] = {0};
            uint8_t ATisOK = 0;

            while (!ATisOK) {
            sprintf(ATcommand, "AT\r\n");
            Serial1.print(ATcommand);
            delay(1000);
            if (Serial1.find("OK")) {
            ATisOK = 1;
            }
           delay(1000);
            }

           sprintf(ATcommand, "AT+CMGF=1\r\n");
           Serial1.print(ATcommand);
           delay(100);
           Serial1.readBytes(buffer, sizeof(buffer));
           delay(1000);
           memset(buffer, 0, sizeof(buffer));

           sprintf(ATcommand, "AT+CMGS=\"%s\"\r\n", mobileNumber);
           Serial1.print(ATcommand);
           delay(100);
           sprintf(ATcommand, "The Electric Vehicle has been Disconnected from Power Successfully !%c", 0x1a);
           Serial1.print(ATcommand);
           delay(4000);

          }
      }
      // Clear the message buffer
      memset(incomingMessage, 0, sizeof(incomingMessage));
    }
    else
    {
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
      delay(2000); // Wait for 2s
      digitalWrite(DriveraPin, HIGH); // Rotate the motor anticlockwise for 2s
      delay(2000); // Wait for 2s
      digitalWrite(DriveraPin, LOW); // Turn off the Motor

            // Sending message to notify when the system is switched Off
            char mobileNumber[] = "+254796456877";  // Mobile Number to send SMS
            char ATcommand[80];
            uint8_t buffer[30] = {0};
            uint8_t ATisOK = 0;

            while (!ATisOK) {
            sprintf(ATcommand, "AT\r\n");
            Serial1.print(ATcommand);
            delay(1000);
            if (Serial1.find("OK")) {
            ATisOK = 1;
            }
           delay(1000);
            }

           sprintf(ATcommand, "AT+CMGF=1\r\n");
           Serial1.print(ATcommand);
           delay(100);
           Serial1.readBytes(buffer, sizeof(buffer));
           delay(1000);
           memset(buffer, 0, sizeof(buffer));

           sprintf(ATcommand, "AT+CMGS=\"%s\"\r\n", mobileNumber);
           Serial1.print(ATcommand);
           delay(100);
           sprintf(ATcommand, "The Electric Vehicle has been Disconnected from Power Successfully !%c", 0x1a);
           Serial1.print(ATcommand);
           delay(4000);
    } 

  // Second IR Receiver (Switch On IR Receiver)
  int detect2 = digitalRead(IrPin2); // Read second IR sensor status and store

    if (detect2 == LOW)  // Signal is received from the second transmitter (Switch On the System)
    {
      digitalWrite(DrivercPin, HIGH); // Rotate the motor clockwise for 2s
      delay(2000); // Wait for 2s
      digitalWrite(DrivercPin, LOW); // Turn off the Motor
      delay(2000); // Wait for 2s
      digitalWrite(RelayPin, HIGH); // Turn on the Relay Module to connect power
      delay(2000); // Wait for 2s

            // Sending message to notify when the system is switched On
            char mobileNumber[] = "+254796456877";  // Mobile Number to send SMS
            char ATcommand[80];
            uint8_t buffer[30] = {0};
            uint8_t ATisOK = 0;

            while (!ATisOK) {
            sprintf(ATcommand, "AT\r\n");
            Serial1.print(ATcommand);
            delay(1000);
            if (Serial1.find("OK")) {
            ATisOK = 1;
            }
           delay(1000);
            }

           sprintf(ATcommand, "AT+CMGF=1\r\n");
           Serial1.print(ATcommand);
           delay(100);
           Serial1.readBytes(buffer, sizeof(buffer));
           delay(1000);
           memset(buffer, 0, sizeof(buffer));

           sprintf(ATcommand, "AT+CMGS=\"%s\"\r\n", mobileNumber);
           Serial1.print(ATcommand);
           delay(100);
           sprintf(ATcommand, "The SevWC System is Switched ON successfully!%c", 0x1a);
           Serial1.print(ATcommand);
           delay(4000);     
     }

}
