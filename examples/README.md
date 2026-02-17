# Examples

This directory contains example code demonstrating various ways to use the TX335 treadmill driver.

## Available Examples

### custom_workout.c

Demonstrates how to create custom workout programs with the treadmill. Includes three different examples:

1. **Interval Training**: A complete workout program with warm-up, intervals, and cool-down
2. **Simple Speed Control**: Basic example of changing speeds
3. **Pulse Monitoring**: How to monitor motor pulses to calculate actual speed

## Using the Examples

### Method 1: Replace Demo Task

To use any of these examples, you can replace the demo task in the main application:

1. Copy the example file to the `main/` directory:
   ```bash
   cp examples/custom_workout.c main/
   ```

2. Update `main/CMakeLists.txt` to include the new file:
   ```cmake
   idf_component_register(SRCS "tx335_main.c" "custom_workout.c"
                          INCLUDE_DIRS ".")
   ```

3. In `main/tx335_main.c`, replace or modify the demo task creation in `app_main()`:
   ```c
   // Replace:
   xTaskCreate(treadmill_demo_task, "treadmill_demo", 4096, NULL, 5, NULL);
   
   // With:
   xTaskCreate(custom_workout_task, "custom_workout", 4096, NULL, 5, NULL);
   ```

4. Add the function declaration at the top of `tx335_main.c`:
   ```c
   extern void custom_workout_task(void *pvParameters);
   ```

### Method 2: Create Your Own Application

You can also copy the example as a starting point for your own application:

1. Copy the example file to `main/` with a new name:
   ```bash
   cp examples/custom_workout.c main/my_workout.c
   ```

2. Modify the code to suit your needs

3. Update CMakeLists.txt to include your file

4. Update `tx335_main.c` to call your task

## Example Details

### Interval Training Program

The interval training example includes:
- 5-minute warm-up at 20% speed
- 2-minute moderate pace at 40% speed
- 1-minute high intensity at 60% speed
- 2-minute moderate pace at 40% speed
- 5-minute cool-down at 20% speed

Modify the `workout_program` array to create your own custom workout:

```c
static const workout_interval_t my_custom_workout[] = {
    {15, 180, "Easy warm-up"},
    {30, 300, "Steady state"},
    {50, 120, "High intensity"},
    {25, 240, "Recovery"},
    {10, 120, "Cool-down"},
    {0,  0,   NULL}  // Always end with this marker
};
```

### Speed Control

The simple speed control example shows how to:
- Enable the motor
- Set different speeds
- Monitor pulse counts
- Safely stop the motor

### Pulse Monitoring

The pulse monitoring example demonstrates:
- Resetting the pulse counter
- Reading pulse counts at intervals
- Calculating pulses per second
- Using feedback for speed monitoring

## Safety Notes

⚠️ **Important Safety Considerations**:

- Always test with the treadmill belt removed first
- Start with low speeds (10-20%) and gradually increase
- Implement emergency stop functionality in production code
- Monitor pulse feedback to detect motor stalls
- Add timeout mechanisms for fail-safe operation
- Never leave a running treadmill unattended during testing

## Creating Your Own Examples

When creating your own treadmill control applications, consider:

1. **Progressive Speed Changes**: Ramp speed up/down gradually rather than sudden changes
2. **Error Handling**: Check return values and handle errors appropriately
3. **Pulse Monitoring**: Use motor pulse feedback to verify operation
4. **User Input**: Add buttons or other input methods for manual control
5. **Display Output**: Add a display to show speed, time, distance, etc.
6. **WiFi/Bluetooth**: Add wireless connectivity for remote control or data logging

## Additional Resources

- See `WIRING.md` for hardware connection details
- See `README.md` for build and installation instructions
- Refer to `main/tx335_treadmill.h` for complete API documentation

## Contributing

If you create useful examples, consider contributing them back to the project!
