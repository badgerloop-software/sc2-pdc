#include "can_pdc.h"

#include "board_config.h"
#include "can_ids.h"

// ------------- GLOBALS -------------

volatile bool forwardAndReverse = false;

// ------------- LOCAL -------------

static uint8_t lastDriveMode = 0xFF;

static constexpr uint16_t THROTTLE_SENT_MAX = 4095U;

// ------------- PUBLIC FUNCTIONS -------------

CanPdc::CanPdc(CAN_TypeDef* canPort, CAN_PINS pins, int frequency)
    : CANManager(canPort, pins, frequency) {}

void CanPdc::readHandler(CAN_message_t msg) {
    switch (msg.id) {
        case CAN_STEERING_DIGITAL: {
            // byte 0 bit 3: direction_switch (1=fwd, 0=rev)
            bool forward_selected = ((msg.buf[0] >> 3) & 1) != 0;
            forwardAndReverse = forward_selected ? FORWARD_VALUE : REVERSE_VALUE;
            break;
        }

        case CAN_STEERING_REGEN: {
            if (msg.len >= sizeof(float)) {
                float regen_val = 0.0f;
                memcpy((void*)&regen_val, msg.buf, sizeof(float));
                if (regen_val < 0.0f) {
                    regen_val = 0.0f;
                } else if (regen_val > 1.0f) {
                    regen_val = 1.0f;
                }
                regen_in = regen_val;
            }
            break;
        }

#ifndef TEST_MODE
        case CAN_STEERING_THROTTLE: {
            uint16_t throttle_raw = 0;
            memcpy((void*)&throttle_raw, msg.buf, sizeof(uint16_t));
            acc_in_raw = throttle_raw;

            float normalized = (float)throttle_raw / (float)THROTTLE_SENT_MAX;
            if (normalized < 0.0f) {
                normalized = 0.0f;
            } else if (normalized > 1.0f) {
                normalized = 1.0f;
            }
            acc_in = normalized;
            break;
        }

        case CAN_STEERING_DRIVE_MODE: {
            if (msg.len >= 1 && msg.buf[0] != lastDriveMode) {
                lastDriveMode = msg.buf[0];
                set_eco_mode(lastDriveMode == CAN_DRIVE_MODE_ECO);
            }
            break;
        }
#endif

#ifdef TEST_MODE
        case CAN_TEST_PEDAL: {
            memcpy((void*)&acc_in, msg.buf, sizeof(float));
            break;
        }
#endif

        default:
            break;
    }
}

void CanPdc::sendPDCData() {
    sendMessage(CAN_PDC_ACC_OUT, (void*)&acc_out, sizeof(float));
    sendMessage(CAN_PDC_REGEN, (void*)&regen_brake, sizeof(float));
    sendMessage(CAN_PDC_LV_12V, (void*)&lv_12V_telem, sizeof(float));
    sendMessage(CAN_PDC_LV_5V, (void*)&lv_5V_telem, sizeof(float));
    sendMessage(CAN_PDC_LV_5V_I, (void*)&lv_5V_current, sizeof(float));
    sendMessage(CAN_PDC_CURRENT_IN, (void*)&current_in_telem, sizeof(float));
    sendMessage(CAN_PDC_BRAKE_PRESSURE, (void*)&brake_pressure_telem, sizeof(float));
    sendMessage(CAN_PDC_DIGITAL, (void*)&digital_data, sizeof(digital_data));
    sendMessage(CAN_PDC_MPH, (void*)&mph, sizeof(float));
}
