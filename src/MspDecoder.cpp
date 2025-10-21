#include "MspDecoder.h"
#include <WString.h>

MspDecoder::MspDecoder() {}

const MspCommandName MspDecoder::_commandNames[] = {
    {MSP_STATUS, "MSP_STATUS"},
    {MSP_RAW_IMU, "MSP_RAW_IMU"},
    {MSP_SERVO, "MSP_SERVO"},
    {MSP_MOTOR, "MSP_MOTOR"},
    {MSP_RC, "MSP_RC"},
    {MSP_RAW_GPS, "MSP_RAW_GPS"},
    {MSP_COMP_GPS, "MSP_COMP_GPS"},
    {MSP_ATTITUDE, "MSP_ATTITUDE"},
    {MSP_ALTITUDE, "MSP_ALTITUDE"},
    {MSP_ANALOG, "MSP_ANALOG"},
    {MSP_RC_TUNING, "MSP_RC_TUNING"},
    {MSP_PID, "MSP_PID"},
    {MSP_BOXNAMES, "MSP_BOXNAMES"},
    {MSP_PIDNAMES, "MSP_PIDNAMES"},
    {MSP_WP, "MSP_WP"},
    {MSP_BOXIDS, "MSP_BOXIDS"},
    {MSP_SERVO_CONFIGURATIONS, "MSP_SERVO_CONFIGURATIONS"},
    {MSP_NAV_STATUS, "MSP_NAV_STATUS"},
    {MSP_NAV_CONFIG, "MSP_NAV_CONFIG"},
    {MSP_MOTOR_3D_CONFIG, "MSP_MOTOR_3D_CONFIG"},
    {MSP_RC_DEADBAND, "MSP_RC_DEADBAND"},
    {MSP_SENSOR_ALIGNMENT, "MSP_SENSOR_ALIGNMENT"},
    {MSP_LED_STRIP_MODECOLOR, "MSP_LED_STRIP_MODECOLOR"},
    {MSP_VOLTAGE_METERS, "MSP_VOLTAGE_METERS"},
    {MSP_CURRENT_METERS, "MSP_CURRENT_METERS"},
    {MSP_BATTERY_STATE, "MSP_BATTERY_STATE"},
    {MSP_MOTOR_CONFIG, "MSP_MOTOR_CONFIG"},
    {MSP_GPS_CONFIG, "MSP_GPS_CONFIG"},
    {MSP_COMPASS_CONFIG, "MSP_COMPASS_CONFIG"},
    {MSP_ESC_SENSOR_DATA, "MSP_ESC_SENSOR_DATA"},
    {MSP_GPS_RESCUE, "MSP_GPS_RESCUE"},
    {MSP_GPS_RESCUE_PIDS, "MSP_GPS_RESCUE_PIDS"},
    {MSP_VTXTABLE_BAND, "MSP_VTXTABLE_BAND"},
    {MSP_VTXTABLE_POWERLEVEL, "MSP_VTXTABLE_POWERLEVEL"},
    {MSP_MOTOR_TELEMETRY, "MSP_MOTOR_TELEMETRY"},
    {MSP_SIMPLIFIED_TUNING, "MSP_SIMPLIFIED_TUNING"},
    {MSP_SET_SIMPLIFIED_TUNING, "MSP_SET_SIMPLIFIED_TUNING"},
    {MSP_CALCULATE_SIMPLIFIED_PID, "MSP_CALCULATE_SIMPLIFIED_PID"},
    {MSP_CALCULATE_SIMPLIFIED_GYRO, "MSP_CALCULATE_SIMPLIFIED_GYRO"},
    {MSP_CALCULATE_SIMPLIFIED_DTERM, "MSP_CALCULATE_SIMPLIFIED_DTERM"},
    {MSP_VALIDATE_SIMPLIFIED_TUNING, "MSP_VALIDATE_SIMPLIFIED_TUNING"},
    {MSP_STATUS_EX, "MSP_STATUS_EX"},
    {MSP_UID, "MSP_UID"},
    {MSP_GPSSVINFO, "MSP_GPSSVINFO"},
    {MSP_GPSSTATISTICS, "MSP_GPSSTATISTICS"},
    {MSP_OSD_VIDEO_CONFIG, "MSP_OSD_VIDEO_CONFIG"},
    {MSP_SET_OSD_VIDEO_CONFIG, "MSP_SET_OSD_VIDEO_CONFIG"},
    {MSP_DISPLAYPORT, "MSP_DISPLAYPORT"},
    {MSP_COPY_PROFILE, "MSP_COPY_PROFILE"},
    {MSP_BEEPER_CONFIG, "MSP_BEEPER_CONFIG"},
    {MSP_SET_BEEPER_CONFIG, "MSP_SET_BEEPER_CONFIG"},
    {MSP_SET_TX_INFO, "MSP_SET_TX_INFO"},
    {MSP_TX_INFO, "MSP_TX_INFO"},
    {MSP_SET_OSD_CANVAS, "MSP_SET_OSD_CANVAS"},
    {MSP_OSD_CANVAS, "MSP_OSD_CANVAS"},
    {MSP_SET_RAW_RC, "MSP_SET_RAW_RC"},
    {MSP_SET_RAW_GPS, "MSP_SET_RAW_GPS"},
    {MSP_SET_PID, "MSP_SET_PID"},
    {MSP_SET_RC_TUNING, "MSP_SET_RC_TUNING"},
    {MSP_ACC_CALIBRATION, "MSP_ACC_CALIBRATION"},
    {MSP_MAG_CALIBRATION, "MSP_MAG_CALIBRATION"},
    {MSP_RESET_CONF, "MSP_RESET_CONF"},
    {MSP_SET_WP, "MSP_SET_WP"},
    {MSP_SELECT_SETTING, "MSP_SELECT_SETTING"},
    {MSP_SET_HEADING, "MSP_SET_HEADING"},
    {MSP_SET_SERVO_CONFIGURATION, "MSP_SET_SERVO_CONFIGURATION"},
    {MSP_SET_MOTOR, "MSP_SET_MOTOR"},
    {MSP_SET_NAV_CONFIG, "MSP_SET_NAV_CONFIG"},
    {MSP_SET_MOTOR_3D_CONFIG, "MSP_MOTOR_3D_CONFIG"},
    {MSP_SET_RC_DEADBAND, "MSP_SET_RC_DEADBAND"},
    {MSP_SET_RESET_CURR_PID, "MSP_SET_RESET_CURR_PID"},
    {MSP_SET_SENSOR_ALIGNMENT, "MSP_SET_SENSOR_ALIGNMENT"},
    {MSP_SET_LED_STRIP_MODECOLOR, "MSP_SET_LED_STRIP_MODECOLOR"},
    {MSP_SET_MOTOR_CONFIG, "MSP_SET_MOTOR_CONFIG"},
    {MSP_SET_GPS_CONFIG, "MSP_SET_GPS_CONFIG"},
    {MSP_SET_COMPASS_CONFIG, "MSP_SET_COMPASS_CONFIG"},
    {MSP_SET_GPS_RESCUE, "MSP_SET_GPS_RESCUE"},
    {MSP_SET_GPS_RESCUE_PIDS, "MSP_SET_GPS_RESCUE_PIDS"},
    {MSP_SET_VTXTABLE_BAND, "MSP_SET_VTXTABLE_BAND"},
    {MSP_SET_VTXTABLE_POWERLEVEL, "MSP_SET_VTXTABLE_POWERLEVEL"},
    {MSP_MULTIPLE_MSP, "MSP_MULTIPLE_MSP"},
    {MSP_MODE_RANGES_EXTRA, "MSP_MODE_RANGES_EXTRA"},
    {MSP_SET_ACC_TRIM, "MSP_SET_ACC_TRIM"},
    {MSP_ACC_TRIM, "MSP_ACC_TRIM"},
    {MSP_SERVO_MIX_RULES, "MSP_SERVO_MIX_RULES"},
    {MSP_SET_SERVO_MIX_RULE, "MSP_SET_SERVO_MIX_RULE"},
    {MSP_SET_PASSTHROUGH, "MSP_SET_PASSTHROUGH"},
    {MSP_SET_RTC, "MSP_SET_RTC"},
    {MSP_RTC, "MSP_RTC"},
    {MSP_SET_BOARD_INFO, "MSP_SET_BOARD_INFO"},
    {MSP_SET_SIGNATURE, "MSP_SET_SIGNATURE"},
    {MSP_EEPROM_WRITE, "MSP_EEPROM_WRITE"},
    {MSP_RESERVE_1, "MSP_RESERVE_1"},
    {MSP_RESERVE_2, "MSP_RESERVE_2"},
    {MSP_DEBUGMSG, "MSP_DEBUGMSG"},
    {MSP_DEBUG, "MSP_DEBUG"},
    {MSP_V2_FRAME, "MSP_V2_FRAME"},
    {MSP_API_VERSION, "MSP_API_VERSION"},
    {MSP_FC_VARIANT, "MSP_FC_VARIANT"},
    {MSP_FC_VERSION, "MSP_FC_VERSION"},
    {MSP_BOARD_INFO, "MSP_BOARD_INFO"},
    {MSP_BUILD_INFO, "MSP_BUILD_INFO"},
    {MSP_NAME, "MSP_NAME"},
    {MSP_SET_NAME, "MSP_SET_NAME"}
};
const size_t MspDecoder::_commandNamesCount = sizeof(MspDecoder::_commandNames) / sizeof(MspDecoder::_commandNames[0]);

// Converts a single byte value to its hexadecimal string representation.
// The byte to convert.
// A String containing the hexadecimal representation (e.g., "0A", "FF").
void MspDecoder::printHexString(Stream& stream, uint8_t value) {
    if (value < 0x10) {
        stream.print('0'); // Add leading zero for single-digit hex values
    }
    stream.print(value, HEX);
}

// Converts a single byte value to its binary string representation.
// The byte to convert.
// A String containing the binary representation (e.g., "00001010", "11111111").
void MspDecoder::printBinaryString(Stream& stream, uint8_t value) {
    for (int i = 7; i >= 0; --i) { // Changed from 15 to 7 for uint8_t
        stream.print(((value >> i) & 1) ? '1' : '0');
    }
}

// Retrieves the human-readable name for a given MSP command ID.
// The MSP command ID.
// A String containing the command name (e.g., "MSP_STATUS") or "UNKNOWN_COMMAND" if not found.
void MspDecoder::printCommandName(Stream& stream, uint16_t command) {
    for (size_t i = 0; i < _commandNamesCount; ++i) {
        if (_commandNames[i].commandId == command) {
            stream.print(_commandNames[i].name);
            return;
        }
    }
    stream.print(F("UNKNOWN_COMMAND"));
}

// Decodes the payload of an MSP message into a human-readable string.
// Currently, it returns the hexadecimal representation of the payload.
// Future enhancements could include command-specific payload decoding.
// The MSP command ID.
// Pointer to the payload data.
// The size of the payload in bytes.
// A String containing the decoded payload information.
void MspDecoder::printDecodedPayload(Stream& stream, uint16_t command, const uint8_t* payload, uint16_t payloadSize) {
    stream.print(F("Payload (HEX): "));
    for (uint16_t i = 0; i < payloadSize; ++i) {
        if (payload[i] < 0x10) stream.print('0');
        stream.print(payload[i], HEX);
        stream.print(' ');
    }
}