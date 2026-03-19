#include "hysteresis.h"

namespace hysteresis {
using progress_t = HysteresisBase::progress_t;
using Direction = HysteresisBase::Direction;
using EventType = HysteresisBase::EventType;

//------------------------------------------------------------------------------
// HysteresisBase
//------------------------------------------------------------------------------

void HysteresisBase::process() {
    SC_REPORT_INFO(this->name(), "Hysteresis process started");
    output->write(false); // initialize output to low

    while ( true ) {
        // call the current state function
        (this->*m_current_state_fn)(); 
    }
}

void HysteresisBase::phase_idle_low() {
    // State 0: Idle low, wait for start event
    wait(trigger_ascend); // wait for start event
    SC_REPORT_INFO(this->name(), ("@ " + sc_time_stamp().to_string() + "\t[Idle Low] Hysteresis ascending triggered").c_str());
    m_progress = 0.0f; // reset progress at the start of each cycle
    
    // goto next phase
    m_current_state_fn = &HysteresisBase::phase_ascending_trigger;
}
void HysteresisBase::phase_idle_high() {
    // State 3: Idle high, wait for stop event
    wait(trigger_descend); // wait for stop event
    SC_REPORT_INFO(this->name(), ("@ " + sc_time_stamp().to_string() + "\t[Idle High] Hysteresis descending triggered").c_str());
    m_progress = 1.0f; // reset progress at the stop of each cycle
    
    // goto next phase
    m_current_state_fn = &HysteresisBase::phase_descending_trigger;
}

template <HysteresisBase::Direction dir>
HysteresisBase::StateFunction HysteresisBase::phase_common(
    EventType target_event, 
    sc_event& trigger_event, 
    StateFunction on_successful, 
    StateFunction on_interrupted
) {
    constexpr auto dir_str = (dir == Direction::ASCENDING) ? "ASCENDING" : "DESCENDING";
    const char* event_str = (target_event == EventType::STATE_FLIP) ? "STATE_FLIP" : "COMPLETED";
    auto head = [&] (sc_time t) -> std::string { return "@ " + t.to_string() + "\t[" + std::string(dir_str) + " - " + event_str + "] progress: " + std::to_string(m_progress) + ": "; };
    
    SC_REPORT_INFO(this->name(), (head(sc_time_stamp()) + "State Change, wait for " + getTimeTo(dir, target_event, m_progress).to_string()).c_str());

    // wait until either the target event point is reached or the trigger event is notified
    sc_time t0 = sc_time_stamp();
    wait( getTimeTo(dir, target_event, m_progress), trigger_event | reset );
    sc_time elapsed = sc_time_stamp() - t0;

    m_progress = getProgressAt(dir, elapsed, m_progress);
    
    bool progress_reached = false;
    if constexpr ( dir == Direction::ASCENDING ) {
        progress_reached = m_progress >= getProgressAt(dir, target_event);
    } else if constexpr ( dir == Direction::DESCENDING ) {
        progress_reached = m_progress <= getProgressAt(dir, target_event);
    }

    SC_REPORT_INFO(this->name(), (head(sc_time_stamp()) 
        + "Event Wait Over, elapsed time: " + elapsed.to_string() 
        + ", target progress: " + std::to_string(getProgressAt(dir, target_event)) 
        + ", reset triggered: " + std::to_string(reset.triggered()) 
        + ", trigger event triggered: " + std::to_string(trigger_event.triggered())).c_str()
    );
    
    if ( reset.triggered() ) {
        // reset event triggered during the transition, go back to idle low state immediately
        output->write(false); // reset output to low
        SC_REPORT_INFO(this->name(), (head(sc_time_stamp()) + "<RESET> Hysteresis reset triggered").c_str());
        return &HysteresisBase::phase_idle_low;
    }

    if ( trigger_event.triggered() ) {
        // stop event triggered before reaching state flip point, start descending immediately
        SC_REPORT_INFO(this->name(), (head(sc_time_stamp()) + "<PREEMPT> Hysteresis preempted by trigger event").c_str());
        return on_interrupted;
    }
    
    if ( progress_reached ) {
        SC_REPORT_INFO(this->name(), (head(sc_time_stamp()) + "<PROGRESS> progress reached: " + (target_event == EventType::STATE_FLIP ? " (STATE_FLIP)" : " (COMPLETED)")).c_str());
        if ( target_event == EventType::STATE_FLIP ) {
            output->write( dir == Direction::ASCENDING ); // active if ascending, otherwise deactivate;
        }
        return on_successful;
    }
    
    SC_REPORT_FATAL(this->name(), (head(sc_time_stamp()) + "<ERROR> Encountered unexpected state in hysteresis phase_common()").c_str());
    return nullptr; // should never reach here
}


void HysteresisBase::phase_ascending_trigger() {
    // State 1: Ascending until the ascending state flip point
    m_current_state_fn = phase_common<Direction::ASCENDING>(
        EventType::STATE_FLIP, 
        trigger_descend, 
        &HysteresisBase::phase_ascending_complete, 
        &HysteresisBase::phase_descending_complete
    );
}
void HysteresisBase::phase_ascending_complete() {
    // State 2: Ascending until reaching the completion point
    m_current_state_fn = phase_common<Direction::ASCENDING>(
        EventType::COMPLETED, 
        trigger_descend, 
        &HysteresisBase::phase_idle_high, 
        &HysteresisBase::phase_descending_trigger
    );
}
void HysteresisBase::phase_descending_trigger() {
    // State 4: Descending until the descending state flip point
    m_current_state_fn = phase_common<Direction::DESCENDING>(
        EventType::STATE_FLIP, 
        trigger_ascend, 
        &HysteresisBase::phase_descending_complete, 
        &HysteresisBase::phase_ascending_complete
    );
}
void HysteresisBase::phase_descending_complete() {
    // State 5: Descending until reaching the completion point
    m_current_state_fn = phase_common<Direction::DESCENDING>(
        EventType::COMPLETED, 
        trigger_ascend, 
        &HysteresisBase::phase_idle_low, 
        &HysteresisBase::phase_ascending_trigger
    );
}


//------------------------------------------------------------------------------
// LinearHysteresis
//------------------------------------------------------------------------------

progress_t LinearHysteresis::getProgressAt(Direction dir, const sc_time& elapsed, progress_t progress) const {
    sc_time total_time = (dir == Direction::ASCENDING) ? m_time_ascend : m_time_descend;
    
    if (total_time == SC_ZERO_TIME) {
        return (dir == Direction::ASCENDING) ? 1.0f : 0.0f; // instant transition
    }
    
    progress_t new_progress = progress +  ((dir == Direction::ASCENDING) ? 1.0f : -1.0f) * progress_t(elapsed.to_double() / total_time.to_double());
    return std::clamp(new_progress, 0.0f, 1.0f);
}
progress_t LinearHysteresis::getProgressAt(Direction dir, EventType event) const {
    static const progress_t LUT[2 /*EventType*/][2 /*Direction*/] = {
        // ASCENDING             , DESCENDING
        { m_progress_ascend_flip, m_progress_descend_flip }, // STATE_FLIP
        { 1.0f                  , 0.0f                    }  // COMPLETED
    };

    return LUT[static_cast<size_t>(event)][static_cast<size_t>(dir)];
}
sc_time LinearHysteresis::getTimeTo(Direction dir, EventType event, progress_t progress) const {
    sc_time total_time = (dir == Direction::ASCENDING) ? m_time_ascend : m_time_descend;
    progress_t target_progress = getProgressAt(dir, event);
    
    if (dir == Direction::ASCENDING  && progress >= target_progress
     || dir == Direction::DESCENDING && progress <= target_progress) {
        SC_REPORT_WARNING("Hysteresis", ("Already at or past target event: target progress = " + std::to_string(target_progress) + ", current progress = " + std::to_string(progress)).c_str());
        return SC_ZERO_TIME; // already at or past the target progress
    }
    
    progress_t remaining_progress = (dir == Direction::ASCENDING) ? target_progress - progress : progress - target_progress;
    return remaining_progress * total_time;
}


//------------------------------------------------------------------------------
// ExponentialHysteresis
//------------------------------------------------------------------------------

progress_t ExponentialHysteresis::getProgressAt(Direction dir, const sc_time& elapsed, progress_t progress) const {
    sc_time reference_time = inverse_progress_function(dir, progress);
    sc_time now = reference_time + elapsed;
    return progress_function(dir, now);
}
progress_t ExponentialHysteresis::getProgressAt(Direction dir, EventType event) const {
    static const progress_t LUT[2 /*EventType*/][2 /*Direction*/] = {
        // ASCENDING             , DESCENDING
        { m_progress_ascend_flip, m_progress_descend_flip }, // STATE_FLIP
        { 1.0f                  , 0.0f                    }  // COMPLETED
    };

    return LUT[static_cast<size_t>(event)][static_cast<size_t>(dir)];
}
sc_time ExponentialHysteresis::getTimeTo(Direction dir, EventType event, progress_t progress) const {
    progress_t target_progress = getProgressAt(dir, event);
    
    if (dir == Direction::ASCENDING  && progress >= target_progress
     || dir == Direction::DESCENDING && progress <= target_progress) {
        SC_REPORT_WARNING("Hysteresis", ("Already at or past target event: target progress = " + std::to_string(target_progress) + ", current progress = " + std::to_string(progress)).c_str());
        return SC_ZERO_TIME; // already at or past the target progress
    }

    sc_time delta_time = time_between_progress(dir, progress, target_progress);
    return delta_time;
}

inline progress_t ExponentialHysteresis::progress_function(Direction dir, const sc_time& t) const {
    if (t <= SC_ZERO_TIME) {
        SC_REPORT_WARNING("Hysteresis", "Non-positive time input to progress function, returning completed state");
        return (dir == Direction::ASCENDING) ? 0.0f : 1.0f; // initial state
    }

    progress_t exp = progress_t(std::exp( -(t / mean_life_time) ));
    
    // Apply saturation
    if (dir == Direction::ASCENDING) {
        progress_t q = 1.0f - exp;
        return (q >= m_saturation_cutoff) ? 1.0f : q;
    } else {
        progress_t q = exp;
        return (q <= 1.0f - m_saturation_cutoff) ? 0.0f : q;
    }
}
inline sc_time ExponentialHysteresis::inverse_progress_function(Direction dir, progress_t p) const {
    if (dir == Direction::ASCENDING) {
        if (p >= m_saturation_cutoff) {
            return m_saturation_time; // return saturation time if progress is at or beyond cutoff
        }
        return (-1) * mean_life_time * std::log(1.0f - p);
    } else {
        if (p <= 1.0f - m_saturation_cutoff) {
            return m_saturation_time; // return saturation time if progress is at or beyond cutoff
        }
        return (-1) * mean_life_time * std::log(p);
    }
}
inline sc_time ExponentialHysteresis::time_between_progress(Direction dir, progress_t start, progress_t end) const {
    sc_time t_start = inverse_progress_function(dir, start);
    sc_time t_end = inverse_progress_function(dir, end);
    sc_time dt = t_end - t_start;
    return (dt > SC_ZERO_TIME) ? dt : SC_ZERO_TIME; // ensure non-negative time
}
inline sc_time ExponentialHysteresis::mean_life_time_from_cutoff(progress_t cutoff, sc_time saturation_time) const {
    if (cutoff <= 0.0f || cutoff >= 1.0f) {
        SC_REPORT_FATAL("Hysteresis", "Invalid cutoff value for mean life time calculation, returning default mean life time of 1 second");
        return SC_ZERO_TIME;
    }
    return (-1) * saturation_time / std::log(1.0f - cutoff);
}



}; // namespace hysteresis