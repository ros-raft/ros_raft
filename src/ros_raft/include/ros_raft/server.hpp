// Copyright 2022 John Farrell
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#ifndef ROS_RAFT__SERVER_HPP_
#define ROS_RAFT__SERVER_HPP_

#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/logger.hpp"

#include "ros_raft/state_machine.hpp"

#include "ros_raft_interfaces/srv/request_vote.hpp"
#include "ros_raft_interfaces/srv/append_entries.hpp"

namespace ros_raft
{

class Server : public rclcpp::Node
{
public:
  Server();

private:
  RaftStateMachine current_state_;

  rclcpp::Service<ros_raft_interfaces::srv::AppendEntries>::SharedPtr append_entries_srv_;
  rclcpp::Service<ros_raft_interfaces::srv::RequestVote>::SharedPtr request_vote_srv_;

  rclcpp::Client<ros_raft_interfaces::srv::AppendEntries>::SharedPtr append_entries_client_;
  rclcpp::Client<ros_raft_interfaces::srv::RequestVote>::SharedPtr request_vote_client_;

  /**
   * @brief Callback for AppendEntries RPC
   * 
   * @param req_header Request header. Ignored
   * @param request Request from leader
   * @param response Response to leader
   */
  void appendEntriesCallback(
    const std::shared_ptr<rmw_request_id_t> req_header,
    const std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Request> request,
    std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Response> response
  );

  /**
   * @brief Callback for RequestVote RPC
   * 
   * @param req_header Request header. Ignored
   * @param request Request from candidates
   * @param response Response to candidates
   */
  void requestVoteCallback(
    const std::shared_ptr<rmw_request_id_t> req_header,
    const std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Request> request,
    std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Response> response
  );

};

}  // namespace ros_raft

#endif  // ROS_RAFT__SERVER_HPP_
