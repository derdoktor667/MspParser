#include "crc8.h"

uint8_t crc8_dvb_s2(uint8_t crc, uint8_t a) {
    crc ^= a;
    for (int i = 0; i < 8; ++i) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ 0x30; // DVB-S2 polynomial
        } else {
            crc <<= 1;
        }
    }
    return crc;
}