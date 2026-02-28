#pragma once

#include "systemc.h"

namespace sim {


} // namespace sim


namespace relay_logic {

enum class EVoltageLevel {
    GND = 0,    // reference ground level
    VCR,        // VCR is the voltage level of relay coil retention
    VNOM,       // VNOM is the nominal voltage level at which the relay coil is operated
    VHIGH       // VHIGH is the voltage level, used in specialized arithmetic operations
};

enum class ELogicValue {
    LOW,
    HIGH,
    HIGH_Z
};

typedef int FanOut_t; // type for fan-out count
static constexpr FanOut_t INF_FANOUT = -1; // maximum fan-out count for a relay

struct RelayValue {
    ELogicValue logic_value = ELogicValue::HIGH_Z;
    EVoltageLevel voltage_level = EVoltageLevel::GND;
    int fan_out = INF_FANOUT; // number of max fan-out connections this relay can drive
};


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
    RelayValue_Signal() {}

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

class RelayBase : public sc_module {
public:
    virtual void coil_change_event() = 0; // event triggered when the coil pins change states

    RelayBase(sc_module_name name) : sc_module(name) {
        SC_METHOD(coil_pin_event);
        sensitive << coil[0] << coil[1];
    }

    sc_port<RelayValue_if> coil[2];

protected:
    void coil_pin_event() {
        coil_change_event();
    }
};


} // namespace relay_logic