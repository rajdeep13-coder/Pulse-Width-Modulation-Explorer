# ATmega328P PWM Demo - LED Fade & Servo Control

![License](https://img.shields.io/badge/license-MIT-green)
![Language](https://img.shields.io/badge/language-C-blue)
![MCU](https://img.shields.io/badge/MCU-ATmega328P-red)
![Status](https://img.shields.io/badge/status-complete-brightgreen)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

A comprehensive embedded firmware project demonstrating advanced PWM techniques on the ATmega328P microcontroller using hardware timers and interrupt-driven timing.

## 📋 Objective

This project provides a production-ready C implementation of two distinct PWM control modes:

1. **LED Fade Control** - Smooth brightness modulation using Timer0 (8-bit Fast PWM)
2. **Servo Positioning** - RC servo control using Timer1 (16-bit Fast PWM with ICR1 TOP)

The firmware implements **non-blocking, interrupt-driven event loop architecture** using Timer2 as a millisecond counter, allowing the CPU to remain responsive for additional tasks while maintaining precise PWM outputs.

### Key Learning Outcomes

- Direct register manipulation for AVR timer configuration
- Fast PWM mode implementation on 8-bit and 16-bit timers
- Interrupt Service Routine (ISR) design patterns
- Non-blocking timing techniques using static state machines
- Prescaler selection for optimal frequency and resolution

---

## 🎯 Features

| Feature | Implementation |
|---------|-----------------|
| **LED Fade** | Timer0 (8-bit), 976.56 Hz PWM, OC0A (PD6) |
| **Servo Control** | Timer1 (16-bit), 50 Hz PWM, OC1A (PB1) |
| **Timing Base** | Timer2 ISR, 1 ms tick counter |
| **Architecture** | Non-blocking event loop |
| **CPU Efficiency** | Free for additional tasks |
| **Zero External Dependencies** | Direct AVR register access |

---

## 🔌 Hardware Connections

### Microcontroller: ATmega328P (DIP-28)

```
    +-----+
PB5 |  1  o------ RESET (with capacitor to GND)
PB3 |  2  |
PB4 |  3  |
GND |  4  |--- GND (common with all GND pins)
...  ... ...
 VCC |  8  |--- +5V (with 100nF bypass capacitor)
...  ... ...
PD6 |  6  o------ LED OUTPUT (OC0A)
PB1 |  7  o------ SERVO OUTPUT (OC1A)
...  ... ...
     +-----+

XTAL Connections (16 MHz external crystal):
Pin 9  (PB6) --- Crystal --- 22 pF capacitor --- GND
Pin 10 (PB7) --- Crystal --- 22 pF capacitor --- GND
```

### LED Fade Circuit (PD6 / OC0A)

```
┌─ +5V (via USB or power supply)
│
├─── 220Ω resistor
├─┐
│ │  LED Anode
│ ◄►
│ │  LED Cathode
├─┘
│
├─── PD6 (OC0A pin on ATmega328P)
│
└─── GND (common with MCU ground)
```

**Alternative (for higher power LED or multiple LEDs):**

```
┌─ +5V (via power supply)
│
├─── 1kΩ resistor (gate resistor)
├─── Gate of N-channel MOSFET (2N7000 or similar)
│
│ Drain ──┬─── LED circuit (+5V) --- LED Anode --- Cathode
│         └─── Feedback resistor (optional)
│
│ Source ──── GND
│
├─── PD6 (OC0A)
│
└─── GND
```

### Servo Control Circuit (PB1 / OC1A)

```
Typical RC Servo:

┌──────────────────┐
│   RC SERVO       │
│  ┌──────────────┤
│  │ Signal (PWM) ├─── PB1 (OC1A)
│  │ GND          ├─── GND (common)
│  │ +5V (Power)  ├─── +5V (dedicated power supply)
│  └──────────────┤
└──────────────────┘

     ┌─────────────────────┐
     │   PWM Pulse Info    │
     ├─────────────────────┤
     │ 1.0 ms  → 0°        │
     │ 1.5 ms  → 90°       │
     │ 2.0 ms  → 180°      │
     │ Period  → 20 ms     │
     │ Freq    → 50 Hz     │
     └─────────────────────┘
```

**Power Considerations:**
- Standard servo draws 200-300 mA when under load
- Use dedicated 5V power supply (≥500 mA)
- Connect servo GND to MCU GND (common reference)
- Add 100µF capacitor on servo power for current spikes

### Complete Breadboard Wiring

```
ATmega328P Connections:

VCC (Pin 8)   ──── +5V (with 100nF bypass cap to GND)
GND (Pin 4)   ──── GND (power supply common)
GND (Pin 11)  ──── GND (power supply common)

Crystal (16 MHz):
  Pin 9  (PB6) ──┬── Crystal Xtal1
                 └── 22pF cap to GND
  Pin 10 (PB7) ──┬── Crystal Xtal2
                 └── 22pF cap to GND

LED Fade Output:
  Pin 6 (PD6)  ──── 220Ω resistor ──── LED Anode
  GND          ──── LED Cathode

Servo Control Output:
  Pin 7 (PB1)  ──── Servo Signal (PWM)
  GND          ──── Servo GND (common reference)
  +5V (sep)    ──── Servo Power (dedicated 500mA+ supply)

ICSP/Programming Header (optional for debugging):
  Pin 1  (MISO) ──── PB4
  Pin 2  (VCC)  ──── +5V
  Pin 3  (SCK)  ──── PB5
  Pin 4  (MOSI) ──── PB3
  Pin 5  (RESET)──── RESET (with 10kΩ pull-up + programming cap)
  Pin 6  (GND)  ──── GND
```

---

## 🛠️ Compilation & Programming

### Prerequisites

```bash
# Install AVR GCC toolchain (Windows)
# Download from: https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers

# Or use package manager:
# Windows (MSYS2): pacman -S mingw-w64-x86_64-avr-gcc
# Ubuntu/Debian: sudo apt-get install gcc-avr binutils-avr avr-libc
# macOS: brew install avr-gcc
```

### Compilation

```bash
# Basic compilation
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o pwm_demo.elf pwm_demo.c

# With optimization and warnings
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -O2 -std=c99 -Wall -Wextra \
  -o pwm_demo.elf pwm_demo.c

# Generate hex file for programming
avr-objcopy -O ihex pwm_demo.elf pwm_demo.hex

# Check program size
avr-size pwm_demo.elf
# Output example:
#    text    data     bss     dec     hex filename
#     876      16       2     894     37e pwm_demo.elf
```

### Programming (Using USBASP)

```bash
# Program flash memory
avrdude -c usbasp -p m328p -U flash:w:pwm_demo.hex:i

# Set fuses for 16 MHz external crystal
avrdude -c usbasp -p m328p -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0x05:m

# Verify programming
avrdude -c usbasp -p m328p -v
```

### Programming (Using Arduino as ISP)

```bash
# If using Arduino as an ISP programmer
avrdude -c arduino -P COM3 -b 19200 -p m328p -U flash:w:pwm_demo.hex:i
# Replace COM3 with your serial port
```

---

## PWM Configuration Details

### Timer0 - LED Fade (8-bit Fast PWM)

| Parameter | Value | Notes |
|-----------|-------|-------|
| Mode | Fast PWM (Mode 3) | WGM0[1:0] = 11 |
| Prescaler | 64 | CS0[2:0] = 011 |
| Clock Frequency | 16 MHz | |
| PWM Frequency | 976.56 Hz | 16M / (64 × 256) |
| Output Pin | OC0A (PD6) | Non-inverting mode |
| Duty Cycle Range | 0-255 | Controlled by OCR0A |
| Update Rate | Every 8 ms | Via Timer2 ISR |

**Why Prescaler 64?**
- 976.56 Hz is well above flicker fusion threshold (~50-100 Hz)
- Human eye perceives PWM as continuous brightness (not flickering)
- Lower prescalers (8) risk visible flicker at persistence of vision
- Higher prescalers (256) acceptable but less smooth

### Timer1 - Servo Control (16-bit Fast PWM with ICR1)

| Parameter | Value | Notes |
|-----------|-------|-------|
| Mode | Fast PWM with ICR1 as TOP (Mode 14) | WGM1[3:0] = 1110 |
| Prescaler | 64 | CS1[2:0] = 011 |
| Clock Frequency | 16 MHz | |
| PWM Frequency | 50 Hz | 16M / (64 × 5000) |
| TOP Value (ICR1) | 4999 | Defines 20 ms period |
| Timer Resolution | 4 µs/count | 64 / 16M |
| Output Pin | OC1A (PB1) | Non-inverting mode |
| Pulse Width Range | 250-500 counts | 1.0-2.0 ms |
| Angle Range | 0° to 180° | Standard RC servo |

**Servo Pulse Widths:**
```
0°   (min):  1.0 ms = 250 counts   → OCR1A = 250
90°  (mid):  1.5 ms = 375 counts   → OCR1A = 375
180° (max):  2.0 ms = 500 counts   → OCR1A = 500

Formula: OCR1A = 250 + (angle / 180) × 250
```

**Why Prescaler 64?**
- Timer tick is 4 µs/count, improving position granularity
- For 20 ms period: ICR1 = (20 ms / 4 µs) - 1 = 4999
- 1-2 ms servo window maps to 250-500 counts (251 usable steps)
- Gives smoother motion than coarse 16 µs/count timing

### Timer2 - Millisecond Counter (CTC Mode, ISR-driven)

| Parameter | Value | Notes |
|-----------|-------|-------|
| Mode | CTC (Clear Timer on Compare Match) | WGM2[1:0] = 10 |
| Prescaler | 64 | CS2[2:0] = 110 |
| Clock Frequency | 16 MHz | |
| Compare Value (OCR2A) | 249 | Triggers every 1 ms |
| Interrupt | TIMER2_COMPA | Fires every 1 ms |
| Counter Type | 8-bit | Wraps at 256 |
| Tick Period | 1 ms | Actual: 1.024 ms |

**Timer2 Calculation:**
```
Tick Period = (OCR2A + 1) × Prescaler / F_CPU
            = 250 × 64 / 16,000,000
            = 1.024 ms ≈ 1 ms
```

---


## Usage

### Compile and Program

```bash
# Step 1: Compile
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o pwm_demo.elf pwm_demo.c

# Step 2: Generate hex file
avr-objcopy -O ihex pwm_demo.elf pwm_demo.hex

# Step 3: Program ATmega328P
avrdude -c usbasp -p m328p -U flash:w:pwm_demo.hex:i

# Step 4: Power on and observe:
#   - LED on PD6 fading smoothly 0-255-0 continuously
#   - Servo on PB1 sweeping 0°-180°-0° continuously
```

### Behavior

| Component | Behavior | Duration |
|-----------|----------|----------|
| LED Fade | Ramps 0→255 then 255→0 continuously | ~8 seconds per full cycle |
| Servo Sweep | Sweeps 0°→180°→0° continuously | ~15 seconds per full cycle |
| Timing | Interrupt-driven 1 ms ticks | Background, CPU-independent |

---

## 📖 Documentation

For detailed technical documentation, refer to **[PWM_DOCUMENTATION.txt](PWM_DOCUMENTATION.txt)**:

- Complete register analysis with bit-level explanations
- Frequency calculations and derivations
- Prescaler selection rationale
- Hardware setup diagrams
- Servo pulse timing formulas
- Compilation commands with all flags
- Debugging troubleshooting guide
- Advanced configuration options

---

## 🔧 File Structure

```
project/
├── pwm_demo.c                    # Complete firmware source code
├── README.md                     # This file (quick start & overview)
└── PWM_DOCUMENTATION.txt         # Detailed technical reference
```

---

## Register Summary (Quick Reference)

### Timer0 (LED Fade)

```c
TCCR0A = 0xA3;  // COM0A=10 (non-inverting), WGM0=11 (Fast PWM)
TCCR0B = 0x03;  // CS0=011 (prescaler 64)
OCR0A   = 0-255; // Duty cycle
```

### Timer1 (Servo)

```c
TCCR1A = 0xA2;  // COM1A=10 (non-inverting), WGM1[1:0]=10
TCCR1B = 0x1B;  // WGM1[3:2]=11, CS1=011 (prescaler 64)
ICR1    = 4999;  // TOP (20 ms period, 50 Hz)
OCR1A   = 250-500; // Pulse width (1.0-2.0 ms)
```

### Timer2 (Millis Counter)

```c
TCCR2A = 0x02;  // WGM2=10 (CTC mode)
TCCR2B = 0x04;  // CS2=100 (prescaler 64)
OCR2A   = 249;   // 1 ms tick
TIMSK2 |= 0x02; // Enable COMPA interrupt
```

---

## 🐛 Troubleshooting

| Issue | Solution |
|-------|----------|
| LED not fading | Check PD6 connection, verify TCCR0A/TCCR0B settings |
| Servo not responding | Verify PB1 connection, check OCR1A range (250-500) |
| Incorrect frequency | Confirm F_CPU definition, verify crystal oscillator |
| Program won't compile | Ensure avr-gcc is installed: `avr-gcc --version` |
| Device won't program | Check USBASP driver, verify fuse settings |

---

## 📚 References

- **Atmel ATmega328P Datasheet** - Timer configuration and registers
- **AVR Instruction Set Manual** - Timer/Counter operation
- **AVR Libc Manual** - C library for AVR microcontrollers

---

## 📄 License

This project is released under the **MIT License** - free for educational and commercial use.

---

## 💡 Applications & Extensions

This codebase serves as a foundation for:

- **Multi-axis servo control** (add Timer1B on OC1B)
- **Advanced lighting effects** (breathing, color mixing)
- **Motor speed control** (PWM-driven DC motors)
- **Power management** (frequency modulation)
- **BLDC motor commutation** (synchronized PWM outputs)

---

**Happy PWM coding! 🎉**

For questions or improvements, refer to the detailed documentation file.
