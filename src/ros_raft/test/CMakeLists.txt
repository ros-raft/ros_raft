ament_add_gtest(state_machine_test
  state_machine_test.cpp
)
target_link_libraries(state_machine_test
  ${LIBRARY_NAME}
)