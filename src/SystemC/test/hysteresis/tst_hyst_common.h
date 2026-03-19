#include "systemc.h"
#include "simtypes.h"
#include "hysteresis.h"

#include <gtest/gtest.h>

using namespace sc_core;
using namespace sim;
using namespace hysteresis;
using Direction = HysteresisBase::Direction;

struct HystEvent {
    sc_time time;
    HysteresisBase::Direction direction;

    void assert_eq(HystEvent const& other) const {
        EXPECT_EQ(time, other.time);
        EXPECT_EQ(direction, other.direction);
    };
    void assert_eq(HystEvent const& other, sc_time tolerance) const {
        EXPECT_NEAR(time.to_double(), other.time.to_double(), tolerance.to_double());
        EXPECT_EQ(direction, other.direction);
    };
};

struct Monitor : public sc_module {
    sc_in<bool> sense;

    std::vector<HystEvent> hyst_events;

    SC_CTOR(Monitor) {
        SC_METHOD(monitor);
        sensitive << sense;
        dont_initialize();
    }

    void monitor() {
        HystEvent event { sc_time_stamp(), sense.read() ? HysteresisBase::Direction::ASCENDING : HysteresisBase::Direction::DESCENDING };
        SC_REPORT_INFO("Monitor", (std::string("Event: time=") + sc_time_stamp().to_string() + ", direction=" + (event.direction == HysteresisBase::Direction::ASCENDING ? "ASCENDING" : "DESCENDING")).c_str());
        hyst_events.push_back(event);
    }
};


class TestBench : public sc_module {
public:
    HysteresisBase* hysteresis;
    Monitor monitor;
    sc_signal<bool> sense_sig;

    sc_event_queue trigger_ascend;
    sc_event_queue trigger_descend;

    SC_CTOR(TestBench, HysteresisBase* h) : monitor("Monitor"), sense_sig("sense_sig"), hysteresis(h) {
        monitor.sense.bind(sense_sig);
        hysteresis->output.bind(sense_sig);

        SC_METHOD(stimulus_ascend);
        sensitive << trigger_ascend;
        dont_initialize();
        
        SC_METHOD(stimulus_descend);
        sensitive << trigger_descend;
        dont_initialize();
    }

    void stimulus_ascend() {
        // SC_REPORT_INFO("TestBench", ("@ " + sc_time_stamp().to_string() + "\tStimulus: Ascend Triggered").c_str());
        hysteresis->trigger_ascend.notify(SC_ZERO_TIME); // notify the ascend trigger event
    }
    
    void stimulus_descend() {
        // SC_REPORT_INFO("TestBench", ("@ " + sc_time_stamp().to_string() + "\tStimulus: Descend Triggered").c_str());
        hysteresis->trigger_descend.notify(SC_ZERO_TIME); // notify the descend trigger event
    }

    const std::vector<HystEvent>& get_events() const {
        return monitor.hyst_events;
    }
};

int sc_main(int argc, char* argv[]) {
    sc_report_handler::set_log_file_name("test_report.log");
    sc_report_handler::set_actions("writer", SC_INFO, SC_DISPLAY);
    SC_REPORT_INFO("sc_main", "Starting hysteresis testbench");

    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}