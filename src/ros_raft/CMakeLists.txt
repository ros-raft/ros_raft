cmake_minimum_required(VERSION 3.8)
project(ros_raft)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(ros_raft_interfaces REQUIRED)
find_package(GTest CONFIG REQUIRED)

include_directories(
  include
)

set(EXECUTABLE_NAME "ros_raft_server")
set(LIBRARY_NAME "${EXECUTABLE_NAME}-core")

# Create core library
add_library(${LIBRARY_NAME}
  src/server.cpp
  src/server_state_machine.cpp
)

# Create executable
add_executable(${EXECUTABLE_NAME} src/main.cpp)


# Link the core library to the executable
target_link_libraries(${EXECUTABLE_NAME} ${LIBRARY_NAME})


ament_target_dependencies(${LIBRARY_NAME} rclcpp ros_raft_interfaces)

# Install library and executable
install(TARGETS ${LIBRARY_NAME}
  LIBRARY DESTINATION lib
)

install(DIRECTORY include/
  DESTINATION include/
)

install(TARGETS ${EXECUTABLE_NAME}
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  add_subdirectory(test)
endif()

ament_export_include_directories(include)
ament_export_dependencies(rclcpp ros_raft_interfaces)

ament_package()
