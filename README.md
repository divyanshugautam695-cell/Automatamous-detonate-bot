# TACTIS-VR Dashboard

Responsive mobile + PC dashboard prototype for a **software-only rover digital twin**.

## Current prototype
- Responsive phone and desktop interface
- Manual virtual rover controls
- PC keyboard controls
- Autonomous virtual navigation
- Virtual target and target-reached state
- Simulated telemetry and battery
- Emergency stop in the simulation
- Software-only mission event
- Camera placeholder for a future non-destructive telemetry/camera integration

> Safety boundary: this repository's dashboard simulation does not implement or trigger any physical destructive mechanism. Any mission event is visual/software-only.

## Run locally
Open `dashboard/index.html` in a modern browser.

## Architecture

```text
PHONE / PC
    |
TACTIS-VR DASHBOARD
    |
DIGITAL-TWIN SIMULATION
    |
Future telemetry interface
    |
ESP32 / rover telemetry (non-destructive)
```

## Planned phases
1. Dashboard and digital-twin simulation
2. Camera/telemetry visualization
3. Sensor/GPS visualization
4. Real-time synchronization of rover telemetry with the digital twin
5. Competition/demo mode with physical rover limited to safe stop/indicator behavior
