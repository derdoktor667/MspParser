/*
  MspParserBasicExample
  ---------------------
  This sketch demonstrates the basic functionality of the MspParser library.
  It shows how to initialize the parser, register a callback function,
  and process simulated incoming MSP messages.

  The example will print decoded MSP messages to the Serial Monitor.
  You can send simulated MSP messages via the Serial Monitor to see them parsed.
  For example, a simple MSP_STATUS message (command 101, no payload, checksum 101):
  $M<\x00\x65\x65

  Or an MSP_ATTITUDE message (command 108, 6 bytes payload, checksum calculated):
  $M<\x06\x6C\x00\x00\x00\x00\x00\x00\x72 (example payload for roll=0, pitch=0, yaw=0)
*/

#include <MspParser.h>

MspParser mspParser; // Create an instance of the MspParser

// Callback function to print details of a parsed MSP message.
// This function is called by the MspParser whenever a complete and valid MSP message is received.
void onMspMessageReceived(const MspMessage& message, const char* prefix) {
  Serial.println();
  Serial.print(F("Received MSP Message ("));
  Serial.print(prefix);
  Serial.print(F("): "));
  Serial.print(F("Command ID: 0x"));
  Serial.print(message.command, HEX);
  Serial.print(F(", Payload Size: "));
  Serial.print(message.payloadSize);
  Serial.print(F(", Payload: "));
  for (uint16_t i = 0; i < message.payloadSize; ++i) {
    if (message.payload[i] < 0x10) Serial.print('0');
    Serial.print(message.payload[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(F("MspParser Basic Example Initialized (Raw Output)"));
  Serial.println(F("Send simulated MSP messages via Serial Monitor (e.g., $M<\x00\x65\x65 for MSP_STATUS)"));
  Serial.println(F("--------------------------------------------------------------------------------"));

  // Set the callback function to be called when a valid MSP message is parsed.
  mspParser.onMessage(onMspMessageReceived);

  // Attach the Serial port to the parser.
  // The prefix "Serial" will be passed to the callback to identify the source.
  mspParser.begin(Serial, "Serial");

  // Output format is not set as we are printing raw values directly.
}

void loop() {
  // Continuously update the MSP parser to process incoming bytes from the attached stream.
  mspParser.update();

  // Simulate sending an MSP_STATUS message every 5 seconds for demonstration
  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 5000) {
    lastSendTime = millis();
    Serial.println(F("\n--- Simulating MSP_STATUS message ---"));
    // MSP_STATUS: command 101 (0x65), no payload, checksum 0x65
    // $M< + size (0) + command (101) + checksum (101)
    uint8_t mspStatusMsg[] = {'$', 'M', '<', 0x00, 0x65, 0x65};
    Serial.write(mspStatusMsg, sizeof(mspStatusMsg));
  }
}
