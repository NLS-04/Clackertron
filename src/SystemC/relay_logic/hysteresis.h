#pragma once

#include "systemc.h"
#include "simtypes.h"

namespace hysteresis {
using namespace sim;

class HysteresisBase : public sc_behavior {
public:
    sc_event start;      // event to start ascend of hysteresis
    sc_event stop;       // event to stop ascend of hysteresis, i.e. start descend of hysteresis
    sc_event reset;      // event to reset hysteresis to initial state
    sc_out<bool> output; // output signal indicating whether the hysteresis is high or low

public:
    typedef float progress_t; // progress indicator type, normalized to [0.0, 1.0]
    enum class Direction {
        ASCENDING  = 0,
        DESCENDING = 1
    };
    enum class EventType {
        STATE_FLIP = 0,
        COMPLETED  = 1
    };
    
protected:
    void process();

    /// @brief Get the progress of the hysteresis based on its direction and elapsed time since the given progress value.
    virtual progress_t getProgressAt(Direction dir, const sc_time& elapsed, progress_t progress) const = 0;
    /// @brief Get the progress of the hysteresis based on its direction and event type.
    virtual progress_t getProgressAt(Direction dir, EventType event) const = 0;
    
    /// @brief Get the time required to reach the next state transition based on current direction, event type, and progress.
    virtual sc_time getTimeTo(Direction dir, EventType event, progress_t progress) const = 0;

private:
    using StateFunction = void (HysteresisBase::*)();
    
    StateFunction m_current_state_fn; // pointer to the current state function
    progress_t m_progress; // progress of the current transition, normalized to [0.0, 1.0]
    
    template <Direction dir>
    StateFunction phase_common(
        EventType target_event, 
        sc_event& trigger_event, 
        StateFunction on_successful, 
        StateFunction on_interrupted
    );

    void phase_idle_low();
    void phase_idle_high();
    
    void phase_ascending_trigger();
    void phase_ascending_complete();
    void phase_descending_trigger();
    void phase_descending_complete();
};


class LinearHysteresis : public HysteresisBase {
public:
    SC_CTOR( LinearHysteresis,
        sc_time ascend, 
        sc_time descend, 
        progress_t flip_up=1.0f,    // default to fully flip at the end of the ascend phase
        progress_t flip_down=0.0f   // default to fully flip at the end of the descend phase
    ) 
    : m_progress_ascend_flip(flip_up)
    , m_progress_descend_flip(flip_down)
    , m_time_ascend(ascend)
    , m_time_descend(descend)
    {
        sc_assert(ascend >= SC_ZERO_TIME && descend >= SC_ZERO_TIME);
        sc_assert(flip_up   >= 0.0f && flip_up   <= 1.0f);
        sc_assert(flip_down >= 0.0f && flip_down <= 1.0f);
        sc_assert(flip_up > flip_down); // to ensure proper hysteresis behavior
    }

protected:
    progress_t getProgressAt(Direction dir, const sc_time& elapsed, progress_t progress) const override;
    progress_t getProgressAt(Direction dir, EventType event) const override;
    sc_time getTimeTo(Direction dir, EventType event, progress_t progress) const override;

private:
    progress_t m_progress_ascend_flip;
    progress_t m_progress_descend_flip;
    
    sc_time m_time_ascend;
    sc_time m_time_descend;
};

class ExponentialHysteresis : public HysteresisBase {
public:
    SC_CTOR( ExponentialHysteresis,
        sc_time mean_life_time,               // inverse of the exponential rate
        progress_t saturation_cutoff = 0.99f, // progress value at which the state is considered fully saturated, i.e. completeted ascend
        progress_t flip_up=1.0f,
        progress_t flip_down=0.0f
    ) 
    : m_progress_ascend_flip(flip_up)
    , m_progress_descend_flip(flip_down)
    , mean_life_time(mean_life_time)
    , m_saturation_cutoff(saturation_cutoff)
    , m_saturation_time(inverse_progress_function(Direction::ASCENDING, saturation_cutoff))
    {
        sc_assert(mean_life_time > SC_ZERO_TIME);
        sc_assert(saturation_cutoff > 0.5f && saturation_cutoff < 1.0f); // to ensure proper hysteresis behavior
        sc_assert(flip_up   >= 0.0f && flip_up   <= 1.0f);
        sc_assert(flip_down >= 0.0f && flip_down <= 1.0f);
        sc_assert(flip_up > flip_down); // to ensure proper hysteresis behavior
    }

protected:
    progress_t getProgressAt(Direction dir, const sc_time& elapsed, progress_t progress) const override;
    progress_t getProgressAt(Direction dir, EventType event) const override;
    sc_time getTimeTo(Direction dir, EventType event, progress_t progress) const override;

private:
    inline progress_t progress_function(Direction dir, const sc_time& t) const;
    inline sc_time inverse_progress_function(Direction dir, progress_t p) const;
    inline sc_time time_between_progress(Direction dir, progress_t start, progress_t end) const;


    sc_time mean_life_time; // inverse of the exponential rate
    
    sc_time m_saturation_time; // time at which the progress reaches saturation_cutoff
    progress_t m_saturation_cutoff;

    progress_t m_progress_ascend_flip;
    progress_t m_progress_descend_flip;
};


}; // namespace hysteresis