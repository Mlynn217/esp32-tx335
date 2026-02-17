# ESP32 TX335 Treadmill Driver

Headless driver for the TX335 treadmill using ESP32-WROVER-KIT_v4.1.

## Overview

This project implements a headless treadmill controller that replaces the original console. The ESP32 interfaces with the treadmill motor controller using PWM for speed control, enable signals, and motor pulse feedback.

## Hardware Requirements

- **ESP32-WROVER-KIT_v4.1**
- **External LDO** (for power regulation)
- **Level Shifter** (for voltage level conversion between ESP32 3.3V and treadmill 12-15V signals)
- **TX335 Treadmill** motor controller interface

## Pin Configuration

The following connections are required between the ESP32 and the treadmill controller:

| Pin | Function | ESP32 GPIO | Direction | Description |
|-----|----------|------------|-----------|-------------|
| 3   | GND      | GND        | -         | Ground connection |
| 4   | PWR      | External   | Input     | 12-15V Power (via external LDO) |
| 5   | PWM      | GPIO 5     | Output    | PWM signal for speed control |
| 7   | ENABLE   | GPIO 7     | Output    | Motor enable/disable signal |
| 8   | MTR_PULSE| GPIO 8     | Input     | Motor pulse feedback (speed monitoring) |

**Important:** Use an external LDO to regulate the 12-15V power input to 3.3V for the ESP32, and use a level shifter for the GPIO signals to safely interface between 3.3V (ESP32) and the treadmill's voltage levels.

## Features

- **PWM Speed Control**: Adjustable speed control using PWM (0-100%)
- **Motor Enable/Disable**: Safe motor control with enable signal
- **Pulse Counting**: Real-time motor speed feedback via pulse counting
- **Interrupt-driven**: Efficient pulse counting using GPIO interrupts
- **Demo Mode**: Built-in demonstration of treadmill control

## Building the Project

### Prerequisites

1. Install ESP-IDF (Espressif IoT Development Framework):
   ```bash
   # Follow the official guide at:
   # https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/
   ```

2. Set up the ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

### Build Commands

```bash
# Configure the project (optional, use defaults)
idf.py menuconfig

# Build the project
idf.py build

# Flash to the ESP32
idf.py -p /dev/ttyUSB0 flash

# Monitor the output
idf.py -p /dev/ttyUSB0 monitor

# Or combine flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor
```

## Usage

### API Functions

The following functions are available for controlling the treadmill:

```c
/**
 * Set treadmill speed (0-100%)
 */
void set_treadmill_speed(uint8_t speed_percent);

/**
 * Enable or disable the treadmill motor
 */
void set_treadmill_enable(bool enable);

/**
 * Get the current motor pulse count
 */
uint32_t get_motor_pulse_count(void);

/**
 * Reset the motor pulse counter
 */
void reset_motor_pulse_count(void);
```

### Example Usage

```c
// Enable the motor
set_treadmill_enable(true);

// Set speed to 50%
set_treadmill_speed(50);

// Wait and check pulses
vTaskDelay(pdMS_TO_TICKS(5000));
uint32_t pulses = get_motor_pulse_count();
ESP_LOGI(TAG, "Motor pulses: %lu", pulses);

// Stop the motor
set_treadmill_speed(0);
set_treadmill_enable(false);
```

## Demo Mode

The project includes a built-in demo that:
1. Enables the motor
2. Gradually ramps up speed to 30%
3. Holds the speed for 10 seconds
4. Gradually ramps down to 0%
5. Disables the motor

You can modify or remove the demo task in `tx335_main.c` for your custom application.

## Customization

### Changing GPIO Pins

Edit the pin definitions in `main/tx335_main.c`:

```c
#define PWM_GPIO        GPIO_NUM_5      // PWM output
#define ENABLE_GPIO     GPIO_NUM_7      // Enable output
#define MTR_PULSE_GPIO  GPIO_NUM_8      // Motor pulse input
```

### Adjusting PWM Frequency

Modify the PWM frequency in `main/tx335_main.c`:

```c
#define LEDC_FREQUENCY  (5000)  // 5 kHz (default)
```

### PWM Resolution

The default PWM resolution is 13-bit (0-8191). You can adjust this in:

```c
#define LEDC_DUTY_RES   LEDC_TIMER_13_BIT
```

## Safety Considerations

- **Always test with the treadmill unloaded first**
- Ensure proper voltage level conversion with the level shifter
- Verify all connections before powering on
- Implement emergency stop functionality in your application
- The motor pulse feedback can be used to detect stall conditions
- Add watchdog timers for production use

## Troubleshooting

### Motor not responding
- Check enable signal connection (GPIO 7)
- Verify level shifter is working correctly
- Ensure PWM signal is reaching the motor controller

### No pulse counting
- Verify MTR_PULSE connection (GPIO 8)
- Check pull-up resistor configuration
- Monitor GPIO interrupt handler with debug logs

### Build errors
- Ensure ESP-IDF is properly installed and sourced
- Check that all required components are available
- Verify CMakeLists.txt configuration

## License

This project is provided as-is for use with the TX335 treadmill.

## Contributing

Contributions are welcome! Please ensure any changes maintain compatibility with the ESP32-WROVER-KIT_v4.1 and TX335 treadmill interface.
