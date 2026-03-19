# relay_logic target

add_library(relay_logic STATIC
    hysteresis.cpp
)

target_include_directories(relay_logic
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(relay_logic
	PUBLIC
		SystemC::systemc
)
