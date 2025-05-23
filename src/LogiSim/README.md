<p align="center" width="100%">
    <img src="https://baillifard.com/logisim/img-guide/logisim.png" alt="Logisim-evolution-logo"/>
</p>

# Logisim-Evolution

Logisim evolution is used to construct the logic design of the whole CPU and to have a logically accurate simulation of the CPUs behavior. The signal propagation is simulated and synchronized with "clock pulses". 

---
## Files
- `Clackertron_sync.circ` main CPU circuit design
- `demo-Full_Adder_Logic.circ` demonstration of the possible *FA* designs for relay logic
- `Relay_sync.circ` synchronized relay logic implementation

> Deprecated:
> - ~~`Clackertron.circ` deprecated non synchronized CPU circuit design~~
> - ~~`Clackertron - old copy.circ` deprecated circuit version~~
> - ~~`Relay_async.circ` deprecated non synchronized relay package~~

---
## Design Considerations
Some design consideration are in place to streamline and communicate information.

- **Logic buffer**: are symbolic for electrical diodes to prohibit reverse current flow and therefor signal flow
- **Logic OR gates**: since signals are effectively current driven OR gates can be implemented as electrical net joints, i.e. by electrically connect to nets with each over.
    > ! Warning ! During logic design keep in mind that OR gate inputs will be connected electrically in hardware and therefore will cause "signal crosstalk" if no reverse current blocking diodes are placed before each input.

---
## Packages
### Clackertron_sync
Main implementation of the CPU.

Core components:

| State | Module | Width | Description |
|:-----:|--------|-------|-------------|
|  ✅  | MUX    | 8-Bit | Multiplexer |
|  ✅  | DEMUX  | 8-Bit | Demultiplexer |
|  ✅  | D-FF   | 1-Bit | Data-Flip-Flop |
|  ✅  | D-Reg  | 8-Bit | Data-Register |
|  ✅  | FA     |       | Full Adder (*HiV*) |
|  ✅  | Adder  | 8-Bit | Adder |
|  ✅  | AU     | 8-Bit | (Arithmetic Unit) Adder/Subtractor |
|  ✅  | LU     | 8-Bit | Logic Unit |
|  ✅  | ALU    | 8-Bit | Arithmetic Logic Unit
|  ✅  | INC    | 8-Bit | Incrementer |
|  ✅  | INC/DEC | 8-Bit | Combined Incrementer and Decrementer |
|  ✅  | INC-PC | 16-Bit | Special Incrementer for the program counter |
|  ✅  | PC     | 16-Bit | Program counter |
|  ✅  | P-Reg  | (8-Bit) | Status Register |
|  WIP  | Sequencer(?) |  ---  | Controls all control signals in sequence for a given opcode |
|  WIP  | Inst-Dec     |  ---  | Instruction Decoder - selects the opcode to execute and sets up sequencer |
|  TODO | Branching    |  ---  | Jump and Branch Executer |
|  TODO | BUS Layout   |  ---  | Data-Bus + Address-Bus + Special-Bus(?) + interconnection |
|  TODO | IR           | 8-Bit | Instruction Register |
|  TODO | Ring-Counter |  ---  | Timing Ticks Generator
|  TODO | Bus-Driver   | 8/16-Bit | I/O Data and address bus driver |
|  TBD? | EAR/T-Reg    | 8-Bit | Effective-Adress-Register or Temporary-Register for temporary (adress-) byte storage (TBD if required and how to implement) |
| TBD? | BCD-Adder-Adjuster | --- | Binary Coded Decimal Adjuster |


### Relay_sync
Relay_sync is a custom package which correctly mimics the logical behavior of *DPDT* and *SPDT* relays.
Transition delays caused by the energization of the coil and subsequent movement of the pole are simulated by requiring a rising "clock pulse". Such abstraction allows for accurate simulation of signal propagation through cascaded relay circuits.
The transition delays from *low* to *high* signals and vice-versa are assumed to be equivalent and therefore are simulated with 1 "clock tick" each.

Due to the limitation of logisim-evolution of only having mono-directional ports for in- and output we require 2 implementations of the *SPDT* relay and 4 for the *DPDT* relay for each I/O combination.

> TODO: Add further Explanation of logic design of the relays
