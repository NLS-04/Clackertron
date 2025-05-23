<p align="center" width="100%">
    <img src="https://i.ytimg.com/vi/pGDA6ch8twc/maxresdefault.jpg" alt="Falstad Circuit Simulator Image"/>
</p>

# Falstad

[Falstad](https://www.falstad.com/circuit/)'s electrical circuit simulator is **currently** used for testing and simulation of simple electrical implementations of logic circuits or general proof-of-concepts.

> TODO: simulate in more professional simulator like LTSpice.

---
## Signal Interpretation and Voltage Levels
Since relays are effectively current driven devices the general design of this cpu is *active-high* and *inactive-hi-Z*, where low signals are represented as high impedance.

The CPU requires multiple voltage levels for correct operation:

| Name | Abbr. | Level | Description | bound |
|-----:|--------------|------:|-------------|:-----:|
| ground         | **GND**  | **0V**    | common ground for all components | - |
| coil retention | **Vcr**  | **+2V**   | retention voltage of the FF/registers | 20% *Vr* < *Vcr* < 75% *Vr* |
| design-voltage | **Vnom** | **+5V**   | normal relay operation voltage | relay dependant |
| high-voltage   | **Vhi**/*HiV*  | **+9.4V** | high voltage used for arithmetic calculations | regard derivation of paper |

> Actual voltage levels depend on *Vnom* of the used relays and are currently still to be determine (WIP/TBD).

The ALU exclusively uses a special High-Voltage(*HiV* or *Vhi*) design where active-high uses a special higher voltage level than the regular design.
> This allows for highly optimized design of the underlying logic gates used in the ALU and therefor reducing the required relay count and execution time.

---
## Files
Files are categorized in groups indicated as:
- `debug-` : circuits for debugging or testing designs
- `demo-`  : circuits demonstrating technical or electrical features
- `module-`: electrical circuits implementation of logic circuits

| File | Description |
|------|-------------|
| **> debug <**                          | |
| _debug-Incrementer-Logic_              | debug and testing designs of basic Incrementer circuit |
| **> demo <**                           | |
| _demo-2bit-ADD-SUB_                    | 2-bit adder and subtract demonstrations | 
| _demo-rising-edge-enable_ | demonstration of rising edge enable signal mechanism. Using capacitors for enabling (bus-) gate relays for the period of ca. 1 relay switching time. | 
| _demo-self-writing-D-Latch-circuit_    | example of using the rising edge enable signal mechanism displayed in _demo-rising-edge-enable_ with a DDFF (_Double-Data-Flip-Flop_) wired to itself over the inverted output with a signal retention (capacitor-) circuit | 
| **> module <**                         | |
| _module-Adder-8bit_                    | 8-bit adder circuit with bus output gates | 
| _module-Amplifier_                     | signal amplifier. Useful for boosting signal drive strength. (switching pole polarity(voltages) works as inverse amplifier) | 
| _module-D-Latch_                       | 8-bit data latch/register with simple voltage level controlled enable gates and with inverse outputs | 
| _module-Double-Data-Flip-Flop_         | 2-bit data latch element used in the _module-D-Latch_ latch/register | 
| _module-Full-Adder_                    | 1-bit optimized *HiV* full adder circuit with balanced current path design for cascaded adders | 
| _module-Full-Adder-Carry-Lookahead_    | 1-bit optimized *HiV* full adder circuit implementation with propagate and generate outputs | 
| _module-Half-Adder_                    | 1-bit optimized *HiV* half adder circuit | 
| _module-Set-Reset-Register_            | 8-bit SR-Latch/Register with inverted outputs | 
