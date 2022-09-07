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

#include "ros_raft/server.hpp"

ros_raft::Server::Server() : rclcpp::Node("test")
{
  RCLCPP_INFO(get_logger(), "Initialized ROS Raft server [ %s ]", get_name());

  srv_cb_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  append_entries_srv_ = create_service<ros_raft_interfaces::srv::AppendEntries>(
    std::string("/append_entries"),
    std::bind(
      &Server::appendEntriesCallback, this, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3),
    rmw_qos_profile_services_default, srv_cb_group_);

  request_vote_srv_ = create_service<ros_raft_interfaces::srv::RequestVote>(
    std::string("/request_vote"),
    std::bind(
      &Server::requestVoteCallback, this, std::placeholders::_1, std::placeholders::_2,
      std::placeholders::_3),
    rmw_qos_profile_services_default, srv_cb_group_);

  cli_cb_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  append_entries_client_ = create_client<ros_raft_interfaces::srv::AppendEntries>(
    std::string("/append_entries"), rmw_qos_profile_services_default, cli_cb_group_);

  request_vote_client_ = create_client<ros_raft_interfaces::srv::RequestVote>(
    std::string("/request_vote"), rmw_qos_profile_services_default, cli_cb_group_);

  registerTransitions();

  tmr_cb_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  election_timer_ = create_wall_timer(
    std::chrono::duration(std::chrono::milliseconds(
      200)),  // TODO(johnfarrell) make this duration random according to Raft algo
    std::bind(&Server::electionTimerCallback, this), tmr_cb_group_);
}

void ros_raft::Server::registerTransitions()
{
  state_machine_.RegisterTransitionCallback(
    ros_raft::StateTransition(ros_raft::NodeState::FOLLOWER, ros_raft::NodeState::CANDIDATE),
    [&](const ros_raft::StateTransition & state) {
      (void)state;
      RCLCPP_INFO(this->get_logger(), "Transitioning to CANDIDATE from FOLLOWER");
      this->current_term_++;

      auto req_vote_msg = std::make_shared<ros_raft_interfaces::srv::RequestVote::Request>();
      req_vote_msg->term = this->current_term_;
      req_vote_msg->candidate_id = this->candidate_id_;

      if (!request_vote_client_->wait_for_service(std::chrono::milliseconds(20))) {
        RCLCPP_ERROR(this->get_logger(), "Failed to wait for service");
        return;
      }
      request_vote_client_->async_send_request(
        req_vote_msg,
        std::move(std::bind(&Server::appendEntriesClientCallback, this, std::placeholders::_1)));
    });
}

void ros_raft::Server::appendEntriesClientCallback(
  rclcpp::Client<ros_raft_interfaces::srv::RequestVote>::SharedFuture future)
{
  (void)future;
  RCLCPP_INFO(this->get_logger(), "recieved response from service");
}

void ros_raft::Server::electionTimerCallback()
{
  if (state_machine_.GetState() == ros_raft::NodeState::FOLLOWER) {
    state_machine_.TransitionState(ros_raft::NodeState::CANDIDATE);
    election_timer_->reset();
    return;
  }
}

void ros_raft::Server::appendEntriesCallback(
  const std::shared_ptr<rmw_request_id_t> req_header,
  const std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Request> request,
  std::shared_ptr<ros_raft_interfaces::srv::AppendEntries::Response> response)
{
  (void)req_header;
  (void)request;
  (void)response;
}

void ros_raft::Server::requestVoteCallback(
  const std::shared_ptr<rmw_request_id_t> req_header,
  const std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Request> request,
  std::shared_ptr<ros_raft_interfaces::srv::RequestVote::Response> response)
{
  (void)req_header;
  RCLCPP_INFO(get_logger(), "Request vote callback");

  response->term = request->term;
}
