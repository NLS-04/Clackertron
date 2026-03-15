#pragma once

namespace sim {

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
    // int fan_out = INF_FANOUT; // number of max fan-out connections this relay can drive
    
    bool operator==(RelayValue const&) const = default;
};


} // namespace sim