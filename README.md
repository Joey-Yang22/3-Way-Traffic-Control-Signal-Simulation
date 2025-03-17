# 3-Way Traffic Control Signal Simulation 🚦

An embedded systems project for EE 128 simulating a 3-way traffic intersection using a K64F microcontroller and two Arduino units for pedestrian crosswalk control. The system dynamically adjusts signal timing based on real-time vehicle presence and pedestrian input to optimize traffic flow and enhance safety.

## 🔧 Project Overview

- **Main Controller:** K64F Microcontroller (Kinetis Design Studio)
- **Crosswalk Control:** Two Arduino microcontrollers for handling pedestrian signals
- **Sensor Input:** Phototransistors connected to ADC channels to detect vehicle presence in different lanes
- **Modes:** Supports left-turn priority, dynamic signal duration, and walk request handling

## ⚙️ Features

- Adaptive traffic light control using vehicle detection (phototransistors + ADC)
- Left-turn light activation logic with timed yellow/red transitions
- Pedestrian walk signal system with button request handling
- Real-time cross-controller communication between K64F and Arduino units
- GPIO and ADC-based signal control
- Energy-efficient cycle management with dynamic countdown logic

## 📷 Demo

https://www.youtube.com/watch?v=5-wGfDezM0A

## 👨‍💻 Contributors

- Jonathon Gomez
- Joey Yang

## 🛠 Tools & Technologies

- Kinetis Design Studio
- Embedded C (Low-level register programming)
- Arduino IDE (C/C++)
- Analog-to-Digital Conversion (ADC)
- GPIO control and software delay functions

## 📌 Future Enhancements

- Implement real-time clock-based scheduling
- Add serial communication between K64F and Arduinos
- Expand to 4-way intersection logic
- Integrate LCD display for status output
