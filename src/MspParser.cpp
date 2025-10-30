#include "MspParser.h"
#include "MspDecoder.h"

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a)
{
    crc ^= a;
    for (int i = 0; i < 8; ++i) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0xD5;
        } else {
            crc <<= 1;
        }
    }
    return crc & 0xFF; // Ensure it stays 8-bit
}

// Constructor for MspParser.
MspParser::MspParser() : _debugCallback(nullptr) {}

// Attaches a Stream object to the parser.
// The parser will listen for data on this stream. Up to two streams can be attached.
void MspParser::begin(Stream& stream, const char* prefix, void (*debugCallback)(const String& message)) {
  // Only allow attaching up to 2 streams
  if (_parserCount < 2) {
    _parsers[_parserCount]._stream = &stream;
    _parsers[_parserCount]._prefix = prefix;
    _parserCount++;
  }
  _debugCallback = debugCallback;
}

// Sets the callback function to be called when a valid MSP message is parsed.
void MspParser::onMessage(MspMessageCallback callback) {
  _messageCallback = callback;
}

// Processes all attached streams.
// This method should be called repeatedly in the main Arduino `loop()` function.
void MspParser::update() {
  // Iterate through all active parsers
  for (uint8_t i = 0; i < _parserCount; ++i) {
    // Process all available bytes in the current stream
    while (_parsers[i]._stream && _parsers[i]._stream->available()) {
      uint8_t incomingByte = _parsers[i]._stream->read();
      processIncomingByte(incomingByte, _parsers[i]);
    }
  }
}

// Sets the desired output format for command and payload decoding.
void MspParser::setOutputFormat(MspOutputFormat format) {
  _currentOutputFormat = format;
}

// Gets the formatted string representation of an MSP command ID.
// The format depends on the currently set output format.
void MspParser::printFormattedCommand(Stream& stream, uint16_t command) {
  switch (_currentOutputFormat) {
    case MSP_FORMAT_HEX:
      _mspDecoder.printHexString(stream, command);
      break;
    case MSP_FORMAT_BINARY:
      _mspDecoder.printBinaryString(stream, command);
      break;
    case MSP_FORMAT_DECODED:
    default:
      _mspDecoder.printCommandName(stream, command);
      break;
  }
}

// Gets the formatted string representation of an MSP message payload.
// The format depends on the currently set output format.
void MspParser::printFormattedPayload(Stream& stream, uint16_t command, const uint8_t* payload, uint16_t payloadSize) {
  switch (_currentOutputFormat) {
    case MSP_FORMAT_HEX:
      {
        for (uint8_t i = 0; i < payloadSize; ++i) {
          _mspDecoder.printHexString(stream, payload[i]);
          stream.print(' ');
        }
      }
      break;
    case MSP_FORMAT_BINARY:
      {
        for (uint8_t i = 0; i < payloadSize; ++i) {
          _mspDecoder.printBinaryString(stream, payload[i]);
          stream.print(' ');
        }
      }
      break;
    case MSP_FORMAT_DECODED:
    default:
      _mspDecoder.printDecodedPayload(stream, command, payload, payloadSize);
      break;
  }
}

void MspParser::processIncomingByte(uint8_t incomingByte, Parser& parser) {
  if (_debugCallback) {
    _debugCallback("DEBUG MspParser: Incoming byte: 0x" + String(incomingByte, HEX) + ", State: " + String(parser._currentState));
  }

  switch (parser._currentState) {
    case IDLE:
      if (incomingByte == '$') {
        parser._currentState = HEADER_M;
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> HEADER_M");
      }
      break;
    case HEADER_M:
      if (incomingByte == 'M') {
        parser._currentState = HEADER_DIRECTION;
        parser._isMspV2 = false; // Set flag for MSPv1
        parser._currentCrc = 0; // Reset CRC for MSPv1
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> HEADER_DIRECTION (MSPv1)");
      } else if (incomingByte == 'X') {
        parser._currentState = HEADER_DIRECTION;
        parser._isMspV2 = true;  // Set flag for MSPv2
        parser._currentCrc = 0; // Reset CRC for MSPv2
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> HEADER_DIRECTION (MSPv2)");
      } else {
        parser._currentState = IDLE; // Invalid header, reset
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> IDLE (Invalid header)");
      }
      break;
    case HEADER_DIRECTION:
      // Check for message direction ('>' for FC to App, '<' for App to FC)
      if (incomingByte == '<' || incomingByte == '>') {
        parser._currentDirection = incomingByte; // Store direction
        parser._currentPayloadSize = 0; // Reset for new message
        parser._currentCommand = 0;
        parser._currentPayloadIndex = 0;
        parser._currentChecksum = 0;
        if (parser._isMspV2) { // Use the new flag
          parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte); // Include direction in CRC
          parser._currentState = FLAGS;
          if (_debugCallback) _debugCallback("DEBUG MspParser: State -> FLAGS (MSPv2)");
        } else { // If it was MSPv1 header
          parser._currentState = SIZE;
          if (_debugCallback) _debugCallback("DEBUG MspParser: State -> SIZE (MSPv1)");
        }
      } else {
        parser._currentState = IDLE; // Invalid header, reset
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> IDLE (Invalid direction)");
      }
      break;
    case FLAGS: // MSPv2 specific
      parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte);
      parser._currentState = SIZE; // Next is payload size low byte
      if (_debugCallback) _debugCallback("DEBUG MspParser: State -> SIZE (MSPv2 flags received)");
      break;
    case SIZE:
      parser._currentPayloadSize = incomingByte;
      parser._currentChecksum ^= incomingByte; // MSPv1 checksum
      if (parser._isMspV2) { // Use the new flag
        parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte); // MSPv2 CRC
        parser._currentState = SIZE_HIGH;
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> SIZE_HIGH (MSPv2 size low received)");
      } else {
        parser._currentState = COMMAND;
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> COMMAND (MSPv1 size received)");
      }
      break;
    case SIZE_HIGH: // MSPv2 specific
      parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte);
      parser._currentState = COMMAND; // Next is command low byte
      if (_debugCallback) _debugCallback("DEBUG MspParser: State -> COMMAND (MSPv2 size high received)");
      break;
    case COMMAND:
      parser._currentCommand = incomingByte;
      parser._currentChecksum ^= incomingByte; // MSPv1 checksum
      if (parser._isMspV2) {
        parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte); // MSPv2 CRC
        parser._currentState = COMMAND_HIGH;
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> COMMAND_HIGH (MSPv2 command low received)");
      } else {
        parser._currentState = (parser._currentPayloadSize > 0) ? PAYLOAD : CHECKSUM;
        if (_debugCallback) _debugCallback("DEBUG MspParser: State -> " + String(parser._currentState == PAYLOAD ? "PAYLOAD" : "CHECKSUM") + " (MSPv1 command received)");
      }
      break;
    case COMMAND_HIGH: // MSPv2 specific
      parser._currentCommand |= (uint16_t)incomingByte << 8;
      parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte);
      parser._currentState = (parser._currentPayloadSize > 0) ? PAYLOAD : CRC; // Next is payload or CRC
      if (_debugCallback) _debugCallback("DEBUG MspParser: State -> " + String(parser._currentState == PAYLOAD ? "PAYLOAD" : "CRC") + " (MSPv2 command high received)");
      break;
    case PAYLOAD:
      // Store payload byte if buffer has space
      if (parser._currentPayloadIndex < sizeof(parser._payloadBuffer)) {
        parser._payloadBuffer[parser._currentPayloadIndex++] = incomingByte;
      }
      parser._currentChecksum ^= incomingByte; // MSPv1 checksum
      if (parser._isMspV2) {
        parser._currentCrc = crc8_dvb_s2(parser._currentCrc, incomingByte); // MSPv2 CRC
      }
      // Check if all payload bytes have been received
      if (parser._currentPayloadIndex >= parser._currentPayloadSize) {
        if (parser._isMspV2) { // Use the new flag
          parser._currentState = CRC;
          if (_debugCallback) _debugCallback("DEBUG MspParser: State -> CRC (MSPv2 payload complete)");
        } else { // If it was MSPv1
          parser._currentState = CHECKSUM;
          if (_debugCallback) _debugCallback("DEBUG MspParser: State -> CHECKSUM (MSPv1 payload complete)");
        }
      }
      break;
    case CHECKSUM: // MSPv1 specific
      // Validate checksum and trigger callback if valid
      if (incomingByte == parser._currentChecksum && _messageCallback) {
        MspMessage parsedMessage;
        parsedMessage.direction = parser._currentDirection; // Use stored direction
        parsedMessage.command = parser._currentCommand;
        parsedMessage.payloadSize = parser._currentPayloadSize;
        parsedMessage.payload = parser._payloadBuffer;
        _messageCallback(parsedMessage, parser._prefix);
        if (_debugCallback) _debugCallback("DEBUG MspParser: MSPv1 message parsed successfully. Command: 0x" + String(parsedMessage.command, HEX));
      } else {
        if (_debugCallback) _debugCallback("DEBUG MspParser: MSPv1 checksum mismatch or no callback. Expected: 0x" + String(parser._currentChecksum, HEX) + ", Received: 0x" + String(incomingByte, HEX));
      }
      parser._currentState = IDLE; // Reset for next message
      if (_debugCallback) _debugCallback("DEBUG MspParser: State -> IDLE");
      break;
    case CRC: // MSPv2 specific
      // Validate CRC and trigger callback if valid
      if (incomingByte == parser._currentCrc && _messageCallback) {
        MspMessage parsedMessage;
        parsedMessage.direction = parser._currentDirection; // Use stored direction
        parsedMessage.command = parser._currentCommand;
        parsedMessage.payloadSize = parser._currentPayloadSize;
        parsedMessage.payload = parser._payloadBuffer;
        _messageCallback(parsedMessage, parser._prefix);
        if (_debugCallback) _debugCallback("DEBUG MspParser: MSPv2 message parsed successfully. Command: 0x" + String(parsedMessage.command, HEX));
      } else {
        if (_debugCallback) _debugCallback("DEBUG MspParser: MSPv2 CRC mismatch or no callback. Expected: 0x" + String(parser._currentCrc, HEX) + ", Received: 0x" + String(incomingByte, HEX));
      }
      parser._currentState = IDLE; // Reset for next message
      if (_debugCallback) _debugCallback("DEBUG MspParser: State -> IDLE");
      break;
  }
}

// Constructs and sends an MSP message over the specified stream.
// The Stream object to send the message over.
// The MSP command ID.
// Pointer to the payload data.
// The size of the payload in bytes.
// If true, sends an MSPv2 message; otherwise, sends an MSPv1 message.
// Private helper to send an MSPv1 message.
void MspParser::sendMspV1Message(Stream& stream, char direction, uint8_t command, const uint8_t* payload, uint8_t payloadSize) {
    uint8_t checksum = 0;

    stream.write('$');
    stream.write('M');
    stream.write(direction);

    stream.write(payloadSize);
    checksum ^= payloadSize;

    stream.write(command);
    checksum ^= command;

    for (uint16_t i = 0; i < payloadSize; ++i) {
        stream.write(payload[i]);
        checksum ^= payload[i];
    }
    stream.write(checksum);
}

// Private helper to send an MSPv2 message.
void MspParser::sendMspV2Message(Stream& stream, char direction, uint16_t command, const uint8_t* payload, uint16_t payloadSize) {
    uint8_t crc = 0;
    uint8_t flags = 0; // For now, flags are 0

    stream.write('$');
    stream.write('X');
    stream.write(direction);
    crc = crc8_dvb_s2(crc, direction);

    stream.write(flags);
    crc = crc8_dvb_s2(crc, flags);

    stream.write((uint8_t)(payloadSize & 0xFF)); // Payload size low byte
    crc = crc8_dvb_s2(crc, (uint8_t)(payloadSize & 0xFF));
    stream.write((uint8_t)(payloadSize >> 8));   // Payload size high byte
    crc = crc8_dvb_s2(crc, (uint8_t)(payloadSize >> 8));

    stream.write((uint8_t)(command & 0xFF));     // Command low byte
    crc = crc8_dvb_s2(crc, (uint8_t)(command & 0xFF));
    stream.write((uint8_t)(command >> 8));       // Command high byte
    crc = crc8_dvb_s2(crc, (uint8_t)(command >> 8));

    for (uint16_t i = 0; i < payloadSize; ++i) {
        stream.write(payload[i]);
        crc = crc8_dvb_s2(crc, payload[i]);
    }
    stream.write(crc);
}

// Constructs and sends an MSP message over the specified stream.
// The Stream object to send the message over.
// The MSP command ID.
// Pointer to the payload data.
// The size of the payload in bytes.
// If true, sends an MSPv2 message; otherwise, sends an MSPv1 message.
void MspParser::sendMspMessage(Stream& stream, char direction, uint16_t command, const uint8_t* payload, uint16_t payloadSize, bool useMspV2) {
    // Automatically upgrade to MSPv2 if command or payload size is too large for V1
    sendMspV2Message(stream, direction, command, payload, payloadSize);
}
