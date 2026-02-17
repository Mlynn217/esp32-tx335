/**
 * @file tx335_treadmill.h
 * @brief Public API for TX335 treadmill control
 */

#ifndef TX335_TREADMILL_H
#define TX335_TREADMILL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set treadmill speed
 * 
 * @param speed_percent Speed percentage (0-100)
 *                      0 = stopped
 *                      100 = maximum speed
 */
void set_treadmill_speed(uint8_t speed_percent);

/**
 * @brief Enable or disable the treadmill motor
 * 
 * @param enable true to enable motor, false to disable
 */
void set_treadmill_enable(bool enable);

/**
 * @brief Get the current motor pulse count
 * 
 * @return Number of pulses counted since last reset
 */
uint32_t get_motor_pulse_count(void);

/**
 * @brief Reset the motor pulse counter to zero
 */
void reset_motor_pulse_count(void);

#ifdef __cplusplus
}
#endif

#endif // TX335_TREADMILL_H
