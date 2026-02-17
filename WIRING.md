# Hardware Wiring Guide

## Overview

This guide provides detailed information about connecting the ESP32-WROVER-KIT_v4.1 to the TX335 treadmill motor controller.

## Safety Warning

⚠️ **WARNING**: This project involves working with high voltage (12-15V) and motor control. Always:
- Disconnect power before making connections
- Use proper insulation and strain relief
- Test with the treadmill belt removed first
- Have an emergency stop mechanism in place
- Verify all connections before applying power

## Required Components

1. **ESP32-WROVER-KIT_v4.1**
2. **External LDO Voltage Regulator** (12-15V to 3.3V)
   - Example: LM1117-3.3, AMS1117-3.3, or similar
   - Must handle sufficient current for ESP32 (minimum 500mA recommended)
3. **Bi-directional Level Shifter**
   - Example: TXS0108E (8-channel) or similar
   - Needed for 3.3V (ESP32) to treadmill voltage level conversion
4. **Connecting Wires and Terminal Blocks**

## Treadmill Controller Pinout

The TX335 treadmill motor controller has the following interface pins:

| Pin | Function   | Voltage Level | Direction  |
|-----|------------|---------------|------------|
| 3   | GND        | 0V            | Common     |
| 4   | PWR        | 12-15V        | Input      |
| 5   | PWM        | 12-15V        | Input      |
| 7   | ENABLE     | 12-15V        | Input      |
| 8   | MTR_PULSE  | 12-15V        | Output     |

## Connection Diagram

```
TX335 Controller                Level Shifter           ESP32-WROVER-KIT
┌──────────────┐               ┌─────────────┐        ┌──────────────┐
│              │               │             │        │              │
│ Pin 3 (GND)  ├───────────────┼─────────────┼────────┤ GND          │
│              │               │             │        │              │
│ Pin 4 (PWR)  ├───┐           │             │        │              │
│   12-15V     │   │           │             │        │              │
└──────────────┘   │           │             │        └──────────────┘
                   │           │             │
                   └──────┐    │   LV Side   │
                          │    │   (3.3V)    │
                   ┌──────┴────┤             │
                   │  LDO      │             │
                   │ (3.3V)    ├─────────────┼────────┤ 3.3V Power   │
                   └───────────┤             │        │              │
                               │             │        │              │
┌──────────────┐               │   HV Side   │        ┌──────────────┐
│              │               │  (12-15V)   │        │              │
│ Pin 5 (PWM)  ├───────────────┤ HV1 -> LV1  ├────────┤ GPIO 5       │
│              │               │             │        │              │
│ Pin 7 (EN)   ├───────────────┤ HV2 -> LV2  ├────────┤ GPIO 7       │
│              │               │             │        │              │
│ Pin 8 (PULSE)├───────────────┤ LV3 -> HV3  ├────────┤ GPIO 8       │
│              │               │             │        │              │
└──────────────┘               └─────────────┘        └──────────────┘
```

## Step-by-Step Wiring Instructions

### Step 1: Power Supply

1. Connect the treadmill's 12-15V power output to:
   - LDO input (VIN)
   - Level shifter HV (high voltage) power
2. Connect LDO output (3.3V) to:
   - Level shifter LV (low voltage) power
   - ESP32 3.3V power pin
3. Connect all grounds together:
   - TX335 Pin 3 (GND)
   - LDO GND
   - Level shifter GND
   - ESP32 GND

### Step 2: Signal Connections

1. **PWM Signal (Pin 5)**:
   - ESP32 GPIO 5 → Level Shifter LV1
   - Level Shifter HV1 → TX335 Pin 5

2. **ENABLE Signal (Pin 7)**:
   - ESP32 GPIO 7 → Level Shifter LV2
   - Level Shifter HV2 → TX335 Pin 7

3. **MTR_PULSE Signal (Pin 8)**:
   - TX335 Pin 8 → Level Shifter HV3
   - Level Shifter LV3 → ESP32 GPIO 8

### Step 3: Verification

Before applying power:
1. ✓ Verify all GND connections are secure
2. ✓ Verify power polarity (VIN/GND on LDO)
3. ✓ Verify level shifter orientation (HV side to treadmill, LV side to ESP32)
4. ✓ Verify no short circuits with multimeter
5. ✓ Ensure treadmill belt is removed for initial testing

## Testing Procedure

1. **Power-on Test**:
   - Apply power
   - Verify ESP32 boots (check serial output)
   - Verify 3.3V regulation is stable
   - LED on ESP32 should light up

2. **Signal Test** (with treadmill disconnected from load):
   - Run the demo program
   - Use oscilloscope or logic analyzer to verify:
     - PWM signal on GPIO 5
     - ENABLE signal on GPIO 7
   - Signals should be clean 3.3V levels

3. **Level Shifter Test**:
   - Verify 12-15V levels on treadmill side
   - Verify 3.3V levels on ESP32 side
   - Check with oscilloscope during operation

4. **Motor Test** (with treadmill connected):
   - Start with low speed (10-20%)
   - Verify motor responds
   - Check MTR_PULSE feedback on GPIO 8
   - Gradually increase speed if successful

## Troubleshooting

### ESP32 doesn't boot
- Check 3.3V power supply
- Verify LDO is working correctly
- Check for shorts

### Motor doesn't respond
- Verify ENABLE signal reaches treadmill
- Check PWM signal amplitude on treadmill side
- Verify level shifter is working
- Check treadmill power supply

### No pulse counting
- Verify MTR_PULSE connection
- Check level shifter direction
- Use oscilloscope to verify signal from treadmill
- Check pull-up resistor on ESP32 GPIO 8

## Component Recommendations

### LDO Regulators
- **AMS1117-3.3**: Inexpensive, 1A output
- **LM1117-3.3**: Common, 800mA output
- **LD1117V33**: Low dropout, 800mA output

### Level Shifters
- **TXS0108E**: 8-channel, auto-direction sensing
- **74LVC245**: Octal bus transceiver (requires direction control)
- **BSS138**: MOSFET-based (for individual signals)

## Additional Notes

- Always use bypass capacitors (0.1µF) near the ESP32 power pins
- Add a 10µF capacitor on LDO output for stability
- Use twisted pair or shielded cables for signal lines if experiencing noise
- Consider adding pull-up/pull-down resistors on signal lines for added noise immunity
- The demo code limits speed to 30% - adjust as needed for your application

## Schematic Symbol Reference

```
LDO:
   VIN ─┬─ [LDO] ─┬─ VOUT (3.3V)
        │         │
       GND       GND

Level Shifter (TXS0108E):
   VCCA (3.3V)  ┌─────────┐  VCCB (12-15V)
   A1 ─────────┤  TXS    ├──────── B1
   A2 ─────────┤  0108E  ├──────── B2
   A3 ─────────┤         ├──────── B3
   GND ────────┤         ├──────── GND
                └─────────┘
```

## References

- [ESP32-WROVER-KIT_v4.1 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-wrover-kit.html)
- [TXS0108E Datasheet](https://www.ti.com/product/TXS0108E)
- [ESP32 GPIO Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html)
