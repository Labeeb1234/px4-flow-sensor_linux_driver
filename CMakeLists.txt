cmake_minimum_required(VERSION 3.10)
project(optical_flow)

find_library(I2C_LIB i2c) # currently this lib is unused make use it if required (check feasibility)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
   add_compile_options(-Wall -Wextra -Wpedantic)
endif()


add_executable(test_optical_flow src/test.cpp)
target_include_directories(test_optical_flow
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(test_optical_flow 
        PRIVATE
        ${I2C_LIB}
)
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

install(
  TARGETS
    test_optical_flow
  DESTINATION lib/${PROJECT_NAME}
)

