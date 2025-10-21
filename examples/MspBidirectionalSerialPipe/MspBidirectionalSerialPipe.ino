/*
  ArduinoSerialPipe Example
  -------------------------
  This sketch demonstrates how to use the MspParser library to create a
  bidirectional serial pipe that logs any valid MSP messages passing through it.

  It works on any board with two serial ports, like an ESP32 or Arduino Mega.

  - WIRING (for ESP32):
    - Connect your App (e.g., Betaflight Configurator) to the USB port (`Serial`).
    - Connect your Flight Controller to `Serial1` (RX:18, TX:19).
*/

#include <MspParser.h>

// Use Serial for the App, Serial1 for the Device (Flight Controller)
// This example is best suited for an ESP32 or Arduino Mega.
HardwareSerial &appPort = Serial;
HardwareSerial &devicePort = Serial;

MspParser mspParser; // Create an instance of the MspParser

// Callback function to print details of a parsed MSP message.
// This function is called by the MspParser whenever a complete and valid MSP message is received.
void printMspMessage(const MspMessage &message, const char *prefix)
{
  appPort.println();
  appPort.print(prefix);
  appPort.print(F(" - Command: "));
  // Use the parser's helper to get the formatted command name
  mspParser.printFormattedCommand(appPort, message.command);
  appPort.print(F(" (0x"));
  // Also print the raw hex value of the command for reference
  appPort.print(message.command, HEX);
  appPort.print(F("), Payload Size: "));
  appPort.println(message.payloadSize);
  appPort.print(F("  Payload: "));
  // Use the parser's helper to get the formatted payload
  mspParser.printFormattedPayload(appPort, message.command, message.payload, message.payloadSize);
  appPort.println();
}

// Arduino setup function. Runs once when the sketch starts.
void setup()
{
  // Initialize serial communication for both ports
  appPort.begin(115200);
  devicePort.begin(115200); // Adjust baud rate to match your Flight Controller

  // Wait for the app serial port to connect (useful for some boards)
  while (!appPort)
  {
    ;
  }

  appPort.println("MSP Serial Pipe & Logger Initialized");

  // Set the desired output format for MSP messages (HEX, BINARY, or DECODED)
  // Change this to MSP_FORMAT_HEX or MSP_FORMAT_BINARY to see different outputs
  mspParser.setOutputFormat(MSP_FORMAT_DECODED);

  // Register the callback function to handle parsed MSP messages
  mspParser.onMessage(printMspMessage);

  // Tell the parser to listen on both the app and device streams
  // The prefixes help identify which stream a message originated from
  mspParser.begin(appPort, "App -> FC");
  mspParser.begin(devicePort, "FC -> App");
}

// Arduino loop function. Runs repeatedly after setup.
void loop()
{
  // Continuously update the MSP parser to process incoming bytes from attached streams
  mspParser.update();

  // Manually forward any incoming bytes from the app port to the device port
  while (appPort.available())
  {
    devicePort.write(appPort.read());
  }
  // Manually forward any incoming bytes from the device port to the app port
  while (devicePort.available())
  {
    appPort.write(devicePort.read());
  }
}
