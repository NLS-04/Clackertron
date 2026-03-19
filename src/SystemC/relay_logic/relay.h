#pragma once

#include "systemc.h"
#include "simtypes.h"

namespace relay_logic {
using namespace sim;


class RelayValue_if : public sc_interface {
public:
    virtual RelayValue read() const = 0;
    virtual void write(const RelayValue& value) = 0;
};

class RelayValue_Signal : public RelayValue_if, public sc_prim_channel {
private:
    RelayValue currentValue;
    RelayValue newValue;
    sc_event valueChangedEvent;

    public:
    RelayValue_Signal() noexcept {}

    RelayValue read() const override {
        return currentValue;
    }

    void write(const RelayValue& d) override {
        newValue = d;
        if(newValue != currentValue) {
            request_update();
        }
    }

    void update() {
        if(newValue != currentValue) {
            currentValue = newValue;
            valueChangedEvent.notify(SC_ZERO_TIME);
        }
    }

    const sc_event& default_event() const {
        return valueChangedEvent;
    }
};



// class Coil : public sc_module {
// public:
//     sc_port<RelayValue_if> pin[2]; // two coil pins
//     sc_event coilChangeEvent; // event triggered when the coil pins change states

//     bool coilIsEnergized() const;

//     Coil(sc_module_name name) : coilChangeEvent(name) {
//         SC_METHOD(pinMethod);
//         sensitive << pin[0] << pin[1];
//     }

// protected:
//     sc_event pinChangeEvent; // internal event to detect pin changes

// };





// class RelayBase : public sc_module {
// public:
//     virtual void coil_change_event() = 0; // event triggered when the coil pins change states

//     RelayBase(sc_module_name name) : sc_module(name) {
//         SC_METHOD(coil_pin_event);
//         sensitive << coil[0] << coil[1];
//     }

//     sc_port<RelayValue_if> coil[2];

// protected:
//     void coil_pin_event() {
//         coil_change_event();
//     }
// };


} // namespace relay_logic