/*
  MspParserEsp32Example
  ---------------------
  This example demonstrates the full functionality of the MspParser library
  on an ESP32 board. It showcases:
  1.  Parsing incoming MSPv1 and MSPv2 messages from a Serial stream.
  2.  Using the MspDecoder for human-readable output of commands and payloads.
  3.  Sending MSPv1 and MSPv2 messages.
  4.  Direct usage of MspDecoder for formatting.

  The ESP32 is well-suited for this example due to its multiple hardware serial
  ports and ample memory, allowing for full decoding features.

  To test:
  - Upload this sketch to your ESP32 board.
  - Open the Serial Monitor at 115200 baud.
  - Send simulated MSP messages (MSPv1 or MSPv2) via the Serial Monitor.
    Examples:
    MSPv1 (MSP_STATUS, command 101, no payload, checksum 101):
    $M<\x00\x65\x65

    MSPv2 (MSP_API_VERSION, command 1, no payload, CRC calculated):
    $X<\x00\x00\x01\x00\x01
    (Note: The last byte '01' is the CRC for this specific message. You might need a tool to calculate CRC for other MSPv2 messages.)

  The sketch will also periodically send simulated MSP messages and demonstrate
  direct MspDecoder usage.
*/

#include <MspParser.h>
#include <HardwareSerial.h> // For ESP32's multiple hardware serials

// Create an instance of the MspParser
MspParser mspParser;

// Define a second HardwareSerial for demonstration if available (e.g., Serial2 on ESP32)
// If your ESP32 board doesn't have Serial2, you can comment this out or use SoftwareSerial
// if you have a library for it on ESP32, but HardwareSerial is preferred.
// HardwareSerial SerialPort2(2); // Use UART2 pins (GPIO16, GPIO17 by default)

// Callback function to handle parsed MSP messages
void onMspMessageReceived(const MspMessage& message, const char* prefix) {
  Serial.println(F("\n--- MSP Message Received ---"));
  Serial.print(F("Source: "));
  Serial.println(prefix);

  // Access the MspDecoder instance from the MspParser
  MspDecoder& decoder = mspParser.getDecoder();

  Serial.print(F("Direction: "));
  Serial.println(message.direction);

  Serial.print(F("Command ID (Raw): 0x"));
  Serial.println(message.command, HEX);

  Serial.print(F("Command Name: "));
  decoder.printCommandName(Serial, message.command); // Use MspDecoder for human-readable name
  Serial.println();

  Serial.print(F("Payload Size: "));
  Serial.println(message.payloadSize);

  Serial.print(F("Payload (HEX): "));
  for (uint16_t i = 0; i < message.payloadSize; ++i) {
    if (message.payload[i] < 0x10) Serial.print('0');
    Serial.print(message.payload[i], HEX);
    Serial.print(' ');
  }
  Serial.println();

  // Demonstrate formatted payload output using MspParser's built-in function
  Serial.print(F("Payload (Formatted via MspParser): "));
  mspParser.setOutputFormat(MSP_FORMAT_HEX); // Set format for this print
  mspParser.printFormattedPayload(Serial, message.command, message.payload, message.payloadSize);
  Serial.println();

  // You can also set other formats and print
  // mspParser.setOutputFormat(MSP_FORMAT_BINARY);
  // Serial.print(F("Payload (Binary via MspParser): "));
  // mspParser.printFormattedPayload(Serial, message.command, message.payload, message.payloadSize);
  // Serial.println();

  Serial.println(F("----------------------------"));
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  Serial.println(F("MspParser ESP32 Example Initialized"));
  Serial.println(F("-----------------------------------"));

  // Set the callback function to be called when a valid MSP message is parsed.
  mspParser.onMessage(onMspMessageReceived);

  // Attach the main Serial port to the parser.
  // The prefix "Serial0" will be passed to the callback to identify the source.
  mspParser.begin(Serial, "Serial0");

  // If you have a second hardware serial port, you can attach it too.
  // SerialPort2.begin(115200);
  // mspParser.begin(SerialPort2, "Serial2");

  // Set the default output format for MspParser's printFormatted functions
  // This will be used if not explicitly changed before calling printFormattedCommand/Payload
  mspParser.setOutputFormat(MSP_FORMAT_DECODED);

  Serial.println(F("Parser ready. Send MSP messages via Serial Monitor."));
  Serial.println(F("-----------------------------------"));
}

void loop() {
  // Continuously update the MSP parser to process incoming bytes from the attached streams.
  mspParser.update();

  // --- Demonstrate Sending MSP Messages ---
  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 5000) { // Send messages every 5 seconds
    lastSendTime = millis();

    Serial.println(F("\n--- Simulating Sending MSP Messages ---"));

    // Example 1: Send MSPv1 MSP_STATUS (command 101, no payload)
    Serial.println(F("Sending MSPv1 MSP_STATUS..."));
    uint8_t mspStatusPayload[] = {}; // Empty payload
    mspParser.sendMspMessage(Serial, MSP_STATUS, mspStatusPayload, 0, false);
    Serial.println(F("Sent MSPv1 MSP_STATUS."));

    // Example 2: Send MSPv2 MSP_API_VERSION (command 1, no payload)
    // Note: For MSPv2, the command ID is 16-bit.
    Serial.println(F("Sending MSPv2 MSP_API_VERSION..."));
    uint8_t mspApiVersionPayload[] = {}; // Empty payload
    mspParser.sendMspMessage(Serial, MSP_API_VERSION, mspApiVersionPayload, 0, true);
    Serial.println(F("Sent MSPv2 MSP_API_VERSION."));

    // Example 3: Send MSPv1 MSP_SET_RAW_RC (command 200, 16 bytes payload for 8 channels)
    Serial.println(F("Sending MSPv1 MSP_SET_RAW_RC with dummy data..."));
    uint8_t rcData[16]; // 8 channels * 2 bytes/channel
    for (int i = 0; i < 16; ++i) {
      rcData[i] = i + 1; // Dummy data
    }
    mspParser.sendMspMessage(Serial, MSP_SET_RAW_RC, rcData, sizeof(rcData), false);
    Serial.println(F("Sent MSPv1 MSP_SET_RAW_RC."));

    // --- Demonstrate Direct MspDecoder Usage ---
    Serial.println(F("\n--- Demonstrating Direct MspDecoder Usage ---"));
    MspDecoder& decoder = mspParser.getDecoder();

    // Print a command name directly
    Serial.print(F("Direct Decoder - Command Name for MSP_ATTITUDE (0x6C): "));
    decoder.printCommandName(Serial, MSP_ATTITUDE);
    Serial.println();

    // Print a byte in hex
    uint8_t testByte = 0xA5;
    Serial.print(F("Direct Decoder - 0xA5 in HEX: "));
    decoder.printHexString(Serial, testByte);
    Serial.println();

    // Print a byte in binary
    Serial.print(F("Direct Decoder - 0xA5 in BIN: "));
    decoder.printBinaryString(Serial, testByte);
    Serial.println();

    // Print a dummy payload in hex
    uint8_t dummyPayload[] = {0x12, 0x34, 0xAB, 0xCD};
    Serial.print(F("Direct Decoder - Dummy Payload (HEX): "));
    decoder.printDecodedPayload(Serial, 0, dummyPayload, sizeof(dummyPayload)); // Command 0 is placeholder
    Serial.println();

    Serial.println(F("---------------------------------------------"));
  }
}
