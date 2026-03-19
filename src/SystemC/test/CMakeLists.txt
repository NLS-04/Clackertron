# CMakeLists.txt for internal tests

# Enable testing
enable_testing()

if(MSVC)
    # Keep GoogleTest aligned with the parent runtime library settings on Windows.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_subdirectory(hysteresis)