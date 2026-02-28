# Clackertron (CT-65)
## High Performance Intel 4004 4-Bit Relay Computer

Relay based replication of the first comercially available cpu, the intel 4004.

<p align="center" width="100%">
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/32/Chip_layout_from_the_development_phase_of_the_Intel_4004_from_1971%2C_the_first_microprocessor_of_the_world_%28cropped_and_edited_image%29.jpg/1280px-Chip_layout_from_the_development_phase_of_the_Intel_4004_from_1971%2C_the_first_microprocessor_of_the_world_%28cropped_and_edited_image%29.jpg" alt="Intel 4004 silicon die" height="300px"/>
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/8/87/4004_arch.svg/1280px-4004_arch.svg.png" alt="4004 CPU Architecture" height="300px"/>
</p>

---
# Goal
The aim of this project is to design and built a Intel 4004 CPU with relays and basic electrical components.

<!--
This CPU's ISA is aimed to be as close to the original by using the same Mnemonics and behavior but utilizing a different bytecode.
This allows the user to use existing 6502 assembly with only minimal adjustment.

> The goal is to only require a translation script, which translates assembled 6502 Bytecode into CT-65 bytecode or to use the custom CT-65 assembler [**WIP**].
-->
---
# Structure
## General
The general layout of the cpu is very similar to the original Intel 4004 design.

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

## SystemC
[SystemC](https://github.com/accellera-official/systemc) is used as a prototyping and validation platform. The relay based CPU architecture is implemented in SystemC RTL.
Electrical verfication may be implemented in SystemC AMS or alternatively in [Falstad](https://www.falstad.com/circuit/).

### Falstad
[Falstad](https://www.falstad.com/circuit/)'s electrical circuit simulator is **currently** used for testing and simulation of simple electrical implementations of logic circuits or general proof-of-concepts.

> see [Falstad README](src/Falstad/README.md) for more details

<!--
## Logisim-evolution
[Logisim evolution](https://github.com/logisim-evolution/logisim-evolution) is used to construct the logic design of the whole CPU and to have a logically accurate simulation of the CPUs behavior. The signal propagation is simulated and synchronized with "clock pulses".
> see [LogiSim README](src/LogiSim/README.md) for more details
-->

---
# References & Sources
- [Intel 4004 Datasheet](https://datasheets.chipdb.org/Intel/MCS-4/datashts/intel-4004.pdf)
- [MCS4 Reference Datasheets](https://datasheets.chipdb.org/Intel/MCS-4/datashts/MCS4_Data_Sheet_Nov71.pdf)
- [MCS4 User Manual](https://www.bitsavers.org/components/intel/MCS4/MCS-4_UsersManual_Feb73.pdf)
- [MCS4 Assembly Language Programming Manual](https://bitsavers.trailing-edge.com/components/intel/MCS4/MCS-4_Assembly_Language_Programming_Manual_Dec73.pdf)
- [4004 Schematic](https://datasheets.chipdb.org/Intel/MCS-4/4004_schematic.pdf)
  
---
# License
This work is licensed under the [Creative Commons Attribution 4.0 International License (CC BY 4.0)](https://creativecommons.org/licenses/by/4.0/).

You are free to:

- **Share** — copy and redistribute the material in any medium or format  
- **Adapt** — remix, transform, and build upon the material for any purpose, even commercially  

Under the following terms:

- **Attribution** — You must give appropriate credit, provide a link to the license, and indicate if changes were made.



