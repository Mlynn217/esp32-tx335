# Quick Reference Card

## GPIO Pin Mapping

| Function   | GPIO Pin | Direction | Description           |
|------------|----------|-----------|----------------------|
| PWM        | 5        | Output    | Speed control        |
| ENABLE     | 7        | Output    | Motor on/off         |
| MTR_PULSE  | 8        | Input     | Speed feedback       |
| GND        | -        | -         | Ground               |
| PWR        | -        | Input     | 12-15V (via LDO)     |

## Quick Start

```bash
# 1. Setup ESP-IDF
. $HOME/esp/esp-idf/export.sh

# 2. Build
idf.py build

# 3. Flash
idf.py -p /dev/ttyUSB0 flash monitor
```

## API Quick Reference

```c
// Control motor
set_treadmill_enable(true);              // Turn on motor
set_treadmill_speed(50);                 // Set to 50% speed
set_treadmill_speed(0);                  // Stop motor
set_treadmill_enable(false);             // Turn off motor

// Monitor feedback
uint32_t pulses = get_motor_pulse_count();  // Read pulse count
reset_motor_pulse_count();                  // Reset counter
```

## PWM Configuration

- **Frequency**: 5 kHz (adjustable)
- **Resolution**: 13-bit (0-8191)
- **Speed Range**: 0-100%
- **Timer**: LEDC_TIMER_0
- **Channel**: LEDC_CHANNEL_0

## Common Tasks

### Change PWM Frequency
Edit `main/tx335_main.c`:
```c
#define LEDC_FREQUENCY  (10000)  // 10 kHz
```

### Change GPIO Pins
Edit `main/tx335_main.c`:
```c
#define PWM_GPIO        GPIO_NUM_12
#define ENABLE_GPIO     GPIO_NUM_13
#define MTR_PULSE_GPIO  GPIO_NUM_14
```

### Disable Demo Mode
Comment out in `app_main()`:
```c
// xTaskCreate(treadmill_demo_task, "treadmill_demo", 4096, NULL, 5, NULL);
```

### Add Custom Workout
Copy example and modify:
```bash
cp examples/custom_workout.c main/
# Edit main/CMakeLists.txt to include custom_workout.c
# Update app_main() to call your task
```

## Safety Checklist

- [ ] Power disconnected before wiring
- [ ] Level shifter installed and verified
- [ ] All grounds connected together
- [ ] 3.3V power stable
- [ ] Belt removed for testing
- [ ] Emergency stop accessible
- [ ] Serial monitor active

## Troubleshooting

| Problem           | Check                          |
|-------------------|--------------------------------|
| ESP32 won't boot  | 3.3V power, check LDO          |
| Motor won't run   | ENABLE signal, PWM signal      |
| No pulse count    | MTR_PULSE connection           |
| Build error       | ESP-IDF installed and sourced  |

## Key Files

- `main/tx335_main.c` - Main application code
- `main/tx335_treadmill.h` - Public API
- `README.md` - Full documentation
- `WIRING.md` - Hardware connection guide
- `examples/custom_workout.c` - Example programs

## Important Constants

```c
#define LEDC_FREQUENCY    (5000)          // PWM frequency: 5 kHz
#define LEDC_DUTY_RES     LEDC_TIMER_13_BIT  // 13-bit PWM resolution
```

## Demo Sequence

1. Wait 2 seconds
2. Enable motor
3. Ramp 0% → 30% (2 sec intervals, 5% steps)
4. Hold 30% for 10 seconds
5. Ramp 30% → 0% (2 sec intervals, 5% steps)
6. Disable motor

## Serial Output Example

```
I (1234) TX335: ESP32 TX335 Treadmill Driver starting...
I (1235) TX335: Pin Configuration:
I (1236) TX335:   PWM (speed):   GPIO 5
I (1237) TX335:   ENABLE:        GPIO 7
I (1238) TX335:   MTR_PULSE:     GPIO 8
I (1250) TX335: ENABLE GPIO configured on pin 7 (initially disabled)
I (1260) TX335: MTR_PULSE GPIO configured on pin 8 (interrupt enabled)
I (1270) TX335: PWM configured on pin 5 at 5000 Hz
I (1280) TX335: Initialization complete
I (3290) TX335: Motor ENABLED
I (4300) TX335: Speed set to 5% (duty: 409)
```

## Additional Resources

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [ESP32 LEDC PWM](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html)
- [ESP32 GPIO](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html)

---

⚠️ **Safety First**: Always test with belt removed and at low speeds first!
