# CMakeLists.txt for internal tests

add_executable(tst_hysteresis tst_hysteresis.cpp)
target_link_libraries(tst_hysteresis 
  relay_logic 
  SystemC::systemc
  GTest::GTest
)

add_test(hysteresis_quad_linear_full_cycle tst_hysteresis --gtest_filter=HysteresisTest.QuadLinearFullCycle)
add_test(hysteresis_quad_linear_preemptive tst_hysteresis --gtest_filter=HysteresisTest.QuadLinearPreemptedCycle)
add_test(hysteresis_dual_linear_cycle      tst_hysteresis --gtest_filter=HysteresisTest.DualLinearCycle)
add_test(hysteresis_linear_instant_ascend  tst_hysteresis --gtest_filter=HysteresisTest.LinearInstantAscend)
add_test(hysteresis_linear_instant_descend tst_hysteresis --gtest_filter=HysteresisTest.LinearInstantDescend)