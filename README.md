# MspParser - A Universal MSP Parser for Arduino

`MspParser` is a lightweight, universal, and flexible library for parsing MultiWii Serial Protocol (MSP) messages from any Arduino `Stream` object (like `Serial`, `SoftwareSerial`, or `AltSoftSerial`).

It is designed to be non-blocking and uses a callback mechanism, allowing you to react to valid MSP messages without complex state management in your main loop. This makes it ideal for building tools like serial loggers, protocol converters, or custom dashboards.

## Features

- **Universal:** Works with any `Stream` object (HardwareSerial, SoftwareSerial, etc.).
- **Flexible:** Uses a callback function to handle parsed messages, decoupling the parser from your application logic.
- **Multi-Stream:** Can parse up to two streams simultaneously, perfect for building bidirectional serial pipes.
- **Lightweight:** Minimal memory footprint, especially when optimized for resource-constrained devices like Arduino Uno.
- **Well-Documented:** Clear and concise API.
- **Send Messages:** Easily construct and send MSPv1 or MSPv2 messages to a connected device.
- **Decoded Output:** Provides options to output MSP commands and payloads in hexadecimal, binary, or human-readable (decoded) formats, including known Betaflight commands. *Note: Human-readable decoding is memory-intensive and may not be suitable for Arduino Uno.*
- **Improved Internal Structure:** Refactored for better readability and maintainability.
- **Direct Decoder Access:** The `MspDecoder` instance is directly accessible for advanced formatting needs.

## Installation

1.  Download the latest release as a `.zip` file from the [Releases](https://github.com/your-username/MspParser/releases) page.
2.  In the Arduino IDE, go to `Sketch` > `Include Library` > `Add .ZIP Library...`
3.  Select the downloaded `.zip` file.
4.  The library will be installed and you can now use it by including `<MspParser.h>`.

Alternatively, you can clone this repository into your Arduino `libraries` folder.

## How It Works

The library maintains a state machine for each stream you attach. You feed it data by calling the `update()` method in your main `loop()`. When a complete, valid MSP message (with a correct checksum) is received, the library invokes a callback function that you provide, passing the parsed message details in a simple `MspMessage` struct. Additionally, you can use the `sendMspMessage` function to construct and send MSPv1 or MSPv2 messages to a connected device.

## Examples

### MspBasicParsing (Recommended for Arduino Uno)

This example demonstrates the fundamental parsing capabilities of the `MspParser` library. To fit within the limited RAM of an Arduino Uno, this example prints raw command IDs and payload bytes directly, bypassing the human-readable decoding features of the `MspDecoder`.

It initializes the parser with the main `Serial` port and registers a callback to print the received message's command ID (in hex), payload size, and payload bytes (in hex). It also simulates sending an `MSP_STATUS` message every 5 seconds.

You can find this example under `File > Examples > MspParser > MspBasicParsing`.

### MspBidirectionalSerialPipe (Recommended for ESP32/Arduino Mega)

This example demonstrates how to use the `MspParser` library to create a bidirectional serial pipe that logs any valid MSP messages passing through it. It is best suited for boards with multiple hardware serial ports and more RAM, such as an ESP32 or Arduino Mega, as it utilizes the full decoding capabilities of the library.

You can find this example under `File > Examples > MspParser > MspBidirectionalSerialPipe`.

### MspComprehensiveEsp32 (Comprehensive ESP32 Example)

This comprehensive example showcases the full functionality of the `MspParser` library on an ESP32 board. It demonstrates parsing incoming MSPv1 and MSPv2 messages, using the `MspDecoder` for human-readable output of commands and payloads, sending MSPv1 and MSPv2 messages, and direct usage of the `MspDecoder` for custom formatting. It is ideal for understanding the library's capabilities on more powerful microcontrollers.

You can find this example under `File > Examples > MspParser > MspComprehensiveEsp32`.

## Project Structure

The library is structured to promote clarity and maintainability:
-   **`MspParser.h` / `MspParser.cpp`**: Contains the core logic for parsing the MSP protocol, handling the state machine, and extracting raw command IDs and payloads from incoming data streams.
-   **`MspDecoder.h` / `MspDecoder.cpp`**: Provides utilities for interpreting and formatting parsed MSP data. This includes mapping command IDs to human-readable names and converting raw byte data into hexadecimal, binary, or decoded strings. This separation allows for flexible output options and keeps the core parser logic clean.
-   **`msp_protocol.h`**: A header file containing `#define` statements for known MSP command IDs, primarily sourced from Betaflight. This centralizes command definitions, making them easy to update without affecting core logic.
-   **`crc8.h` / `crc8.cpp`**: Implements the CRC8-DVB-S2 algorithm used for MSPv2 checksums.

*Note: Private member variables and internal variables within structs are prefixed with an underscore (`_`) for clarity and adherence to coding conventions.*

This separation of concerns ensures that each component has a single, well-defined responsibility, making the library easier to understand, extend, and maintain, especially in resource-constrained environments like Arduino.

## API Reference

#### `MspParser()`

The constructor. Creates a new parser instance.

#### `void begin(Stream& stream, const char* prefix)`

Attaches a `Stream` object to the parser. The parser will listen for data on this stream. You can call this up to two times to monitor two streams.

-   `stream`: The stream to listen on (e.g., `Serial`, `Serial1`).
-   `prefix`: A string identifier for this stream, which is passed to the callback.

#### `void onMessage(MspMessageCallback callback)`

Registers your callback function. This function will be executed every time a valid message is parsed.

-   `callback`: A function with the signature `void myFunc(const MspMessage& message, const char* prefix)`.

#### `void update()`

Reads available bytes from the attached streams and runs them through the state machine. Call this in your main `loop()`.

#### `void setOutputFormat(MspOutputFormat format)`

Sets the desired output format for command and payload decoding. `format` can be `MSP_FORMAT_HEX`, `MSP_FORMAT_BINARY`, or `MSP_FORMAT_DECODED`.

#### `void printFormattedCommand(Stream& stream, uint16_t command)`

Prints the formatted string representation of an MSP command ID to the specified `Stream`, based on the currently set output format.

#### `void printFormattedPayload(Stream& stream, uint16_t command, const uint8_t* payload, uint16_t payloadSize)`

Prints the formatted string representation of an MSP message payload to the specified `Stream`, based on the currently set output format.

#### `void sendMspMessage(Stream& stream, uint16_t command, const uint8_t* payload, uint16_t payloadSize, bool useMspV2 = false)`

Constructs and sends an MSP message over the specified stream. Internally, this function delegates to specialized helper functions for MSPv1 and MSPv2 message construction.

-   `stream`: The `Stream` object to send the message over.
-   `command`: The MSP command ID.
-   `payload`: A pointer to the payload data.
-   `payloadSize`: The size of the payload in bytes.
-   `useMspV2`: If `true`, sends an MSPv2 message; otherwise, sends an MSPv1 message (defaults to `false`).

#### `MspDecoder& getDecoder()`

Returns a reference to the internal `MspDecoder` instance. This allows direct access to the `MspDecoder`'s formatting utilities for more granular control over output.

#### `MspMessage` Struct

The callback function receives a `const MspMessage&` containing:

-   `char direction`: Direction of the message ('>' for FC to App, '<' for App to FC).
-   `uint16_t command`: The MSP command ID.
-   `uint16_t payloadSize`: The size of the payload in bytes.
-   `const uint8_t* payload`: A pointer to the payload data array.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
