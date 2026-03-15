# relay_logic target

# This project is header-only at the moment (only relay.h exists),
# so provide an INTERFACE target so top-level `target_link_libraries`
# can reference `relay_logic` without producing a .lib file.

add_library(relay_logic INTERFACE)

target_include_directories(relay_logic
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)
