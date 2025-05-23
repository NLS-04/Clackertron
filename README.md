# Clackertron (CT-65)
## High-Performance 6502-Based 8-Bit Relay Computer

Relay based replication of the original SY6502 CPU by Motorola (with minimal differences for version 1.0).

<p align="center" width="100%">
    <img src="https://upload.wikimedia.org/wikipedia/commons/archive/8/8b/20210903220037%21MOS_6502_die.jpg" alt="MOS 6502 silicon die" height="300px"/>
    <!-- <img src="https://upload.wikimedia.org/wikipedia/commons/3/38/Delta_Electronics_DPS-350FB_A_-_board_1_-_OEG_SDT-SS-112M_-_case_removed-3045.jpg" alt="MOS 6502 silicon die" height="300px"/> -->
    <img src="https://github.com/davidmjc/6502/blob/master/bd.png?raw=true" alt="6502 CPU Architecture" height="300px"/>
</p>

---
# Goal
The aim of this project is to design and built a 6502-Based CPU with relays and basic electrical components.

This CPU's ISA is aimed to be as close to the original by using the same Mnemonics and behavior but utilizing a different bytecode.
This allows the user to use existing 6502 assembly with only minimal adjustment.

> The goal is to only require a translation script, which translates assembled 6502 Bytecode into CT-65 bytecode or to use the custom CT-65 assembler [**WIP**].

---
# Structure
## General
The general layout of the cpu is very similar to the original SY6502 design.

Since relays are effectively current driven devices the general design of this cpu is *active-high* and *inactive-hi-Z*, where low signals are represented as high impedance.

The CPU requires multiple voltage levels for correct operation:

| Name | Abbr. | Level | Description | bound |
|-----:|--------------|------:|-------------|:-----:|
| ground         | **GND**  | **0V**    | common ground for all components | - |
| coil retention | **Vcr**  | **+2V**   | retention voltage of the FF/registers | 20% *Vr* < *Vcr* < 75% *Vr* |
| design-voltage | **Vnom** | **+5V**   | normal relay operation voltage | relay dependant |
| high-voltage   | **Vhi**  | **+9.4V** | high voltage used for arithmetic calculations | regard derivation of paper |

> Actual voltage levels depend on *Vnom* and are currently still to be determine (WIP/TBD).


The ALU exclusively uses a special High-Voltage(*HiV* or *Vhi*) design where active-high uses a special higher voltage level than the regular design.
> This allows for highly optimized design of the underlying logic gates used in the ALU and therefor reducing the required relay count and execution time.

## Hardware
Due to the expected size requirements of the assembled hardware the CPU will be partitioned into multiple PCBs, each connected via edge-connecters to a common backplane.

## Logisim-evolution
[Logisim evolution](https://github.com/logisim-evolution/logisim-evolution) is used to construct the logic design of the whole CPU and to have a logically accurate simulation of the CPUs behavior. The signal propagation is simulated and synchronized with "clock pulses".
> see [LogiSim README](src/LogiSim/README.md) for more details

## Falstad
[Falstad](https://www.falstad.com/circuit/)'s electrical circuit simulator is **currently** used for testing and simulation of simple electrical implementations of logic circuits or general proof-of-concepts.

> see [Falstad README](src/Falstad/README.md) for more details

---
# References & Sources
- [6502 CPU enhanced layout schematic](https://github.com/davidmjc/6502/blob/master/bd.png)
- [6502 CPU (probably) original layout schematic](https://i.sstatic.net/DxwKp.jpg)
- [SY6502 family datasheet](https://www.princeton.edu/~mae412/HANDOUTS/Datasheets/6502.pdf)
- [6502 instruction set table](https://www.masswerk.at/6502/6502_instruction_set.html)

---
# License
This work is licensed under the [Creative Commons Attribution 4.0 International License (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/).

You are free to:

- **Share** — copy and redistribute the material in any medium or format  
- **Adapt** — remix, transform, and build upon the material for any purpose, even commercially  

Under the following terms:

- **Attribution** — You must give appropriate credit, provide a link to the license, and indicate if changes were made.



