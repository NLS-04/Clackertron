#include "tst_hyst_common.h"

#define TOLERANCE sc_time(10, SC_US) // tolerance for accumulated rounding errors in timing assertions during preemption

TEST(HysteresisTest, QuadLinearFullCycle) {
    LinearHysteresis lh("LH", sc_time(10, SC_MS), sc_time(10, SC_MS), 0.75f, 0.25f);
    TestBench tb("TB", &lh);

    // full stop events:
    tb.trigger_ascend.notify( sc_time(  5, SC_MS));  // trigger ascend  at  5ms, should flip at  5ms + 7.5ms = 12.5ms (75% of progress)
    tb.trigger_descend.notify(sc_time( 20, SC_MS)); // trigger descend at 20ms, should flip at 20ms + 7.5ms = 27.5ms (25% of progress)

    sc_start(sc_time(15, SC_MS));

    EXPECT_EQ(tb.get_events().size(), 1);
    tb.get_events()[0].assert_eq(HystEvent{sc_time(12.5, SC_MS), Direction::ASCENDING}, TOLERANCE);
}

TEST(HysteresisTest, QuadLinearPreemptedCycle) {
    LinearHysteresis lh("LH", sc_time(10, SC_MS), sc_time(10, SC_MS), 0.75f, 0.25f);
    TestBench tb("TB", &lh);

    // preempt the ascend phase with a descend trigger before it flips:
    tb.trigger_ascend.notify( sc_time(  0, SC_MS)); // trigger ascend  at 0ms, would   flip at  0ms + 7.5ms = 7.5ms (75% of progress)
    tb.trigger_descend.notify(sc_time(  5, SC_MS)); // trigger descend at 5ms, preempt flip

    // trigger flip up, then preempt the descend phase with a descend trigger before it flips:
    tb.trigger_ascend.notify( sc_time( 10, SC_MS)); // trigger ascend  at 10ms, must   flip at  10ms + 7.5ms = 17.5ms (75% of progress)
    tb.trigger_descend.notify(sc_time( 18, SC_MS)); // trigger descend at 18ms, would  flip at  18ms + 7.5ms = 25.5ms (25% of progress)
    tb.trigger_ascend.notify( sc_time( 22, SC_MS)); // trigger ascend  at 22ms, preempt flip at 25.5ms
    tb.trigger_descend.notify(sc_time( 24, SC_MS)); // trigger descend at 24ms, progress should be 2ms/10ms = 0.2 at 24ms => must flip at 27.5ms

    sc_start(sc_time(30, SC_MS));

    EXPECT_GE(sc_time_stamp(), sc_time(30, SC_MS));
    EXPECT_EQ(tb.get_events().size(), 2);
    tb.get_events()[0].assert_eq(HystEvent{sc_time(17.5, SC_MS), Direction::ASCENDING }, TOLERANCE);
    tb.get_events()[1].assert_eq(HystEvent{sc_time(27.5, SC_MS), Direction::DESCENDING}, TOLERANCE);
}

TEST(HysteresisTest, DualLinearCycle) {
    LinearHysteresis lh("LH", sc_time(10, SC_MS), sc_time(10, SC_MS), 1.0f, 0.0f);
    TestBench tb("TB", &lh);

    // full cycle ascend-descend phase:
    tb.trigger_ascend.notify( sc_time(  0, SC_MS)); // trigger ascend  at  0ms, must flip at 10ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 11, SC_MS)); // trigger descend at 11ms, must flip at 21ms (  0% of progress)

    // preempt the ascend phase with a descend trigger before it flips:
    tb.trigger_ascend.notify( sc_time( 25, SC_MS)); // trigger ascend  at 25ms, would flip at 35ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 30, SC_MS)); // trigger descend at 30ms, preempt flip
    tb.trigger_ascend.notify( sc_time( 33, SC_MS)); // trigger ascend  at 33ms, would flip at 35ms + 2*3ms = 41ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 35, SC_MS)); // trigger descend at 35ms, preempt flip
    tb.trigger_ascend.notify( sc_time( 36, SC_MS)); // trigger ascend  at 36ms, must  flip at 41ms + 2*1ms = 43ms (100% of progress)

    sc_start(sc_time(50, SC_MS));

    EXPECT_GE(sc_time_stamp(), sc_time(50, SC_MS));
    EXPECT_EQ(tb.get_events().size(), 3);
    tb.get_events()[0].assert_eq(HystEvent{sc_time(10, SC_MS), Direction::ASCENDING }, TOLERANCE);
    tb.get_events()[1].assert_eq(HystEvent{sc_time(21, SC_MS), Direction::DESCENDING}, TOLERANCE);
    tb.get_events()[2].assert_eq(HystEvent{sc_time(43, SC_MS), Direction::ASCENDING }, TOLERANCE);
}

TEST(HysteresisTest, LinearInstantAscend) {
    LinearHysteresis lh("LH", sc_time(0, SC_MS), sc_time(10, SC_MS), 1.0f, 0.0f);
    TestBench tb("TB", &lh);

    // full ascend-descend cycle:
    tb.trigger_ascend.notify( sc_time(  0, SC_MS)); // trigger ascend  at  0ms, must flip at  0ms (100% of progress)
    tb.trigger_descend.notify(sc_time(  1, SC_MS)); // trigger descend at  1ms, must flip at 11ms (  0% of progress)

    // preempt the descend phase with a ascend trigger before it flips:
    tb.trigger_ascend.notify( sc_time( 12, SC_MS)); // trigger ascend  at 12ms, must  flip at 12ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 13, SC_MS)); // trigger descend at 13ms, would flip at 23ms (  0% of progress)
    tb.trigger_ascend.notify( sc_time( 15, SC_MS)); // trigger ascend  at 15ms, preempt flip
    tb.trigger_descend.notify(sc_time( 16, SC_MS)); // trigger descend at 16ms, must flip at 26ms (  0% of progress)

    sc_start(sc_time(30, SC_MS));

    EXPECT_GE(sc_time_stamp(), sc_time(30, SC_MS));
    EXPECT_EQ(tb.get_events().size(), 4);
    tb.get_events()[0].assert_eq(HystEvent{sc_time( 0, SC_MS), Direction::ASCENDING }, TOLERANCE);
    tb.get_events()[1].assert_eq(HystEvent{sc_time(11, SC_MS), Direction::DESCENDING}, TOLERANCE);
    tb.get_events()[2].assert_eq(HystEvent{sc_time(12, SC_MS), Direction::ASCENDING }, TOLERANCE);
    tb.get_events()[3].assert_eq(HystEvent{sc_time(26, SC_MS), Direction::DESCENDING}, TOLERANCE);
}

TEST(HysteresisTest, LinearInstantDescend) {
    LinearHysteresis lh("LH", sc_time(10, SC_MS), sc_time(0, SC_MS), 1.0f, 0.0f);
    TestBench tb("TB", &lh);

    // full ascend-descend cycle:
    tb.trigger_ascend.notify( sc_time(  0, SC_MS)); // trigger ascend  at  0ms, must flip at 10ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 11, SC_MS)); // trigger descend at 11ms, must flip at 11ms (  0% of progress)

    // preempt the descend phase with a ascend trigger before it flips:
    tb.trigger_ascend.notify( sc_time( 12, SC_MS)); // trigger ascend  at 12ms, would flip at 22ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 15, SC_MS)); // trigger descend at 15ms, preempt flip
    tb.trigger_ascend.notify( sc_time( 16, SC_MS)); // trigger ascend  at 16ms, must flip at 26ms (100% of progress)
    tb.trigger_descend.notify(sc_time( 27, SC_MS)); // trigger descend at 27ms, must flip at 27ms (  0% of progress)

    sc_start(sc_time(50, SC_MS));

    EXPECT_GE(sc_time_stamp(), sc_time(50, SC_MS));
    EXPECT_EQ(tb.get_events().size(), 4);
    tb.get_events()[0].assert_eq(HystEvent{sc_time(10, SC_MS), Direction::ASCENDING }, TOLERANCE);
    tb.get_events()[1].assert_eq(HystEvent{sc_time(11, SC_MS), Direction::DESCENDING}, TOLERANCE);
    tb.get_events()[2].assert_eq(HystEvent{sc_time(26, SC_MS), Direction::ASCENDING }, TOLERANCE);
    tb.get_events()[3].assert_eq(HystEvent{sc_time(27, SC_MS), Direction::DESCENDING}, TOLERANCE);
}