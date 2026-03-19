cmake_minimum_required(VERSION 3.24)
project(Clackertron VERSION 0.1.0 LANGUAGES C CXX)

option(ARTIFACTS_ENABLE_AMS OFF)
option(TEST OFF)

include(CTest)
include(FetchContent)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /std:c++20")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ Standard")
set(CMAKE_CXX_STANDARD_REQUIRED 20 CACHE STRING "C++ Standard")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_CXX_STANDARD_REQUIRED: ${CMAKE_CXX_STANDARD_REQUIRED}")

FetchContent_Declare(systemc
    GIT_REPOSITORY https://github.com/accellera-official/systemc.git
    GIT_TAG 3.0.1
)

FetchContent_MakeAvailable(systemc)

add_subdirectory(relay_logic)
# add_subdirectory(assembler)

add_executable(clackertron clackertron.cpp)
target_link_libraries(clackertron 
  relay_logic 
  # assembler 
  SystemC::systemc
)

# if(TEST)
message(STATUS "Tests enabled")
add_subdirectory(test)
# endif()


if(ARTIFACTS_ENABLE_AMS)
  FetchContent_Declare(systemc_ams
    URL https://www.coseda-tech.com/files/Files/Proof-of-Concepts/systemc-ams-2.3.4.tar.gz )
  
  FetchContent_MakeAvailable(systemc_ams)

  target_include_directories(systemc-ams PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(systemc-ams PUBLIC ${systemc_ams_SOURCE_DIR})
  target_include_directories(systemc-ams PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(impl_ac PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(impl_ac PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(core PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(core PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(conservative PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(conservative PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(eln PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(eln PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(impl_lsf PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(impl_lsf PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(impl_tdf PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(impl_tdf PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(linear PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(linear PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(solvertdf PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(solvertdf PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(user_solver PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(user_solver PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(sparse_library PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(sparse_library PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(synchronization PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(synchronization PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(data_types PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(data_types PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(reporting PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(reporting PUBLIC ${systemc_SOURCE_DIR}/src)
  target_include_directories(tracing PUBLIC ${systemc_ams_SOURCE_DIR}/src)
  target_include_directories(tracing PUBLIC ${systemc_SOURCE_DIR}/src)

  add_subdirectory(ams-eln)
  add_subdirectory(ams-eln2)
  add_subdirectory(ams-tdf)
  add_subdirectory(ams-tdf2)
  add_subdirectory(ams-lsf)
  add_subdirectory(ams-lsf2)
endif()