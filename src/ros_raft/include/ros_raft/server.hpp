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
#include <memory>
#include <vector>

#include "rclcpp/logger.hpp"
#include "rclcpp/rclcpp.hpp"
#include "ros_raft/state_machine.hpp"
#include "ros_raft/log_entry.hpp"
#include "ros_raft_interfaces/srv/append_entries.hpp"
#include "ros_raft_interfaces/srv/request_vote.hpp"

namespace ros_raft
{

class Server : public rclcpp::Node
{
public:
  Server();

private:
  RaftStateMachine state_machine_;

  uint64_t candidate_id_;
  uint64_t current_term_;

  std::vector<LogEntry> log_;

  /**
   * @brief Callback group for all services
   */
  rclcpp::CallbackGroup::SharedPtr srv_cb_group_;
  /**
   * @brief Callback group for all clients
   */
  rclcpp::CallbackGroup::SharedPtr cli_cb_group_;
  /**
   * @brief Callback group for the election timer
   */
  rclcpp::CallbackGroup::SharedPtr tmr_cb_group_;

  /**
   * @brief Service that handles AppendEntries RPC calls from other servers
   */
  rclcpp::Service<ros_raft_interfaces::srv::AppendEntries>::SharedPtr append_entries_srv_;
  /**
   * @brief Service that handles RequestVote RPC calls from other servers
   */
  rclcpp::Service<ros_raft_interfaces::srv::RequestVote>::SharedPtr request_vote_srv_;

  /**
   * @brief Client for making AppendEntries RPC calls to other servers
   */
  rclcpp::Client<ros_raft_interfaces::srv::AppendEntries>::SharedPtr append_entries_client_;
  /**
   * @brief Client for making RequestVote RPC calls to other servers
   */
  rclcpp::Client<ros_raft_interfaces::srv::RequestVote>::SharedPtr request_vote_client_;

  /**
   * @brief Timer that handles starting new elections.
   */
  rclcpp::TimerBase::SharedPtr election_timer_;

  /**
   * @brief Registers all of the valid transition callbacks for the
   * internal state machine
   */
  void registerTransitions();

  /**
   * @brief Callback that gets triggered when the election timer expires.
   */
  void electionTimerCallback();

  /**
   * @brief Callback that handles the response from an AppendEntries RPC request
   * 
   * @param future 
   */
  void appendEntriesClientCallback(
    rclcpp::Client<ros_raft_interfaces::srv::RequestVote>::SharedFuture future);

  /**
   * @brief Callback for AppendEntries RPC requests
   * 
   * @param req_header Request header. Ignored
   * @param request Request from leader
   * @param response Response to leader
   */
  void appendEntriesCallback(
    const std::shared_ptr<rmw_request_id_t> req_header,
    const std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Request> request,
    std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Response> response);

  /**
   * @brief Callback for RequestVote RPC requests
   * 
   * @param req_header Request header. Ignored
   * @param request Request from candidates
   * @param response Response to candidates
   */
  void requestVoteCallback(
    const std::shared_ptr<rmw_request_id_t> req_header,
    const std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Request> request,
    std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Response> response);
};

}  // namespace ros_raft

#endif  // ROS_RAFT__SERVER_HPP_
