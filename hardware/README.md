# TACTIS-VR Real Rover Hardware

This folder contains the safe real-world hardware setup for the TACTIS-VR project.

## Safety boundary

The physical rover has **no detonation, ignition, explosive, or destructive actuator**. The dashboard's simulation event is software-only. A future physical event command is limited to **STOP + buzzer + LED**.

## Hardware architecture

- ESP32 DevKit: motor/sensor controller
- Motor driver: drives the two DC motors
- GPS: position telemetry
- LoRa: optional long-range telemetry link
- Ultrasonic sensors: obstacle detection
- Buzzer + LED: safe event indication
- Hardware emergency-stop: independent motor-power cutoff
- Raspberry Pi + Raspberry Pi Camera: live video and optional local server

## Initial build

Start with ESP32 + motor driver + motors + buzzer + LED + emergency stop. Add GPS, LoRa, sensors, and Raspberry Pi camera after basic motion is verified.

## Safe event behavior

`SAFE_EVENT` must always result in:

1. Stop both motors.
2. Activate the buzzer for a short alert pattern.
3. Turn on the status LED.
4. Never activate any destructive device.

## Notes

Do not connect motors or other loads directly to ESP32 GPIO pins. Use an appropriate motor driver and verify power rails with a multimeter before connecting the ESP32.
