ament_add_gtest(server_state_machine_test
  server_state_machine_test.cpp
)
target_link_libraries(server_state_machine_test
  ${LIBRARY_NAME}
)