#ifndef MSP_PARSER_H
#define MSP_PARSER_H

#include <Arduino.h>
#include <Stream.h>
#include "MspDecoder.h" // Include the MSP decoder class

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a);

// Represents a fully parsed MSP message.
struct MspMessage {
  char direction;
  uint16_t command;
  uint16_t payloadSize;
  const uint8_t* payload;
};

// Callback function type. Users define a function of this type to handle parsed MSP messages.
typedef void (*MspMessageCallback)(const MspMessage& message, const char* prefix);

// MspParser class for parsing MultiWii Serial Protocol (MSP) messages.
// This class provides a non-blocking, callback-based mechanism to parse MSP messages
// from one or two Arduino Stream objects. It can also format the output of commands
// and payloads into hexadecimal, binary, or human-readable decoded strings.
class MspParser {
public:
  // Constructor for MspParser.
  MspParser();

  // Attaches a Stream object to the parser.
  // The parser will listen for data on this stream. Up to two streams can be attached.
  void begin(Stream& stream, const char* prefix, void (*debugCallback)(const String& message) = nullptr);

  // Sets the callback function to be called when a valid MSP message is parsed.
  void onMessage(MspMessageCallback callback);

  // Processes all attached streams.
  // This method should be called repeatedly in the main Arduino `loop()` function.
  void update();

  // Sets the desired output format for command and payload decoding.
  void setOutputFormat(MspOutputFormat format);

  // Gets the formatted string representation of an MSP command ID.
  // The format depends on the currently set output format.
  void printFormattedCommand(Stream& stream, uint16_t command);

  // Gets the formatted string representation of an MSP message payload.
  // The format depends on the currently set output format.
  void printFormattedPayload(Stream& stream, uint16_t command, const uint8_t* payload, uint16_t payloadSize);

  // Constructs and sends an MSP message over the specified stream.
  // The Stream object to send the message over.
  // The MSP command ID.
  // Pointer to the payload data.
  // The size of the payload in bytes.
  // If true, sends an MSPv2 message; otherwise, sends an MSPv1 message.
  void sendMspMessage(Stream& stream, char direction, uint16_t command, const uint8_t* payload, uint16_t payloadSize, bool useMspV2 = false);

  // Returns a reference to the internal MspDecoder instance.
  // This allows users to directly access MspDecoder's formatting utilities.
  MspDecoder& getDecoder() { return _mspDecoder; }

private:
  // Internal states for the MSP parsing state machine.
  enum MspState {
    IDLE,               // Waiting for the start of a new message ('$').
    HEADER_M,           // Received '$', waiting for 'M' (MSPv1).
    HEADER_X,           // Received '$', waiting for 'X' (MSPv2).
    HEADER_DIRECTION,   // Received '$M' or '$X', waiting for '<' or '>'.
    FLAGS,              // Received '$X<', waiting for flags byte (MSPv2).
    SIZE,               // Received '$M<' or '$X<flags>', waiting for payload size byte (MSPv1) or low byte (MSPv2).
    SIZE_HIGH,          // Received '$X<flags><size_low>', waiting for high byte (MSPv2).
    COMMAND,            // Received payload size, waiting for command byte (MSPv1) or low byte (MSPv2).
    COMMAND_HIGH,       // Received '<command_low>', waiting for high byte (MSPv2).
    PAYLOAD,            // Receiving payload bytes.
    CHECKSUM,           // Received all payload bytes, waiting for checksum byte (MSPv1).
    CRC                 // Received all payload bytes, waiting for CRC byte (MSPv2).
  };

  // Structure to hold the state for a single parsing stream.
  struct Parser {
    Stream* _stream = nullptr;
    const char* _prefix = "";
    MspState _currentState = IDLE;
    uint16_t _currentPayloadSize = 0;
    uint16_t _currentCommand = 0;
    uint8_t _currentFlags = 0;
    uint8_t _payloadBuffer[32];
    uint16_t _currentPayloadIndex = 0;
    uint8_t _currentChecksum = 0;
    uint8_t _currentCrc = 0;
    bool _isMspV2 = false;
    char _currentDirection;
  };

  // Array to hold parser states for up to 2 streams simultaneously.
  Parser _parsers[2];
  uint8_t _parserCount = 0;

  MspMessageCallback _messageCallback = nullptr;
  void (*_debugCallback)(const String& message) = nullptr;
  MspDecoder _mspDecoder;
  MspOutputFormat _currentOutputFormat = MSP_FORMAT_DECODED;

  // Processes a single incoming byte for a given parser.
  // This is the core state machine logic for parsing MSP messages.
  void processIncomingByte(uint8_t incomingByte, Parser& parser);

  // Private helper to send an MSPv1 message.
  void sendMspV1Message(Stream& stream, char direction, uint8_t command, const uint8_t* payload, uint8_t payloadSize);
  // Private helper to send an MSPv2 message.
  void sendMspV2Message(Stream& stream, char direction, uint16_t command, const uint8_t* payload, uint16_t payloadSize);
};

#endif // MSP_PARSER_H
