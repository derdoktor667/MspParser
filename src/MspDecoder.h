#ifndef MSP_DECODER_H
#define MSP_DECODER_H

#include <Arduino.h>
// #include <map> // Removed for Arduino compatibility

// Include the MSP command definitions from Betaflight.
#include "msp_protocol.h"

// Defines the possible output formats for MSP commands and payloads.
enum MspOutputFormat {
    MSP_FORMAT_HEX,     // Output in hexadecimal format.
    MSP_FORMAT_BINARY,  // Output in binary format.
    MSP_FORMAT_DECODED  // Output human-readable decoded names/values.
};

// Structure to hold MSP command ID and its corresponding name.
struct MspCommandName {
    uint16_t commandId;
    const char* name;
};

// MspDecoder class provides utilities for decoding and formatting MSP messages.
// It includes functions to convert byte values to hexadecimal or binary strings,
// retrieve human-readable command names, and (in the future) decode specific payloads.
class MspDecoder {
public:
    // Constructor for MspDecoder.
    MspDecoder();

    // Converts a single byte value to its hexadecimal string representation.
    // The byte to convert.
    // A String containing the hexadecimal representation (e.g., "0A", "FF").
    void printHexString(Stream& stream, uint8_t value);

    // Converts a single byte value to its binary string representation.
    // The byte to convert.
    // A String containing the binary representation (e.g., "00001010", "11111111").
    void printBinaryString(Stream& stream, uint8_t value);

    // Retrieves the human-readable name for a given MSP command ID.
    // The MSP command ID.
    // A String containing the command name (e.g., "MSP_STATUS") or "UNKNOWN_COMMAND" if not found.
    void printCommandName(Stream& stream, uint16_t command);

    // Decodes the payload of an MSP message into a human-readable string.
    // Currently, it returns the hexadecimal representation of the payload.
    // Future enhancements could include command-specific payload decoding.
    // The MSP command ID.
    // Pointer to the payload data.
    // The size of the payload in bytes.
    // A String containing the decoded payload information.
    void printDecodedPayload(Stream& stream, uint16_t command, const uint8_t* payload, uint16_t payloadSize);

private:
    // Array of MspCommandName structs to store MSP command IDs to their human-readable names.
    static const MspCommandName _commandNames[];
    static const size_t _commandNamesCount;

    // Initializes the `commandNamesMap` with known MSP command IDs and their names.
    // void initializeCommandNames(); // No longer needed with static array
};


#endif // MSP_DECODER_H
